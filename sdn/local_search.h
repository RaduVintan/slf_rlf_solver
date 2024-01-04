#include "utilities.h"
#include "lp_optimizer.h"
#include <cstring>
#include <stack>
#include <iostream>
using namespace std;


void computeTimestamps(schedule &schedule, int N, int timestamp[]);

int roundCriterion(schedule &sched, uset &i_old, uset &i_new, uset &j_old, uset &j_new, int i, int j);

int minBucketCriterion(schedule &sched, uset &i_old, uset &i_new, uset &j_old, uset &j_new, int i, int j);

int local_search(int sigma[], int inv[], int N, int print);

void compute_localsearch_statistics(int sigma[], int inv[], int N, int numtries, int randomseed);