#ifndef ROUTINGSOLVER_H_
#define ROUTINGSOLVER_H_

#include "routingalgo.h"
#include "message.h"

class RoutingSolver{
	RoutingAlgo *algo;
public:
	RoutingSolver(RoutingAlgo *t){
		algo = t;
	}
	std::vector<std::string> load(const std::string &file);
    std::vector<std::vector<int> > getBlock(const std::string &file);
	message solver(const std::string &file);
private:
	message solve(int n, int m, int D, std::vector<std::vector<int> > block){
		message res = algo->Solve(n, m, D, block);
		printf("Successfully computed!\n");
		return res;
	}
};

#endif //ROUTINGSOLVER_H_