#include "utilities.h"
#include "lp_optimizer.h"
#include <iostream>

void computeLb(int sigma[], int inv[], int lb[], int updated[], int N, int i);

int computeHighestLowerBound(int sigma[], int inv[], int updated[], int N);

void hlbAlgorithm_optOut(int sigma[], int inv[], int N);

void hlbAlgorithm_optIn(int sigma[], int inv[], int N);

void compute_hlb_statistics(int sigma[], int inv[], int N, int numtries, int randomseed);