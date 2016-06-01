#include "evaluate.h"
#include "message.h"
#include "checker.h"
#include "routing.h"
#include "routingalgo.h"
#include <iostream>
#include <fstream>

int main(){
	RoutingSatAlgoFlow algo1;
	RoutingSatAlgoFlowPrune algo2;
	RoutingSatAlgoPointPrune algo3;
	RoutingSatAlgoPointPrunePlus algo4;
	RandomDataGenerator gen;
	
	int tot = 0;
	std::ofstream out("report.out");

	// efficiency evaluation point_p
	/*
	for (int n = 6; n <= 12; ++n)
		for (int m = 1; m <= 9; ++m){
			Evaluation e1(&gen, &algo3, &algo3, n, n, m, 0, (n <= 10) ? 100 : 30);
			e1.Execute(++tot);
			out << "(" << n << "," << m << ")  " << 
				e1.rec_a.average() << " " <<  e1.rec_a.max_time() << 
				" " << e1.rec_a.min_time() << " " << e1.rec_a.standard_deviation() <<std::endl;
		}*/

	// efficiency evaluation point_p^d
	/*
	for (int n = 8; n <= 12; ++n)
		for (int m = 1; m <= 9; ++m){
			Evaluation e1(&gen, &algo4, &algo4, n, n, m, 0, n <= 10 ? 100 : 30);
			e1.Execute(++tot);
			out << "(" << n << "," << m << ")  " <<
			e1.rec_a.average() << " " <<  e1.rec_a.max_time() <<
			" " << e1.rec_a.min_time() << " " << e1.rec_a.standard_deviation() <<std::endl;
		}*/

	// efficiency evaluation flow_p
	/*
	for (int n = 8; n <= 12; ++n)
		for (int m = 1; m <= 9; ++m){
			Evaluation e1(&gen, &algo2, &algo2, n, n, m, 0, n <= 10 ? 100 : 5);
			e1.Execute(++tot);
			out << "(" << n << "," << m << ")  " <<
			e1.rec_a.average() << " " <<  e1.rec_a.max_time() <<
			" " << e1.rec_a.min_time() << " " << e1.rec_a.standard_deviation() <<std::endl;
		}*/

	// efficiency evaluation flow
	/*
	for (int n = 6; n <= 8; ++n)
		for (int m = 1; m <= 9; ++m){
			Evaluation e1(&gen, &algo1, &algo1, n, n, m, 0, n <= 7 ? 100 : 30);
			e1.Execute(++tot);
			out << "(" << n << "," << m << ")  " <<
			e1.rec_a.average() << " " <<  e1.rec_a.max_time() <<
			" " << e1.rec_a.min_time() << " " << e1.rec_a.standard_deviation() <<std::endl;
		}*/
	
	RoutingAlgo *algo[4] = {new RoutingSatAlgoFlow(), new RoutingSatAlgoFlowPrune(), 
							new RoutingSatAlgoPointPrune(), new RoutingSatAlgoPointPrunePlus()};
	// correct check [flow] & [flow_p]
	
	for (int a1 = 0; a1 < 4; ++a1)
		for (int a2 = a1 + 1; a2 < 4; ++a2){
			// pair-wise check algo[a1], algo[a2]
			for (int n = 6; n <= 8; ++n)
				for (int m = 1; m <= 9; ++m){
					Evaluation e1(&gen, algo[a1], algo[a2], n, n, m, 0.1, n <= 7 ? 100 : 30);
					e1.Execute(++tot);
					out << "(" << n << "," << m << ")  " <<
					e1.rec_a.average() << " " <<  e1.rec_b.average() << std::endl;
				}
		}

	delete algo[0];
	delete algo[1];
	delete algo[2];
	delete algo[3];
	out.close();
}