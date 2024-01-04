#include "local_search.h"
using namespace std;

schedule computeInitialSchedule(int sigma[], int inv[], int N) {
    schedule res;
    
    uset forwardEdges;
    for(int i = 1; i <= N - 1; i++) {
        if(isForward(computeTypeOfVertex(sigma, inv, N, i))) {
            forwardEdges.insert(i);
        }
        else {
            uset aux;
            aux.insert(i);
            res.push_back(aux);
        }
    }

    res.insert(res.begin(), forwardEdges);
    return res;
}

void printSchedule(schedule schedule) {
    int k = schedule.size();

    int ct = 1;
    for(auto i = schedule.begin(); i != schedule.end(); i++, ct++) {
        printf("Round %d: ", ct);
        for(auto j = schedule.at(ct - 1).begin(); j != schedule.at(ct - 1).end(); j++) {
            printf("%d ", *j);
        }
        printf("\n");
    }
}

void computeTimestamps(schedule &schedule, int N, int timestamp[]) {
    int ct = 1;
    for(auto i = schedule.begin(); i != schedule.end(); i++, ct++) {
        for(auto j = schedule.at(ct - 1).begin(); j != schedule.at(ct - 1).end(); j++) {
            timestamp[*j] = ct;
        }
    }
}

int roundCriterion(schedule &sched, uset &i_old, uset &i_new, int i) {
    return i_old.empty();
}

int roundCriterion(schedule &sched, uset &i_old, uset &i_new, uset &j_old, uset &j_new, int i, int j) {
    return i_old.empty() || j_old.empty();
}

int minBucketCriterion(schedule &sched, uset &i_old, uset &i_new, uset &j_old, uset &j_new, int i, int j) {
    int newmin = sched.begin()->size();
    
    for(auto i = sched.begin(); i != sched.end(); i++) {
        uset aux = *i;
        if(aux.size() < newmin) {
            newmin = aux.size();
        }
    }

    // revert old state
    i_new.erase(i);
    i_old.insert(i);
    j_new.erase(j);
    j_old.insert(j);

    int min = sched.begin()->size();
    for(auto i = sched.begin(); i != sched.end(); i++) {
        uset aux = *i;
        if(aux.size() < min) {
            min = aux.size();
        }
    }

    // revert new state
    i_old.erase(i);
    i_new.insert(i);
    j_old.erase(j);
    j_new.insert(j);

    return newmin < min;
}

schedule improveSchedule_1Neighborhood(int sigma[], int inv[], schedule &sched, int timestamp[], int N,
                                       int (*criterion) (schedule&, uset&, uset&, int), int print) {
    int k = sched.size();
    
    for(int i = 1; i <= N - 1; i++) {
        int bi = timestamp[i];

        for(int mi = 1; mi <= k; mi++) {
            if(mi == i) {
                continue;
            }

            // trying to move i to mi
            uset &i_old = sched.at(bi - 1);
            uset &i_new = sched.at(mi - 1);

            i_old.erase(i);
            i_new.insert(i);

            if(criterion(sched, i_old, i_new, i)) {
                if(checkValidityOfSchedule(sigma, inv, N, sched)) {
                    schedule newsched;
                    for(auto i = sched.begin(); i != sched.end(); i++) {
                        uset aux = *i;
                        if(!aux.empty()) {
                            newsched.push_back(aux);
                        }
                    }

                    if(print) {
                        printf("Move %d from bucket %d to bucket %d\n\n",   i, bi, mi);
                        printSchedule(newsched);
                    }

                    computeTimestamps(newsched, N, timestamp);
                    return newsched;
                }
            }

            i_new.erase(i);
            i_old.insert(i);
        }
    }
        
    schedule s;
    return s;
}

schedule improveSchedule_2Neighborhood(int sigma[], int inv[], schedule &sched, int timestamp[], int N,
                                       int (*criterion) (schedule&, uset&, uset&, uset&, uset&, int, int), int print) {
    int k = sched.size();
    
    for(int i = 1; i <= N - 1; i++) {
        for(int j = 1; j <= N - 1; j++) {
            if(i == j) {
                continue;
            }

            int bi = timestamp[i];
            int bj = timestamp[j];

            for(int mi = 1; mi <= k; mi++) {
                for(int mj = 1; mj <= k; mj++) {
                    if(mi == bi && mj == bj) {
                        continue;
                    }

                    // trying to move i to mi and j to mj

                    uset &i_old = sched.at(bi - 1);
                    uset &j_old = sched.at(bj - 1);
                    uset &i_new = sched.at(mi - 1);
                    uset &j_new = sched.at(mj - 1);


                    i_old.erase(i);
                    i_new.insert(i);
                    j_old.erase(j);
                    j_new.insert(j);


                    if(criterion(sched, i_old, i_new, j_old, j_new, i, j)) {
                        if(checkValidityOfSchedule(sigma, inv, N, sched)) {
                            schedule newsched;
                            for(auto i = sched.begin(); i != sched.end(); i++) {
                                uset aux = *i;
                                if(!aux.empty()) {
                                    newsched.push_back(aux);
                                }
                            }

                            if(print) {
                                printf("Move %d from bucket %d to bucket %d\n",   i, bi, mi);
                                printf("Move %d from bucket %d to bucket %d\n\n", j, bj, mj);
                                printSchedule(newsched);
                            }

                            computeTimestamps(newsched, N, timestamp);
                            return newsched;
                        }
                    }

                    i_new.erase(i);
                    i_old.insert(i);
                    j_new.erase(j);
                    j_old.insert(j);
                }
            }
        }
    }

    schedule s;
    return s;
}

int local_search(int sigma[], int inv[], int N, int print) {
    schedule sched = computeInitialSchedule(sigma, inv, N);
    
    if(print) {
        printSchedule(sched);
    }
    int timestamp[N + 1];
    computeTimestamps(sched, N, timestamp);

    while(true) {
        schedule newsched = improveSchedule_1Neighborhood(sigma, inv, sched, timestamp, N, &roundCriterion, print);
        if(newsched.empty()) {
            newsched = improveSchedule_2Neighborhood(sigma, inv, sched, timestamp, N, &roundCriterion, print);
            if(newsched.empty()) {
                newsched = improveSchedule_2Neighborhood(sigma, inv, sched, timestamp, N, &minBucketCriterion, print);
                if(newsched.empty()) {
                    return sched.size();
                }
                sched = newsched;
            }
            else {
                sched = newsched;
            }
        }
        sched = newsched;
    }
}

// used to test local search in Thesis
void compute_localsearch_statistics(int sigma[], int inv[], int N, int numtries, int randomseed) {
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

    vector<int> local_search_res;
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

        int ls_res = local_search(sigma, inv, N, 0);
        fileoutput << "\nLocalSearch: " << ls_res << "\n\n";
        local_search_res.push_back(ls_res);
    }

    // aggregating output
    fileoutput << "Local Search SLF: ";
    for(auto j = local_search_res.begin(); j != local_search_res.end(); j++) {
        fileoutput << *j << ",";
    }
    fileoutput.close();
}