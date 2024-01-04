#include "utilities.h"
#include <set>

int peacock(int sigma[], int inv[], int N, int print);
int improvedPeacock(int sigma[], int inv[], int N, int print);
int shortest_path_heuristic(int sigma[], int inv[], int N, int print);
void compute_heuristic_statistics(int sigma[], int inv[], int N, int (*heuristic) (int[], int[], int, int), int numtries, int randomseed);
void compute_peacock_statistics(int (*heuristic) (int[], int[], int, int));