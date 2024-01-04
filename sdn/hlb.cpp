#include "hlb.h"

// Algorithm 1 from Thesis
void computeLb(int sigma[], int inv[], int lb[], int updated[], int N, int i) {
    // keeping visited elements
    int vis[N + 1];
    for(int i = 1; i <= N; i++) {
        vis[i] = 0;
    }

    // where does the dashed edge from i point to
    int j = sigma[inv[i] + 1];
    // j < i should hold because i is backward edge
    if(j >= i) {
        printf("Error");
    }

    vis[i] = 1;

    int k = j;
    lb[i] = 1e5;
    while(1) {
        // we could arrive at destination without running into any cycle, so 1 round suffices
        if(k == N) {
            lb[i] = 1;
            break;
        }

        // k is visited
        vis[k] = 1;

        if(updated[k]) {
            int k_next = sigma[inv[k] + 1];
            if(!vis[k_next]) {
                k = k_next;
                continue;
            }
            else {
                break;
            }
        }
        else {
            // backward edge
            if(!isForward(computeTypeOfVertex(sigma, inv, N, k))) {
                int k_next = sigma[inv[k] + 1];
                // this backward edge is valid
                if(!vis[k_next]) {
                    lb[i] = min(lb[i], lb[k] + 1);
                }
            }
            // forward edge
            else {
                int k_next = sigma[inv[k] + 1];
                // this forward edge is valid
                if(!vis[k_next]) {
                    lb[i] = 2;
                }
            }

            if(!vis[k + 1]) {
                k = k + 1;
                continue;
            }
            else {
                break;
            }
        }
    }
}

// Algorithm 2 from Thesis
int computeHighestLowerBound(int sigma[], int inv[], int updated[], int N) {

    /* 
        lower bounds for backward edges
        lower bound for # rounds required to be able to update this edge
        can be smaller than true minimum number of rounds required
    */
    int lb[N + 1];
    for(int i = 1; i <= N; i++) {
        lb[i] = 0;
    }

    for(int i = 2; i <= N - 1; i++) {
        // is backward
        if(!isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            computeLb(sigma, inv, lb, updated, N, i);
            // printf("Lower bound for reverse edge %d is: %d\n", i, lb[i]);
        }
    }

    int res = lb[1];
    for(int i = 2; i <= N - 1; i++) {
        if(res < lb[i]) {
            res = lb[i];
        }
    }
    return res;
}

void hlbAlgorithm_optOut(int sigma[], int inv[], int N) {
    // already updated nodes
    int updated[N + 1];
    for(int i = 1; i <= N; i++) {
        updated[i] = 0;
    }

    // consider all forward edges already updated
    updated[1] = 1;
    for(int i = 2; i <= N - 1; i++) {
        // is forward
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            updated[i] = 1;
        }
    }

    int hlb = computeHighestLowerBound(sigma, inv, updated, N);
    printf("Highest lower bound is: %d\n", hlb);


    for(int i = 2; i <= N - 1; i++) {
        // is forward
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            updated[i] = 0;
            int newhlb = computeHighestLowerBound(sigma, inv, updated, N);
            printf("Highest lower bound without forward edge %d is: %d\n", i, newhlb);
            if(newhlb < hlb) {
                hlb = newhlb;
                printf("Gave up on forward edge %d\n", i);
            }
            else {
                updated[i] = 1;
            }
        }
    }
}

void hlbAlgorithm_optIn(int sigma[], int inv[], int N) {
    // already updated nodes
    int updated[N + 1];
    for(int i = 1; i <= N; i++) {
        updated[i] = 0;
    }

    int hlb = computeHighestLowerBound(sigma, inv, updated, N);
    printf("Highest lower bound is: %d\n", hlb);

    for(int i = 2; i <= N - 1; i++) {
        // is forward
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            updated[i] = 1;
            int newhlb = computeHighestLowerBound(sigma, inv, updated, N);
            printf("Highest lower bound with forward edge %d is: %d\n", i, newhlb);
            if(newhlb <= hlb) {
                hlb = newhlb;
                printf("Added forward edge %d\n", i);
            }
            else {
                updated[i] = 0;
            }
        }
    }
}

// used to test hlb in paper
void compute_hlb_statistics(int sigma[], int inv[], int N, int numtries, int randomseed) {
    ofstream fileoutput;
    fileoutput.open("results.out");
    
    // avoiding a random seed is useful for reproducibility
    if(randomseed) {
        srand(time(NULL));
    }
    else {
        srand(100);
    }
    sigma[1] = 1;
    sigma[N] = N;
    vector<int> myvector;

    vector<int> hlb_res;
    for(int i = 2; i <= N - 1; i++) {
        myvector.push_back(i);
    }
    // generating random permutations
    for(int ppp = 1; ppp <= numtries; ppp++) {
        loop:
        random_shuffle(myvector.begin(), myvector.end());
        for(int index = 0; index < myvector.size(); index++) {
            sigma[index + 2] = myvector.at(index);
        }

        for(int i = 1; i <= N; i++) {        
            inv[sigma[i]] = i;
        }

        for(int i = 1; i <= N; i++) {
            if(i != 1 && (sigma[i] == sigma[i - 1] + 1)) {
                goto loop;
            }
        }

        for(int i = 1; i <= N; i++) {
            fileoutput << sigma[i] << " ";
        }
        
        int updated[N + 1];
        for(int i = 1; i <= N; i++) {
            updated[i] = 0;
        }
        int ls_res = computeHighestLowerBound(sigma, inv, updated, N);
        fileoutput << "\nHLB: " << ls_res << "\n\n";
        hlb_res.push_back(ls_res);
    }

    // aggregating output
    fileoutput << "HLB: ";
    for(auto j = hlb_res.begin(); j != hlb_res.end(); j++) {
        fileoutput << *j << ",";
    }

    fileoutput.close();
}