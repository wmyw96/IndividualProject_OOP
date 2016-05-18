/** 
  * \file routingalgo.h
  * \brief A header file for the detailed routing algorithm
  * \author YiHong Gu
  * \date 2016-05-14
  * \note 
  * 
  * The algorithm part consists of three major algorithms.
*/

#ifndef ROUTINGALGO_H_
#define ROUTINGALGO_H_

#include <z3++.h>
#include <vector>
#include <iostream>
#include "message.h"

/** 
  * \class <RoutingAlgo> [routingalgo.h]
  * \class RoutingAlgo
  * \brief An abstract class of algorithm implement
  * \author YiHong Gu
  * \date 2016-05-14
  *
  * No details.
*/
class RoutingAlgo{
public:
  z3::context sat_context; // < a component of z3 solver to define expressions
  RoutingAlgo() : sat_context() {}
	virtual message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block) = 0;
};

/** 
  * \class <RoutingSatAlgo> [routingalgo.h]
  * \class RoutingSatAlgo
  * \brief An concrete class of algorithm implement using sat model
  * \author YiHong Gu
  * \date 2016-05-14
  *
  * No details.
*/
class RoutingSatAlgo : public RoutingAlgo{
	z3::expr one, zero;
  int n, m, D;
private:
	// Get the index of No.[d] pair, ([x], [y]) position, block routing type [type].
	int getindx(int d, int x, int y, int type);
	// Get the term
  void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y, int k, int block_type);
  message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug_sel, int aug_limit_t);
  message SolveMaxPairs(int n_, int m_, int D_, std::vector<std::vector<int> > block);
  message SolveMinLength(int n_, int m_, int D_, std::vector<std::vector<int> > block);
public:
  RoutingSatAlgo() : RoutingAlgo(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) {}
	// A concrete implement of the sat model algorithm
	message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

class RoutingSatAlgoAutoOpt : public RoutingAlgo{
    z3::expr one, zero;
    int n, m, D, layer_cnt;
    //int idx[11][11][4];
private:
    int idx(int x, int y, int type);
    // Get the index of No.[d] pair, ([x], [y]) position, block routing type [type].
    int getindx(int d, int x, int y, int type);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y);
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
public:
    RoutingSatAlgoAutoOpt() : RoutingAlgo(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

class RoutingExtendSatAlgoAutoOpt : public RoutingAlgo{
    z3::expr one, zero;
    int n, m, D, layer_cnt;
    //int idx[11][11][4];
private:
    int idx(int x, int y, int type);
    // Get the index of No.[d] pair, ([x], [y]) position, block routing type [type].
    int getindx(int x, int y, int type);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int x, int y, int k);
    message SolveExtendSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
public:
    RoutingExtendSatAlgoAutoOpt() : RoutingAlgo(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

class RoutingSatAlgoAutoOptPrune : public RoutingAlgo{
    z3::expr one, zero;
    int n, m, D, layer_cnt;
    //int idx[11][11][4];
private:
    int idx(int x, int y, int type);
    // Get the index of No.[d] pair, ([x], [y]) position, block routing type [type].
    int getindx(int d, int x, int y, int type);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y);
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
public:
    RoutingSatAlgoAutoOptPrune() : RoutingAlgo(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

#endif //ROUTINGALGO_H_