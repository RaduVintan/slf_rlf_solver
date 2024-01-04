#include "utilities.h"

void readInput(int sigma[], int inv[], int N) {
    for(int i = 1; i <= N; i++) {
        scanf("%d", &sigma[i]);
        if(i != 1 && (sigma[i] == sigma[i - 1] + 1)) {
            printf("Invalid permutation at position %d!\n", i);
        }
    }
    for(int i = 1; i <= N; i++) {        
        inv[sigma[i]] = i;
    }
}

int isInside(int k, uset &my_set) {
    return (my_set.find(k) != my_set.end());
}

int isForward(int type) {
    return type < 2;
}

/* 
    i = index of vertex whose type we want to compute
    outputs: 0 if FF, 1 if FB, 2 if BF, 3 if BB
*/
int computeTypeOfVertex(int sigma[], int inv[], int N, int i)  {
    int res = 0;
    if(sigma[inv[i] + 1] < i) {
        res += 2;
    }
    if(inv[i] > inv[i + 1]) {
        res += 1;
    }
    return res;
}

// basically toposort
int checkValidityOfUpdate(int sigma[], int inv[], int N, uset &alreadyUpdated, uset &toUpdate) {
    int deg[N + 1];
    memset(deg, 0, sizeof(deg));

    for(int i = 1; i <= N - 1; i++) {
        if(isInside(i, alreadyUpdated)) {
            int k = sigma[inv[i] + 1];
            deg[k]++;
        }
        else if(isInside(i, toUpdate)) {
            int k = sigma[inv[i] + 1];
            deg[k]++;
            deg[i + 1]++;
        }
        else {
            deg[i + 1]++;
        }
    }

    stack<int> nodes;
    for(int i = 1; i <= N; i++) {
        if(deg[i] == 0) {
            nodes.push(i);
        }
    }

    while(!nodes.empty()) {
        int v = nodes.top();
        nodes.pop();
        if(v == N) {
            continue;
        }

        if(isInside(v, alreadyUpdated)) {
            int k = sigma[inv[v] + 1];
            deg[k]--;
            if(deg[k] == 0) {
                nodes.push(k);
            }
        }
        else if(isInside(v, toUpdate)) {
            int k = sigma[inv[v] + 1];
            deg[k]--;
            if(deg[k] == 0) {
                nodes.push(k);
            }
            deg[v + 1]--;
            if(deg[v + 1] == 0) {
                nodes.push(v + 1);
            }
        }
        else {
            deg[v + 1]--;
            if(deg[v + 1] == 0) {
                nodes.push(v + 1);
            }
        }
    }

    for(int i = 1; i <= N; i++) {
        if(deg[i]) {
            return 0;
        }
    }
    return 1;
}

int checkValidityOfSchedule(int sigma[], int inv[], int N, schedule &schedule) {
    uset alreadyUpdated;

    int k = schedule.size();
    for(int i = 0; i < k; i++) {
        uset roundi = schedule.at(i);

        if(!checkValidityOfUpdate(sigma, inv, N, alreadyUpdated, roundi)) {
            return 0;
        }

        for(auto j = roundi.begin(); j != roundi.end(); j++) {
            alreadyUpdated.insert(*j);
        }
    }

    return 1;
}

int checkPropertyForRandomPermutations(int N, int numtries,
    int (*property)(int[], int[], int)) {
    int sigma[N + 1];
    int inv[N + 1];

    srand(time(NULL));
    sigma[1] = 1;
    sigma[N] = N;
    vector<int> myvector;
    for(int i = 2; i <= N - 1; i++) {
        myvector.push_back(i);
    }
    for(int ppp = 1; ppp <= numtries; ppp++) {
        loop:
        random_shuffle(myvector.begin(), myvector.end());
        for(int index = 0; index < myvector.size(); index++) {
            sigma[index + 2] = myvector.at(index);
        }

        for(int i = 1; i <= N; i++) {        
            inv[sigma[i]] = i;
            if(i != 1 && (sigma[i] == sigma[i - 1] + 1)) {
                goto loop;
            }
        }

        if(!property(sigma, inv, N)) {
            ofstream outputfile;
            outputfile.open("found.out");
            for(int index = 1; index <= N; index++){
                outputfile << sigma[index] << " ";
            }
            return 0;
        }
    }
    return 1;
}