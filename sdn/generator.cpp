#include "generator.h"
#include <iostream>

void writeGeneratedGraph(int sigma[], int N) {
    ofstream outputfile;
    outputfile.open("sdn.in");

    outputfile << N << "\n";
    for(int i = 1; i <= N; i++) {
        outputfile << sigma[i] << " ";
    }
    printf("Wrote generated graph to sdn.in\n");
} 

/* 
 * generates sigma for the graph family from Figure 8 of the paper "Loop-Free Route Updates for Software-Defined Networks"
 * size of sigma should be 5k + 4
*/
void generateGreedyGraph(int sigma[], int k) {
    int N = 5 * k + 4;

    int index = 2;
    int node = 1;
    sigma[1] = 1;

    while(true) {
        if(node == N) {
            break;
        }

        if(node == 1) {
            node = 4 * k + 3;
            sigma[index++] = node;
            continue;
        }

        if(node == 2) {
            node = 4 * k + 4;
            sigma[index++] = node;
            continue;
        }

        if(node == 4 * k + 3) {
            node = 4 * k + 2;
            sigma[index++] = node;
            continue;
        }

        if(node >= 4 * k + 4) {
            node = (node - 4 * k - 3) * 4;
            sigma[index++] = node;
            continue;
        }

        // we know node is in some block

        if(node % 4 == 0) {
            node = 4 * k + 4 + node / 4;
            sigma[index++] = node;
            continue;
        }

        if(node % 4 == 1 || node % 4 == 2) {
            node = node - 3;
            sigma[index++] = node;
            continue;
        }

        // we know node % 4 == 3
        node = node + 2;
        sigma[index++] = node;
    }
}

// see Thesis
void generateShortIsBadGraph(int sigma[], int k) {
    int N = 4 * k + 2;

    int index = 2;
    int node = 1;
    sigma[1] = 1;

    while(true) {
        if(node == N) {
            break;
        }

        if(node == 1) {
            node = N - 1;
            sigma[index++] = node;
            continue;
        }

        if(node == 4) {
            node = 3;
            sigma[index++] = node;
            continue;
        }

        if(node == N - 3) {
            node = N;
            sigma[index++] = node;
            continue;
        }

        if(node % 4 == 0 || node % 4 == 1) {
            node = node - 3;
            sigma[index++] = node;
            continue;
        }

        if(node % 4 == 2) {
            node = node + 2;
            sigma[index++] = node;
            continue;
        }

        node = node + 4;
        sigma[index++] = node;
    }
}

// graph from Figure 2 of "Loop-Free Route Updates for Software-Defined Networks"
void generateHardGraph(int sigma[], int N) {
    sigma[1] = 1;

    int index = 2;
    int node = 1;
    while(true) {
        if(node == N) {
            break;
        }

        if(node == 1) {
            node = N - 1;
            sigma[index++] = node;
            continue;
        }

        if(node == 2) {
            node = N;
            sigma[index++] = node;
            continue;
        }

        node = node - 1;
        sigma[index++] = node;
    }
}

/*
 * wrong lower bound graphs G_j from paper "Loop-Free Route Updates for Software-Defined Networks"
*/
void generateWrongPeacockLowerBoundGraphs(int sigma[], int j) {
    int N = 1 << (j + 3);
    
    int index[(1 << j) + 10][8];
    int edges[N];

    for(int i = 1; i <= N / 2; i++) {
        edges[i] = N / 2 + i;
    }

    int ct = 1;
    for(int k = 7; k >= 0; k--) {
        for(int i = 3; i <= (1 << j) + 2; i++) {
            index[i][k] = ct++;
        }
    }
    
    for(int i = 3; i <= (1 << j) + 2; i++) {
        edges[index[i][3]] = index[i][5];
        edges[index[i][1]] = index[i][6];
        edges[index[i][2]] = index[i][4];
        if(index[i][0] != N) {
            edges[index[i][0]] = index[i + 1][7];
        }
    }

    for(int i = 1, node = 1; i <= N; i++, node = edges[node]) {
        sigma[i] = node;
    }
}

// returns backward edges for nodes from second half
vector<int> helperForCorrectedPeacockLowerBoundGraphs(int j) {
    int N = (1 << j);
    vector<int> res;

    if(N == 2) {
        return res;
    }

    for(int i = N / 2 + 1; i <= 3 * N / 4; i++) {
        res.push_back(i - N / 4);
    }
    vector<int> smaller = helperForCorrectedPeacockLowerBoundGraphs(j - 1);
    for(auto i = smaller.begin(); i != smaller.end(); i++) {
        res.push_back(*i);
    }

    return res;
}

// as described in Thesis
void generatePeacockLowerBoundGraphsCorrected(int sigma[], int j) {
    int N = (1 << j);
    
    if(N == 2) {
        sigma[1] = 1;
        sigma[2] = 2;
        return;
    }

    vector<int> edgesSecondHalf = helperForCorrectedPeacockLowerBoundGraphs(j);

    int edges[N];
    for(int i = 1; i <= N / 2; i++) {
        edges[i] = N / 2 + i;
    }

    int index = N / 2 + 1;
    for(auto i = edgesSecondHalf.begin(); i != edgesSecondHalf.end(); i++) {
        edges[index++] = *i;
    }

    for(int i = 1, node = 1; i <= N; i++, node = edges[node]) {
        sigma[i] = node;
    }
}