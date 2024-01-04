import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Scanner;

import gurobi.GRB;
import gurobi.GRBConstr;
import gurobi.GRBEnv;
import gurobi.GRBException;
import gurobi.GRBLinExpr;
import gurobi.GRBModel;
import gurobi.GRBVar;

// Solving the Parameterized Linear Program from Lemmas 5 and 10 of the Thesis
// solves either the SLF or the RLF Problem

public class Dynamic_Cycle_Solver {

    public static void main(String[] args) throws GRBException, IOException {
        boolean computeIntegralSolution = false;
        boolean solveRelaxedProblem = false;

        // first argument determines if we have to compute integral or fractional solution
        if(args.length == 1) {
            computeIntegralSolution = (Integer.parseInt(args[0]) == 1);
            solveRelaxedProblem = false;
        }
        
        // second argument determines if we have to solve the SLF or the RLF Problem
        if(args.length == 2) {
            computeIntegralSolution = (Integer.parseInt(args[0]) == 1);
            solveRelaxedProblem = (Integer.parseInt(args[1]) == 1);
        }

        // file from which to read graph and file to which we write result
        Scanner sc = new Scanner(new BufferedReader(new FileReader("edges.txt")));
        BufferedWriter writer = new BufferedWriter(new FileWriter("../sdn/sdn.out"));
        int n = sc.nextInt();

        // follower w.r.t. new path in G
        int[] follower = new int[n + 1];

        while(sc.hasNext()) {
            int a = sc.nextInt();
            int b = sc.nextInt();
            sc.next();
            if(b != a + 1) {
                follower[a] = b;
            }
        }

        // Create environment and model
        GRBEnv env = new GRBEnv();
        GRBModel model = new GRBModel(env);
        
        ArrayList<ArrayList<GRBVar>> y = new ArrayList<>();        
        int T = 0;

        // adding variables y_0i, i.e. for t = 0
        ArrayList<GRBVar> aux = new ArrayList<>();
        aux.add(null);
        for(int i = 1; i < n; i++) {
            if(computeIntegralSolution) {
                aux.add(model.addVar(0, GRB.INFINITY, 0, GRB.INTEGER, "y_{" + 0 + "," + i + "}"));
            }
            else {
                aux.add(model.addVar(0, GRB.INFINITY, 0, GRB.CONTINUOUS, "y_{" + 0 + "," + i + "}"));
            }
        }
        y.add(aux);

        // Add start constraints
        for(int i = 1; i < n; i++) {
            GRBLinExpr expr = new GRBLinExpr();
            expr.addTerm(1.0, y.get(0).get(i));
            model.addConstr(expr, GRB.EQUAL, 1.0, "Start condition");
        }

        // storing endconstraints separately because we might need to remove them later in case we increase T
        GRBConstr[] endConstraints = new GRBConstr[n + 1];

        // T : currently required number of rounds; start from 1
        bigloop: for(T = 1; ; T++) {
            // adding variables y_Ti, i.e. for t = T
            aux = new ArrayList<>();
            aux.add(null);
            for(int i = 1; i < n; i++) {
                if(computeIntegralSolution) {
                    aux.add(model.addVar(0, GRB.INFINITY, 0, GRB.INTEGER, "y_{" + T + "," + i + "}"));
                }
                else {
                    aux.add(model.addVar(0, GRB.INFINITY, 0, GRB.CONTINUOUS, "y_{" + T + "," + i + "}"));
                }
            }
            y.add(aux);

            // adding constraints including new variables

            for(int i = 1; i < n; i++) {
                GRBLinExpr expr = new GRBLinExpr();
                expr.addTerm(1.0, y.get(T).get(i));
                endConstraints[i] = model.addConstr(expr, GRB.EQUAL, 0.0, "End condition");
            }

            for(int i = 1; i < n; i++) {
                GRBLinExpr expr = new GRBLinExpr();
                expr.addTerm(1.0,  y.get(T - 1).get(i));
                expr.addTerm(-1.0, y.get(T).get(i));
                model.addConstr(expr, GRB.GREATER_EQUAL, 0.0, "Monotonous");
            }
            
            /* 
             * in this while loop: 
             * solve problem, check if there exists a cycle which is too short
             * if so: add correspodning constraint
            */ 
            while(true) {
                boolean ok = true;
                // Optimize model
                model.optimize();
                
                // if infeasible, we need to increase T and to remove endconstraints
                if(model.get(GRB.IntAttr.Status) != 2) {
                    for(int j = 1; j < n; j++) {
                        model.remove(endConstraints[j]);
                    }
                    endConstraints = new GRBConstr[n + 1];
                    continue bigloop;
                } 

                // Find shortest cycle using Floyd-Warshall
                int INF = 4 * n;
                for(int time = 1; time <= T; time++) {
                    double[][] d = new double[n + 1][n + 1];
                    int[][] next = new int[n + 1][n + 1];

                    for(int i = 1; i <= n - 1; i++) {
                        for(int j = 1; j <= n - 1; j++) {
                            d[i][j] = INF;
                        }
                    }
                    for(int i = 1; i <= n - 1; i++) {
                        d[i][follower[i]] = y.get(time).get(i).get(GRB.DoubleAttr.X);
                        d[i][i + 1] = 1 - y.get(time - 1).get(i).get(GRB.DoubleAttr.X);
                        
                        next[i][i] = 0;
                        next[i][follower[i]] = follower[i];
                        next[i][i + 1] = i + 1;
                    } 
                    
                    for(int k = 1; k <= n - 1; k++) {
                        for(int i = 1; i <= n - 1; i++) {
                            for(int j = 1; j <= n - 1; j++) {
                                if(d[i][j] > d[i][k] + d[k][j] + 1e-10) {
                                    d[i][j] = d[i][k] + d[k][j];
                                    next[i][j] = next[i][k];
                                }
                            }
                        }
                    }
                    
                    // updated[i] = 1 iff we already found cycle containing i
                    int[] updated = new int[n + 1];
                    for(int i = 1; i <= n; i++) {
                        updated[i] = 0;
                    }
                    loop: for(int i = 1; i <= n - 1; i++) {
                        if(updated[i] == 1) {
                            continue;
                        }
                        if((!solveRelaxedProblem && d[i][i] < 1 - 1e-10) 
                        || (solveRelaxedProblem && d[i][i] + d[1][i] < 1 - 1e-10)) {
                            ok = false;
                            GRBLinExpr expr = new GRBLinExpr();
                            int before = i;
                            int cold = 0;

                            HashSet<Integer> tester = new HashSet<>();
                            for(int j = next[i][i];; j = next[j][i]) {
                                // should never happen
                                if(tester.contains(j)) {
                                    writer.write("FATAL ERROR");
                                    continue loop;
                                }
                                tester.add(j);
                                updated[j] = 1;
                                // old edge
                                if(j == before + 1) {
                                    expr.addTerm(1.0, y.get(time - 1).get(before));
                                    cold++;
                                }
                                // new case
                                else {
                                    expr.addTerm(-1.0, y.get(time).get(before));
                                }
                                before = j;
                                if(j == i) {
                                    break;
                                }
                            }
                            
                            // for relaxed problem we need to find path from s connecting to cycle
                            if(solveRelaxedProblem) {
                                before = 1;
                                for(int j = next[1][i];; j = next[j][i]) {
                                    // reached cycle
                                    if(tester.contains(before)) {
                                        break;
                                    }
                                    
                                    // old edge
                                    if(j == before + 1) {
                                        expr.addTerm(1.0, y.get(time - 1).get(before));
                                        cold++;
                                    }
                                    // new edge
                                    else {
                                        expr.addTerm(-1.0, y.get(time).get(before));
                                    }
                                    before = j;
                                }
                            }
                            model.addConstr(expr, GRB.LESS_EQUAL, cold - 1, "Cycle constraint");
                        }
                    }
                }
                // if no violated cycle found, we are done
                if(ok) {
                    writer.write(T + " rounds ");
                    if(solveRelaxedProblem && computeIntegralSolution) {
                        writer.write("(optimal integral solution of RLF). Optimal schedule:\n");
                    }
                    if(solveRelaxedProblem && !computeIntegralSolution) {
                        writer.write("(optimal fractional solution of RLF). Optimal schedule:\n");
                    }
                    if(!solveRelaxedProblem && computeIntegralSolution) {
                        writer.write("(optimal integral solution of SLF). Optimal schedule:\n");
                    }
                    if(!solveRelaxedProblem && !computeIntegralSolution) {
                        writer.write("(optimal fractional solution of SLF). Optimal schedule:\n");
                    }
                    for(int t = 0; t <= T; t++) {
                        for(int i = 1; i < n; i++) {
                            writer.write(String.format("%.1f", y.get(t).get(i).get(GRB.DoubleAttr.X)).replaceAll( "^-(?=0(\\.0*)?$)", "") + " ");
                        }
                        writer.write("\n");
                    }
                    break;
                }
            }
            writer.close();
            model.write("./dynamic_cycle_solver.lp");
            // Dispose model and environment
            model.dispose();
            env.dispose();
            break;
            
        }
    }
}
