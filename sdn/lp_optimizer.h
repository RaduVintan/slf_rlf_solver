#include <iostream>
#include <fstream>
#include <vector>

void printEdges(int sigma[], int inv[], int N);

void lp_optimize(int sigma[], int inv[], int N);

void dynamic_cycle_solver(int sigma[], int inv[], int N, int integralsol, int relaxed);

void rounding_cycle_solver(int sigma[], int inv[], int N, int relaxed);

void compute_solvelp_statistics(int sigma[], int inv[], int N, int numtries, int integralsol, int relaxed, int randomseed);

void compute_roundlp_statistics(int sigma[], int inv[], int N, int numtries, int relaxed, int randomseed);

void compare_rounding_to_opt();