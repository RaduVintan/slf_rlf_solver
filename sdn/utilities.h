#include <unordered_set>
#include <vector>
#include <stack>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <fstream>
using namespace std;

typedef unordered_set<int> uset;
typedef vector<uset> schedule;

void readInput(int sigma[], int inv[], int N);

int isInside(int k, uset &my_set);

int isForward(int type);

/* 
    i = index of vertex whose type we want to compute
    outputs: 0 if FF, 1 if FB, 2 if BF, 3 if BB
*/
int computeTypeOfVertex(int sigma[], int inv[], int N, int i);

// basically toposort
int checkValidityOfUpdate(int sigma[], int inv[], int N, uset &alreadyUpdated, uset &toUpdate);

int checkValidityOfSchedule(int sigma[], int inv[], int N, schedule &schedule);

int checkPropertyForRandomPermutations(int N, int numtries,
    int (*property)(int[], int[], int));