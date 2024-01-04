import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Scanner;

import gurobi.GRB;
import gurobi.GRBEnv;
import gurobi.GRBLinExpr;
import gurobi.GRBModel;
import gurobi.GRBVar;


// Solving the Linear Program from Lemma 4 of the Thesis
// solves the SLF Problem

public class LP_Solver {

    public static void main(String[] args) {
	try  {
		ArrayList<ArrayList<Integer>> cycles = new ArrayList<>();

		// cycles of the Full Graph G of the instance are stored in this file
		Scanner sc = new Scanner(new BufferedReader(new FileReader("output_cycles.txt")));
		BufferedWriter writer = new BufferedWriter(new FileWriter("../sdn/sdn.out"));
		int n = sc.nextInt();
		
		// reading cycles
		while(sc.hasNext()) {
			ArrayList<Integer> cycle = new ArrayList<>();
			cycles.add(cycle);
			int len = sc.nextInt();

			for(int i = 0; i < len; i++) {
				cycle.add(sc.nextInt());
			}
		}

		// Create environment and model
    		GRBEnv env = new GRBEnv();
    		GRBModel model = new GRBModel(env);

		// same notations as in Thesis
		GRBVar[][] y = new GRBVar[n + 1][n + 1];
		GRBVar[] z = new GRBVar[n + 1];
		
		// creating variables
		for(int t = 0; t < n; t++) {
			for(int i = 1; i < n; i++) {
				y[t][i] = model.addVar(0, GRB.INFINITY, 0, GRB.CONTINUOUS, "y_{" + t + "," + i + "}");
			}
		}

		for(int t = 1; t < n; t++) {
			z[t] = model.addVar(0, GRB.INFINITY, 0, GRB.CONTINUOUS, "z_" + t);
		}
			
            	// Add constraints
		for(int i = 1; i < n; i++) {
			GRBLinExpr expr = new GRBLinExpr();
			expr.addTerm(1.0, y[0][i]);
			model.addConstr(expr, GRB.EQUAL, 1.0, "Start condition");
		}

		for(int t = 1; t < n; t++) {
			for(int i = 1; i < n; i++) {
				GRBLinExpr expr = new GRBLinExpr();
				expr.addTerm(1.0,  y[t - 1][i]);
				expr.addTerm(-1.0, y[t][i]);
				model.addConstr(expr, GRB.GREATER_EQUAL, 0.0, "Monotonous");
			}
		}

		for(int t = 1; t < n; t++) {
			for(int i = 1; i < n; i++) {
				GRBLinExpr expr = new GRBLinExpr();
				expr.addTerm(1.0,   z[t]);
				expr.addTerm(-1.0,  y[t - 1][i]);
				model.addConstr(expr, GRB.GREATER_EQUAL, 0.0, "z is max of ys");
			}
		}

		// adding cycle constraints for each time t
		for(int t = 1; t < n; t++) {
			for(ArrayList<Integer> cycle : cycles) {
				int cold = 0;
				GRBLinExpr expr = new GRBLinExpr();

				for(int index = 0; index < cycle.size() - 1; index++) {
					int i = cycle.get(index);
					int next = cycle.get(index + 1);
					// old edge
					if(next == i + 1) {
						expr.addTerm(1.0,  y[t - 1][i]);
						cold++;
					}
					// new edge
					else {
						expr.addTerm(-1.0, y[t][i]);
					}
				}

				model.addConstr(expr, GRB.LESS_EQUAL, cold - 1, "Cycle constraint");
			}
		}


		// Add objective: sum of zs
	    	GRBLinExpr objective = new GRBLinExpr();
	    	for(int t = 1; t < n; t++) {
			objective.addTerm(1.0, z[t]);
		}
	    	model.setObjective(objective, GRB.MINIMIZE);
	    	model.write("./lp_solver.lp");

	    	// Optimize model
	    	model.optimize();


	    	// Print out fractional solution
        	writer.write("Fractional Optimum: " + model.get(GRB.DoubleAttr.ObjVal) + "\n");
		for(int t = 0; t < n; t++) {
			for(int i = 1; i < n; i++) {
				writer.write(String.format("%.4f", y[t][i].get(GRB.DoubleAttr.X)) + "   ");
			}
			writer.write("\n");
		}
		
		// Compute integral solution
		for(int t = 0; t < n; t++) {
			for(int i = 1; i < n; i++) {
				y[t][i].set(GRB.CharAttr.VType, GRB.INTEGER);
			}
		}

		model.optimize();

		// Print out integral solution
   	 	writer.write("\n\nIntegral Optimum: " + model.get(GRB.DoubleAttr.ObjVal) + "\n");
		for(int t = 0; t < n; t++) {
			for(int i = 1; i < n; i++) {
				writer.write(String.format("%.1f", y[t][i].get(GRB.DoubleAttr.X)).replaceAll( "^-(?=0(\\.0*)?$)", "") + "   ");
			}
			writer.write("\n");
		}

		writer.close();
            	// Dispose model and environment
            	model.dispose();
            	env.dispose();
            	
	} catch (Exception e) {
		e.printStackTrace();
	}
    }
}
