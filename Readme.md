# SLF/RLF Solver

This is a solver implemented as part of the "Fast Algorithms for Loop-Free Network Updates using Linear Programming and Local Search" paper. It solves the Strong Loop-Freedom (SLF) and Relaxed Loop-Freedom (RLF) Problems as introduced in [1]. The software allows the user to solve any of the two problems either optimally, or using one of the provided heuristics which are faster and provide decent approximations. Details regarding the used techniques and algorithms are available in the paper.

&nbsp;

### Structure
- the **sdn** folder contains the C++ implementation of the non-LP based heuristics and the main logic of the solver. For the LP-based algorithms it uses the optimizer.
- the **optimizer** folder contains the Java programs that construct and solve the LPs which our LP-based algorithms use
- the **Results** folder contains the data sampled for the paper. (Some of the plots mention a further heuristic "ShortPath" which was not presented in the paper.)

&nbsp;

### Dependencies and Requirements

This software has been only tested on Linux. (MacOS should work too in principle.)

For running the LP-based algorithms **Java** needs to be installed and the *java* command must be added onto the PATH system variable. Moreover, the **Gurobi** library [2] must be present (see section on installation).

The **make** command is required, as well as a also a local installation of the **g++** compiler. (These are provided by default on most Linux distributions).

&nbsp;

### Installation

This first part of the installation is technically required only if you plan to run any of the LP-based algorithms. Note that these algorithms are the only ones which can compute the optimal solutions for the SLF and RLF Problems:

In the folder **optimizer** create a new folder **lib**, in which the file **gurobi.jar** from your local licensed installation of the **Gurobi** library should be copied. Note that Gurobi offers a free Academic License.

After copying the library, execute inside the **optimizer/src** folder:
```sh
javac -cp "../lib/gurobi.jar" *.java
```
to  compile all Java files. 

The second part of the installation is to compile the C++ files from the **sdn** folder. This step is mandatory. Go into the folder and simply run:
```sh
make
```
The provided Makefile should do everything for you.

&nbsp;

### Usage single instance

We begin with commands that run on one single instance. All these commands read the instance from the file **sdn.in**. An instance is to be stored like in the following example:
```sh
7
1 5 2 6 4 3 7 
```
where the permutation on the second row describes the new path (the old path is assumed to be the identity permutation, e.g. 1 2 3 4 5 6 7). It is not allowed to input permutations where *k* is followed by *k+1* for some k. (Note that in this case the node *k+1* is redundant.)


To apply the LP Algorithms, use:
```sh
./sdn solveLP fractional/integral slf/rlf
```
where the options *fractional* and *integral* allow you to specify whether the computed optimum should be a fractional or an integral optimum, and the *slf* and *rlf* options specify whether you want to solve the SLF or RLF variant.

If only an approximation of the optimum is needed, you can use the rounding procedure:
```sh
./sdn roundLP slf/rlf
```

To apply the Peacock algorithm introduced in [3] for the RLF Problem (but discussed also in the paper), use:
```sh
./sdn peacock
```

To apply the Local Search algorithm introduced in our paper for the RLF Problem, use:
```sh
./sdn localsearch rlf
```

### Usage statistics

The following commands can be used to gather statistics for randomly generated permutations using any of our algorithms for the SLF and RLF Problems. An example:
```sh
./sdn statistics solveLP fractional/integral slf/rlf 60 200 (randomseed)
```
calculates either the fractional or integral OPT for either the SLF or RLF problems for *200* randomly generated permutations/instances with *60* nodes. The last parameter *randomseed* is optional and should be included only if a random seed is desired. If this argument is missing then our software will use a fixed seed. This allows to reproduce the results used for the paper.

The following commands are used to gather statistics with the other algorithms. They should now be self-explanatory:
```sh
./sdn statistics roundLP slf/rlf size tries (randomseed)
./sdn statistics peacock size tries (randomseed)
./sdn statistics localsearch rlf size tries (randomseed)
```
There are two special experiments in the paper which cannot be simulated using the above instructions. To run the Experiment used to generate the data shown in Figure 5 (comparison of approximation factors) for the SLF Problem, use:
```sh
./sdn statistics SLF_experiment
```
To run the Experiment used to generate the data shown in Figure 6 (comparison of heuristics for large number of nodes) for the RLF Problem, use:
```sh
./sdn statistics RLF_experiment
```

### Usage generating graphs
Some interesting types of graphs/instances can be generated in the **sdn.in** file automatically by using certain commands. 

In Figure 8 in [3] a family of graphs is shown for which the greedy strategy of updating all forward nodes in the first round turns out to be a very bad strategy for the SLF Problem. To generate such a graph with k blocks (5k+4 nodes), use:
```sh
./sdn generate greedyGraph k
```
In Figure 2 in [3] a family of graphs is shown for which any SLF solution requires \Omega(n) rounds. To generate the corresponding graph with *k* nodes, use:
```sh
./sdn generate hardGraph k
```
In [3] a family of graphs G(k) is given with 2^(k+3) nodes each and the claim that the Peacock algorithm requires \Omega(k) rounds for solving these instances. As we show in the paper, this construction is not entirely correct. To generate the graph G(k) use:
```sh
./sdn generate wrongPeacockGraph k
```
In the paper we present a corrected construction, where G(k) has 2^k nodes and indeed forces Peacock to use \Omega(k) rounds. To generate the corrected G(k) use:
```sh
./sdn generate correctPeacockGraph k
```

&nbsp;
&nbsp;
&nbsp;

#### References
**[1]** S. Amiri, A. Ludwig, J. Marcinkowski, and S. Schmid. **Transiently Consistent SDN Updates: Being Greedy is Hard.** In: International Colloquium on Structural Information and Communication Complexity. July 2016, pp. 391–406

**[2]** Gurobi Optimization, LLC. Gurobi Optimizer Reference Manual. 2022. URL: https://www.gurobi.com.

**[3]** K.- T. Foerster, A. Ludwig, J. Marcinkowski, and S. Schmid. **Loop-Free Route Updates for Software-Defined Networks.** In: IEEE/ACM Transactions on Networking 26.1 (2018), pp. 328–341.

