#include "utilities.h"

/* generates sigma for the gredy family from the paper
   size of sigma should be 5k + 4
*/
void writeGeneratedGraph(int sigma[], int N);
void generateGreedyGraph(int sigma[], int k);
void generateShortIsBadGraph(int sigma[], int k);
void generateHardGraph(int sigma[], int N);
void generateWrongPeacockLowerBoundGraphs(int sigma[], int j);
void generatePeacockLowerBoundGraphsCorrected(int sigma[], int j);
