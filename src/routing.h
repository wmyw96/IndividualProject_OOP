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
	std::vector<std::string> load(const std::string &file) const;
	void solver(const std::string &file) const;
private:
	void solve(int n, int m, int D, std::vector<std::vector<int> > block) const{
		message res = algo->Solve(n, m, D, block);
		res.display();
	}
};

#endif //ROUTINGSOLVER_H_