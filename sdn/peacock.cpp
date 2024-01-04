#include "peacock.h"

// does the forward edge (l,r) not cross the other forward edges encoded in lb + ub
int doesNotCross(int l, int r, vector<int> lb, vector<int> ub) {
    for(int j = 0; j < lb.size(); j++) {
        if( (l < lb[j] && lb[j] < r) ||
            (l < ub[j] && ub[j] < r) ||
            (l > lb[j] && ub[j] > r)) {
                return 0;
            }
        if(l == lb[j] && r == ub[j]) {
            return 0;
        }   
    }
    return 1;
}

// application of Main Lemma from Thesis
// set "updated" has been obtained as {1,...,n-1} \ Pold for some 1-n-path P in the full graph
// this function computes the reduced instance G(updated)
vector<int> getUpdatedInstance(int sigma[], int inv[], int N, uset updated) {
    int nodeToNewNode[N + 1];
    int index = 1;
    int node;
    for(int i = 1; i <= N; i++) {
        if(!isInside(i, updated)) {
            nodeToNewNode[i] = index;
            if(index == 1) {
                node = i;
            }
            index++;
        }
    }
    
    int newN = N - updated.size();
    vector<int>newsigma;

    if(newN == 1) {
        newsigma.push_back(1);
        return newsigma;
    }

    for(index = 1;; index++) {
        newsigma.push_back(nodeToNewNode[node]);
        if(index == newN) {
            break;
        }
        do {
            node = sigma[inv[node] + 1];
        }while(isInside(node, updated));
    }
    return newsigma;
}

int peacock(int sigma[], int inv[], int N, int print) {
    if(N == 1) {
        return 0;
    }

    vector<int> forwards, dists;

    // storing forward edges including their length
    for(int i = 1; i < N; i++) {
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            forwards.push_back(i);
            dists.push_back(sigma[inv[i] + 1] - i);
        }
    }

    // sorting them in descending order
    for(int i = 0; i < forwards.size(); i++) {
        for(int j = i + 1; j < forwards.size(); j++) {
            if(dists[i] < dists[j]) {
                swap(dists[i], dists[j]);
                swap(forwards[i], forwards[j]);
            }
        }
    }

    // updated corresponds to set S from the Main Lemma
    uset updated;
    vector<int> lb, ub;

    if(print) {
        printf("Updated forwards: ");
    }
    for(int i = 0; i < forwards.size(); i++) {
        int l = forwards[i];
        int r = sigma[inv[l] + 1];

        // checking if we can add edge
        if(!doesNotCross(l, r, lb, ub)) {
            continue;
        }

        // all nodes we jump over will be updated
        if(print) {
            printf("%d ", l);
        }
        for(int k = l; k < r; k++) {
            updated.insert(k);
        }
        lb.push_back(l);
        ub.push_back(r);
    }

    // computing reduced instance and storing it in arrays
    vector<int> newsigma_v = getUpdatedInstance(sigma, inv, N, updated);
    int newN = newsigma_v.size();
    if(newN == 1) {
        if(print) {
            printf("\n");
        }
        return 1;
    }

    if(print) {
        printf("\nObtained reduced instance: ");
    }

    int newsigma[newN + 1], newinv[newN + 1];
    for(int i = 1; i <= newN; i++) {
        newsigma[i] = newsigma_v[i - 1];
        if(print) {
            printf("%d ", newsigma[i]);
        }
    }
    for(int i = 1; i <= newN; i++) {        
        newinv[newsigma[i]] = i;
    }

    if(print) {
        printf("\n");
    }

    // recursive call
    return 2 + peacock(newsigma, newinv, newN, print);
} 

// local search for rlf, presented in Thesis at Subsection 4.2.2
int improvedPeacock(int sigma[], int inv[], int N, int print) {
    if(N == 1) {
        return 0;
    }

    vector<int> forwards, dists;

    for(int i = 1; i < N; i++) {
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            forwards.push_back(i);
            dists.push_back(sigma[inv[i] + 1] - i);
        }
    }

    // uncomment if you want to start with peacock solution
    /*for(int i = 0; i < forwards.size(); i++) {
        for(int j = i + 1; j < forwards.size(); j++) {
            if(dists[i] < dists[j]) {
                swap(dists[i], dists[j]);
                swap(forwards[i], forwards[j]);
            }
        }
    }*/

    // updated corresponds to set S from the Main Lemma
    uset updated;
    vector<int> lb, ub;

    for(int i = 0; i < forwards.size(); i++) {
        int l = forwards[i];
        int r = sigma[inv[l] + 1];

        // checking if we can add edge
        if(!doesNotCross(l, r, lb, ub)) {
            continue;
        }

        for(int k = l; k < r; k++) {
            updated.insert(k);
        }
        lb.push_back(l);
        ub.push_back(r);
    }

    vector<int> newsigma_v = getUpdatedInstance(sigma, inv, N, updated);
    int newN = newsigma_v.size();
    if(newN == 1) {
        if(print) {
            printf("Update everything in 1 more round.\n");
        }
        return 1;
    }
    int newsigma[N + 1], newinv[N + 1];
    for(int i = 1; i <= newN; i++) {
        newsigma[i] = newsigma_v[i - 1];
    }
    for(int i = 1; i <= newN; i++) {        
        newinv[newsigma[i]] = i;
    }

    int bestTime = 2 + improvedPeacock(newsigma, newinv, newN, 0);
    uset bestUpdate = updated;

    while(true) {  
        int foundBetter = 0;
        for(int i = 0; i < forwards.size(); i++) {
            int node = forwards[i];

            int ok = 1;
            for(int j = 0; j < lb.size(); j++) {
                if(node == lb[j]) {
                    ok = 0;
                    break;
                }
            }
            if(!ok) {
                continue;
            }

            vector<int> new_lb, new_ub;
            uset newUpdated;

            for(int j = node; j < sigma[inv[node] + 1]; j++) {
                newUpdated.insert(j);
            }
            new_lb.push_back(node);
            new_ub.push_back(sigma[inv[node] + 1]);

            for(int i = 0; i < lb.size(); i++) {
                int l = lb[i];
                int r = ub[i];

                if(!doesNotCross(l, r, new_lb, new_ub)) {
                    continue;
                }

                for(int k = l; k < r; k++) {
                    newUpdated.insert(k);
                }
                new_lb.push_back(l);
                new_ub.push_back(r);
            }

            for(int i = 0; i < forwards.size(); i++) {
                int l = forwards[i];
                int r = sigma[inv[l] + 1];

                if(!doesNotCross(l, r, new_lb, new_ub)) {
                    continue;
                }

                for(int k = l; k < r; k++) {
                    newUpdated.insert(k);
                }
                new_lb.push_back(l);
                new_ub.push_back(r);
            }                

            newsigma_v = getUpdatedInstance(sigma, inv, N, newUpdated);
            newN = newsigma_v.size();
            for(int i = 1; i <= newN; i++) {
                newsigma[i] = newsigma_v[i - 1];
            }
            for(int i = 1; i <= newN; i++) {        
                newinv[newsigma[i]] = i;
            }

            int newTime; 
            if(newN == 1) {
                newTime = 1;
            } 
            else {
                newTime = 2 + improvedPeacock(newsigma, newinv, newN, 0);
            }

            if(newTime < bestTime) {
                bestTime = newTime;
                updated = newUpdated;
                lb = new_lb;
                ub = new_ub;
                foundBetter = 1;
            }
        }
        if(!foundBetter) {
            break;
        }
    }

    if(print) {
        printf("Updated forwards: ");
        for(int i = 0; i < lb.size(); i++) {
            printf("%d ", lb[i]);
        }
        printf("\n");
    }
    
    if(print) {
        printf("Obtained reduced instance: ");
    }
    newsigma_v = getUpdatedInstance(sigma, inv, N, updated);
    newN = newsigma_v.size();
    for(int i = 1; i <= newN; i++) {
        newsigma[i] = newsigma_v[i - 1];
        if(print) {
            printf("%d ", newsigma[i]);
        }
    }
    for(int i = 1; i <= newN; i++) {        
        newinv[newsigma[i]] = i;
    }

    if(print) {
        printf("\n");
    }

    if(newN == 1) {
        return 1;
    }
    else {
        if(print) {
            return 2 + improvedPeacock(newsigma, newinv, newN, 1);
        }
        else {
            return bestTime;
        }
    }
}

// Short Path Heuristic from Thesis
int shortest_path_heuristic(int sigma[], int inv[], int N, int print) {
    if(N == 1) {
        return 0;
    }

    // implementation of dijkstra for computing shortest path
    // old edges and backward edges have cost of 1
    // forward edges have cost 0

    struct comparator {
        bool operator() (const pair<int,int> &a, const pair<int,int> &b) {
            if(a.second != b.second) {
                return a.second < b.second;
            }
            else {
                return a.first < b.first;
            }
        }
    };

    vector<pair<int,int>> G[N + 1];
    int addcost[N + 1];
    int parent[N + 1];

    int BigCap = N;
    
    for(int i = 1; i <= N; i++) {
        addcost[i] = BigCap;
    }
    addcost[1] = 0;
	
    set<pair<int,int>,comparator> myset;

    for(int i = 1; i <= N - 1; i++) {
        G[i].push_back(make_pair(i + 1, 1));
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            G[i].push_back(make_pair(sigma[inv[i] + 1], 0));
        }
        else {
            G[i].push_back(make_pair(sigma[inv[i] + 1], 1));
        }

        if(i == 1) {
            addcost[i + 1] = 1;
            addcost[sigma[inv[i] + 1]] = 0;
            myset.insert(make_pair(i + 1, 1));
            myset.insert(make_pair(sigma[inv[i] + 1], 0));
            parent[i + 1] = 1;
            parent[sigma[inv[i] + 1]] = 1;
        }
    }

    for(int nodes = 1; nodes <= N - 1; nodes++) {
        pair<int,int> p = *myset.begin();
        int v = p.first;

        addcost[v] = 0;
        myset.erase(p);

        for(auto w = G[v].begin(); w != G[v].end(); w++) {
            if(addcost[w->first] == 0) {
                continue;
            }
            if(w->second < addcost[w->first]) {
                myset.erase(make_pair(w->first, addcost[w->first]));
                myset.insert(make_pair(w->first, w->second));
                addcost[w->first] = w->second;
                parent[w->first] = v;
            }
        }
    }


    // cost initialized as one because forward edges require one round to update
    int cost = 1;
    int atleastoneoldedge = 0;
    uset notUpdated;
    // updated corresponds to set S from the Main Lemma
    uset updated;

    if(print) {
        printf("Chosen path: ");
    }

    stack<int> path;
    for(int node = N; node != 1; node = parent[node]) {
        path.push(node);

        int par = parent[node];
        int next = sigma[inv[par] + 1];

        // path goes from a node i to a node (i + 1)
        if(node == par + 1) {
            // there is a new edge from node i to node (i + 1), so we do not add it to cost
            // required because we might have parallel old and new edges
            if(next == par + 1) {
                continue;
            }
            // else, the used edge is old
            if(!atleastoneoldedge) {
                atleastoneoldedge = 1;
            }
            notUpdated.insert(par);
        }
        else if(!isForward(computeTypeOfVertex(sigma, inv, N, par))) {
            cost++;
        }
    }
    path.push(1);
    if(print) {
        while(!path.empty()) {
            printf("%d ", path.top());
            path.pop();
        }
    }

    for(int i = 1; i < N; i++) {
        if(!isInside(i, notUpdated)) {
            updated.insert(i);
        }
    }
    // need to update some other nodes too if the path is not entirely made up of new edges
    cost = cost + atleastoneoldedge;
    if(print) {
        printf("used to reduce instance with cost %d\n", cost);
    }

    vector<int> newsigma_v = getUpdatedInstance(sigma, inv, N, updated);

    if(print) {
    printf("Obtained reduced instance: ");
        for(int i = 0; i < newsigma_v.size(); i++) {
            printf("%d ", newsigma_v[i]);
        }
        printf("\n");
    }

    int newN = newsigma_v.size();
    if(newN == 1) {
        if(print) {
            printf("Done\n");
        }
        return 1;
    }
    int newsigma[N + 1], newinv[N + 1];
    for(int i = 1; i <= newN; i++) {
        newsigma[i] = newsigma_v[i - 1];
    }
    for(int i = 1; i <= newN; i++) {        
        newinv[newsigma[i]] = i;
    }

    return cost + shortest_path_heuristic(newsigma, newinv, newN, print);
}

// compares the result of the corresponding heuristic with the integral optimum for RLF Problem
// used for the experiments in the Thesis
void compute_heuristic_statistics(int sigma[], int inv[], int N, int (*heuristic) (int[], int[], int, int), int numtries, int randomseed) {
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

    vector<int> heuristic_res;
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

        int T = heuristic(sigma, inv, N, 0);
        heuristic_res.push_back(T);
        fileoutput << "\n" << T << "\n\n";
    }

    // aggregating output
    fileoutput << "Heuristic RLF: ";
    for(auto j = heuristic_res.begin(); j != heuristic_res.end(); j++) {
        fileoutput << *j << ",";
    }

    fileoutput.close();
}

// allows running the experiments leading to Figure 4.7
void compute_peacock_statistics(int (*heuristic) (int[], int[], int, int)) {
    ofstream fileoutput;
    fileoutput.open("results.out");

    srand(100);
    // we store for each minibatch the mean number of rounds obtained over the instances from that minibatch
    vector<double> mean_results;
    for(int N = 300; N <= 400; N = N + 10) {
        fileoutput << "N = " << N << "\n";
        int sigma[N + 1];
        int inv[N + 1];

        sigma[1] = 1;
        sigma[N] = N;
        vector<int> myvector;

        for(int i = 2; i <= N - 1; i++) {
            myvector.push_back(i);
        }

        double somemax = 1.0;
        double mean = 0.0;
        for(int ppp = 1; ppp <= 50; ppp++) {
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

            mean = mean + heuristic(sigma, inv, N, 0);
        }
        mean_results.push_back(mean / 50);
    }


    // aggregating output
    fileoutput << "Mean number of rounds: ";
    for(auto j = mean_results.begin(); j != mean_results.end(); j++) {
        fileoutput << *j << ",";
    }

    fileoutput.close();
}