#include "generator.h"
#include "hlb.h"
#include "utilities.h"
#include "local_search.h"
#include "lp_optimizer.h"
#include "peacock.h"
using namespace std;

int main(int argc, char *argv[]) 
{
    freopen("sdn.in", "r", stdin);

    int validInput = 0;

    if(argc != 1) {
        string s(argv[1]);

        if(s == "generate") {
            // validating input
            if(argc != 4) {
                goto end;
            }
            try {
                stoi(string(argv[3]));
            }
            catch(invalid_argument &e) {
                goto end;
            }
            // validation over
            validInput = 1;
            string graph(argv[2]);
            if(graph == "greedyGraph") {
                int k = stoi(string(argv[3]));
                int N = 5 * k + 4;
                int sigma[N + 1];
                int inv[N + 1];

                generateGreedyGraph(sigma, k);
                writeGeneratedGraph(sigma, N);
            }
            else if(graph == "shortIsBad") {
                int k = stoi(string(argv[3]));
                int N = 4 * k + 2;
                int sigma[N + 1];
                int inv[N + 1];

                generateShortIsBadGraph(sigma, k);
                writeGeneratedGraph(sigma, N);
            }
            else if(graph == "hardGraph") {
                int N = stoi(string(argv[3]));
                int sigma[N + 1];
                int inv[N + 1];

                generateHardGraph(sigma, N);
                writeGeneratedGraph(sigma, N);
            }
            else if(graph == "wrongPeacockGraph") {
                int j = stoi(string(argv[3]));
                int N = 1 << (j + 3);
                int sigma[N + 1];
                int inv[N + 1];

                generateWrongPeacockLowerBoundGraphs(sigma, j);
                writeGeneratedGraph(sigma, N);
            }
            else if(graph == "correctPeacockGraph") {
                int j = stoi(string(argv[3]));
                int N = 1 << j;
                int sigma[N + 1];
                int inv[N + 1];

                generatePeacockLowerBoundGraphsCorrected(sigma, j);
                writeGeneratedGraph(sigma, N);
            }
            else {
                goto end;
            }
        }
        else if(s == "statistics") {
            // validating input
            if(argc < 3) {
                goto end;
            }
            if(string(argv[2]) == "solveLP") {
                if(argc != 7 && argc != 8) {
                    goto end;
                }
                if(string(argv[3]) != "fractional" && string(argv[3]) != "integral") {
                    goto end;
                }
                if(string(argv[4]) != "rlf" && string(argv[4]) != "slf") {
                    goto end;
                }
                try {
                    stoi(string(argv[5]));
                    stoi(string(argv[6]));
                }
                catch(invalid_argument &e) {
                    goto end;
                }

                if(argc == 8 && string(argv[7]) != "randomseed") {
                    goto end;
                }
            }
            else if(string(argv[2]) == "localsearch" || string(argv[2]) == "roundLP") {
                if(argc != 6 && argc != 7) {
                    goto end;
                }
                if(string(argv[3]) != "rlf" && string(argv[3]) != "slf") {
                    goto end;
                }
                try {
                    stoi(string(argv[4]));
                    stoi(string(argv[5]));
                }
                catch(invalid_argument &e) {
                    goto end;
                }

                if(argc == 7 && string(argv[6]) != "randomseed") {
                    goto end;
                }
            }
            else if(string(argv[2]) == "hlb" || string(argv[2]) == "peacock" || string(argv[2]) == "shortpath") {
                if(argc != 5 && argc != 6) {
                    goto end;
                }
                try {
                    stoi(string(argv[3]));
                    stoi(string(argv[4]));
                }
                catch(invalid_argument &e) {
                    goto end;
                }
                if(argc == 6 && string(argv[5]) != "randomseed") {
                    goto end;
                }
            }
            else if(string(argv[2]) == "RLF_experiment") {
                if(argc != 4) {
                    goto end;
                }
                if(string(argv[3]) != "peacock" && string(argv[3]) != "localsearch" && string(argv[3]) != "shortpath") {
                    goto end;
                }
            }
            else if(string(argv[2]) == "SLF_experiment") {
                if(argc != 3) {
                    goto end;
                }
            }
            else {
                goto end;
            }
            // validation over
            validInput = 1;
            if(string(argv[2]) == "hlb") {
                int N = stoi(string(argv[3]));
                int numtries = stoi(string(argv[4]));
                int randomseed = 0;
                if(argc == 6 && string(argv[5]) == "randomseed") {
                    randomseed = 1;
                }

                int sigma[N + 1];
                int inv[N + 1];
                compute_hlb_statistics(sigma, inv, N, numtries, randomseed);
            }
            if(string(argv[2]) == "localsearch") {
                int relaxed = (string(argv[3]) == "rlf") ? 1 : 0;
                int N = stoi(string(argv[4]));
                int numtries = stoi(string(argv[5]));
                int randomseed = 0;
                if(argc == 7 && string(argv[6]) == "randomseed") {
                    randomseed = 1;
                }

                int sigma[N + 1];
                int inv[N + 1];
                if(!relaxed) {
                    compute_localsearch_statistics(sigma, inv, N, numtries, randomseed);
                }
                else {
                    compute_heuristic_statistics(sigma, inv, N, improvedPeacock, numtries, randomseed);
                }
            }
            if(string(argv[2]) == "solveLP") {
                int integralsol = (string(argv[3]) == "integral") ? 1 : 0;
                int relaxed = (string(argv[4]) == "rlf") ? 1 : 0;
                int N = stoi(string(argv[5]));
                int numtries = stoi(string(argv[6]));
                int randomseed = 0;
                if(argc == 8 && string(argv[7]) == "randomseed") {
                    randomseed = 1;
                }

                int sigma[N + 1];
                int inv[N + 1];
                compute_solvelp_statistics(sigma, inv, N, numtries, integralsol, relaxed, randomseed);
            }
            if(string(argv[2]) == "roundLP") {
                int relaxed = (string(argv[3]) == "rlf") ? 1 : 0;
                int N = stoi(string(argv[4]));
                int numtries = stoi(string(argv[5]));
                int randomseed = 0;
                if(argc == 7 && string(argv[6]) == "randomseed") {
                    randomseed = 1;
                }

                int sigma[N + 1];
                int inv[N + 1];
                compute_roundlp_statistics(sigma, inv, N, numtries, relaxed, randomseed);
            }
            if(string(argv[2]) == "peacock") {
                int N = stoi(string(argv[3]));
                int numtries = stoi(string(argv[4]));
                int randomseed = 0;
                if(argc == 6 && string(argv[5]) == "randomseed") {
                    randomseed = 1;
                }

                int sigma[N + 1];
                int inv[N + 1];
                compute_heuristic_statistics(sigma, inv, N, peacock, numtries, randomseed);
            }
            if(string(argv[2]) == "shortpath") {
                int N = stoi(string(argv[3]));
                int numtries = stoi(string(argv[4]));
                int randomseed = 0;
                if(argc == 6 && string(argv[5]) == "randomseed") {
                    randomseed = 1;
                }

                int sigma[N + 1];
                int inv[N + 1];
                compute_heuristic_statistics(sigma, inv, N, shortest_path_heuristic, numtries, randomseed);
            }
            if(string(argv[2]) == "SLF_experiment") {
                compare_rounding_to_opt();
            }
            if(string(argv[2]) == "RLF_experiment") {
                if(string(argv[3]) == "peacock") {
                    compute_peacock_statistics(peacock);
                }
                if(string(argv[3]) == "localsearch") {
                    compute_peacock_statistics(improvedPeacock);
                }
                if(string(argv[3]) == "shortpath") {
                    compute_peacock_statistics(shortest_path_heuristic);
                }
            }

            printf("Computed statistics. Written results to results.out\n");
        }
        else {
            // validating input
            if(s == "hlb" || s == "hlbHeuristic" || s == "solveLPinefficient" || 
                s == "peacock" || s == "shortpath") {
                    if(argc != 2) {
                        goto end;
                    }
            }
            else if(s == "localsearch" || s == "roundLP") {
                if(argc != 3) {
                    goto end;
                }
                if(string(argv[2]) != "rlf" && string(argv[2]) != "slf") {
                    goto end;
                }
            }
            else if(s == "solveLP") {
                if(argc != 4) {
                    goto end;
                }
                if(string(argv[2]) != "fractional" && string(argv[2]) != "integral") {
                    goto end;
                }
                if(string(argv[3]) != "rlf" && string(argv[3]) != "slf") {
                    goto end;
                }
            }
            else {
                goto end;
            }
            // validation over
            validInput = 1;
            int N;
            scanf("%d", &N);
            int sigma[N + 1];
            int inv[N + 1];
            readInput(sigma, inv, N);

            if(s == "hlb") {
                int updated[N + 1];
                for(int i = 1; i <= N; i++) {
                    updated[i] = 0;
                }
                printf("HLB is: %d\n", computeHighestLowerBound(sigma, inv, updated, N));
            }
            if(s == "hlbHeuristic") {
                hlbAlgorithm_optOut(sigma, inv, N);
            }
            if(s == "localsearch") {
                if((string(argv[2]) == "slf")) {
                    printf("%d rounds\n", local_search(sigma, inv, N, 1));
                }
                else {
                    printf("%d rounds\n", improvedPeacock(sigma, inv, N, 1));
                }
            }
            if(s == "solveLPinefficient") {
                lp_optimize(sigma, inv, N);
                printf("Result written to sdn.out\n");
            }
            if(s == "solveLP")  {
                int integralsol = (string(argv[2]) == "integral") ? 1 : 0;
                int relaxed = (string(argv[3]) == "rlf") ? 1 : 0;
                dynamic_cycle_solver(sigma, inv, N, integralsol, relaxed);
                printf("Solved instance; written output to sdn.out\n");
            }
            if(s == "roundLP")  {
                int relaxed = (string(argv[2]) == "rlf") ? 1 : 0;
                rounding_cycle_solver(sigma, inv, N, relaxed);
                printf("Solved instance; written output to sdn.out\n");
            }
            if(s == "peacock") {
                printf("%d rounds\n", peacock(sigma, inv, N, 1));
            }
            if(s == "shortpath") {
                printf("%d rounds\n", shortest_path_heuristic(sigma, inv, N, 1));
            }
        }

        end:
        if(!validInput) {
            printf("Input is invalid.\n");
        }
    }
    else {
        printf("Input is invalid.\n");
    }
    return 0;
}