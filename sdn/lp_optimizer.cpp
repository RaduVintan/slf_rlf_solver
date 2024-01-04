#include "lp_optimizer.h"
#include "utilities.h"
using namespace std;

// this function prints the edges of the Full Graph G of the instance of the SLF/RLF Problem
// it can be used by the program generate_cycles.py to generate all cycles of the graph (therefore the somewhat weird format)
// it is also used by the LP Optimizers to read the graph
void printEdges(int sigma[], int inv[], int N) {
    ofstream myfile;
    myfile.open ("../optimizer/edges.txt");
    
    myfile << N << "\n";
    for(int i = 1; i < N; i++) {
        myfile << i << " " << (i + 1) << " {}\n";
        myfile << i << " " << sigma[inv[i] + 1] << " {}\n";
    }
    myfile.close();
}

// solves the full LP for the SLF Problem
// is suboptimal but we provide it for completeness
void lp_optimize(int sigma[], int inv[], int N) {
    printEdges(sigma, inv, N);
    system("cd ../optimizer; python3 generate_cycles.py");
    system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" LP_Solver > /dev/null");
}

// solves the Parameterized LPs described in Subsection 3.3.2 for the SLF Problem and in Subsection 4.2.1 for the RLF Problem
// can be used to find either optimal fractional solution or optimal integer solution
void dynamic_cycle_solver(int sigma[], int inv[], int N, int integralsol, int relaxed) {
    printEdges(sigma, inv, N);
    if(!integralsol && !relaxed) {
        system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 0 0 > /dev/null");
    }
    if(!integralsol && relaxed) {
        system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 0 1 > /dev/null");
    }
    if(integralsol && !relaxed) {
        system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 1 0 > /dev/null");
    }
    if(integralsol && relaxed) {
        system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 1 1 > /dev/null");
    }
}

// solves and rounds the Parameterized LPs for the SLF or RLF Problem
// rounding is done by integralizing rows from top to bottom as described in the Thesis
void rounding_cycle_solver(int sigma[], int inv[], int N, int relaxed) {
    printEdges(sigma, inv, N);
    if(!relaxed) {
        system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Rounding_Cycle_Solver 0 0 > /dev/null");
    }
    else {
        system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Rounding_Cycle_Solver 0 1 > /dev/null");
    }
}

// computes statistics using the lp solver
// used for the experiments in the Thesis
void compute_solvelp_statistics(int sigma[], int inv[], int N, int numtries, int integralsol, int relaxed, int randomseed) {
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

    vector<int> lp_res;
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

        printEdges(sigma, inv, N);
        // computing optimum
        if(!integralsol && !relaxed) {
            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 0 0 > /dev/null");
        }
        if(!integralsol && relaxed) {
            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 0 1 > /dev/null");
        }
        if(integralsol && !relaxed) {
            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 1 0 > /dev/null");
        }
        if(integralsol && relaxed) {
            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 1 1 > /dev/null");
        }

        int T;
        ifstream myfile;
        myfile.open("sdn.out");
        myfile >> T;
        myfile.close();
        fileoutput << "\n";
        if(!integralsol && !relaxed) {
            fileoutput << "SLF LP_OPT: " << T << "\n\n";  
        }
        if(!integralsol && relaxed) {
            fileoutput << "RLF LP_OPT: " << T << "\n\n";  
        }
        if(integralsol && !relaxed) {
            fileoutput << "SLF OPT: " << T << "\n\n";  
        }
        if(integralsol && relaxed) {
            fileoutput << "RLF OPT: " << T << "\n\n";  
        }
           
        lp_res.push_back(T);
    }

    // aggregating output
    fileoutput << "Results: ";
    for(auto j = lp_res.begin(); j != lp_res.end(); j++) {
        fileoutput << *j << ",";
    }

    fileoutput.close();
}

// computes statistics using the rounding lp solver
// used for the experiments in the Thesis
void compute_roundlp_statistics(int sigma[], int inv[], int N, int numtries, int relaxed, int randomseed) {
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

    vector<int> rounding_res;
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
        printEdges(sigma, inv, N);

        // computing rounding
        if(!relaxed) {
            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Rounding_Cycle_Solver 1 > /dev/null");
        }
        else {
            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Rounding_Cycle_Solver 1 1 > /dev/null");
        }
        int T, aux;
        ifstream myfile;
        myfile.open("sdn.out");
        myfile >> aux >> T;
        myfile.close();
        fileoutput << "\nLP_Opt: " << aux << "\n";
        if(!relaxed) {
            fileoutput << "SLF Rounding: " << T << "\n\n";
        }
        else {
            fileoutput << "RLF Rounding: " << T << "\n\n";
        }
        rounding_res.push_back(T);
    }

    // aggregating output
    fileoutput << "\nRounding result: ";
    for(auto j = rounding_res.begin(); j != rounding_res.end(); j++) {
        fileoutput << *j << ",";
    }
    fileoutput.close();
}

// allows running the experiments leading to Figure 3.6
void compare_rounding_to_opt() {
    ofstream fileoutput;
    fileoutput.open("results.out");

    srand(100);

    // we store for each minibatch the maximum and the mean approximation factor obtained over the instances from that minibatch
    vector<double> approx_factor;
    vector<double> mean_factor;
    for(int N = 60; N <= 110; N = N + 5) {
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

            printEdges(sigma, inv, N);

            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Rounding_Cycle_Solver 1 > /dev/null");
            int aux, T1, T2;
            ifstream myfile;
            myfile.open("sdn.out");
            myfile >> aux >> T1;
            myfile.close();

            system("cd ../optimizer ; java -cp \"./src:./lib/gurobi.jar\" Dynamic_Cycle_Solver 1 0 > /dev/null");
            myfile.open("sdn.out");
            myfile >> T2;
            myfile.close();
            if((1.0 * T1) / T2 > somemax) {
                somemax = (1.0 * T1) / T2;
            }
            mean = mean + (1.0 * T1) / T2;
        }
        approx_factor.push_back(somemax);
        mean_factor.push_back(mean / 50);
    }


    // aggregating output
    fileoutput << "Maximal Approximation Factors: ";
    for(auto j = approx_factor.begin(); j != approx_factor.end(); j++) {
        fileoutput << *j << ",";
    }
    fileoutput << "\nMean Approximation Factors: ";
    for(auto j = mean_factor.begin(); j != mean_factor.end(); j++) {
        fileoutput << *j << ",";
    }

    fileoutput.close();
}