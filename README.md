# IndividualProject_OOP

## Problem

The ’Two-Terminal Path Finding Problem’ has the following description: given N × N routing grids, and M pairs of terminals, you should use program to find routing paths connecting each pair of terminals. The contraints are that different paths cannot cross each other, and no path can cross obstacles. The objective is to maxmize the number of connected pairs, and when all pairs of terminals can be connected, you should minimize the total length of all the paths.

In order to make the objective more clear and not change the original idea of the problem, we simply make the following stipulates that if one grid is defined to be one terminal of pair d, then other pairs d′(d ̸= d′) can not route across the grid.

## Method

We use SAT-Based model to solve the problem with Z3 solver.

## Architecture

The whole system architecture can be divide into four main parts

- the core part, which consists of the whole algorithm(routingalgo.h, routingalgo.cpp, routing.h, routing.cpp, message.h, message.cpp, routing_test.cpp).
- the checker part, which is implemented to check if the answer calculated by the algorithm is legal and correct(checker.h, checker.cpp).
- the evaluation part, which is implemented to evaluate the effciency and correctness of the algorithm(evaluate.h, evaluate.cpp, report.cpp).
- the demo part, we develop a simple demo, you can define your own maps in GUI and view the general results and detailed results to better understanding the problem from a dropout perspective (demo.cpp, demo.h, editor.cpp).

The makefile file can generate three different executable files

- make test: generate a executable file that can choose one/several algorithm to get a answer for a given testcase.
- make repo: generate a executable file that evaluates di erent algorithms.
- make demo: generate a executable file that gives a GUI, which you can de ne the map and run it using a concrete algorithm.

## Details

More details can be seen in the whitepaper in [doc/whitepaper.pdf](https://github.com/wmyw96/IndividualProject_OOP/blob/master/doc/whitepaper.pdf)
