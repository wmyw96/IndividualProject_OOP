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
    virtual message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block) = 0;
};

class RoutingSatAlgo : public RoutingAlgo{
protected:
    z3::context sat_context; // a component of z3 solver to define expressions
    z3::expr one, zero;      // const one(1) and zero(0)
    int n, m, D;             // the maze size = n * m, the number of pairs = D
    virtual message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug) = 0;
public:
    RoutingSatAlgo() : sat_context(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) {}
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
        return SolveSat(n_, m_, D_, block, 0);
    }
};

class RoutingSatAlgoFlow : public RoutingSatAlgo{
    int layer_cnt;
protected:
    int idx(int x, int y, int type);
    // Get the index of No.[d] pair, ([x], [y]) position, direction type [type].
    int getindx(int d, int x, int y, int type);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y);
    void AddSatTerms(z3::expr_vector &method);
    void EstablishModels(z3::expr &total_pairs,
                         z3::expr &total_length,
                         const z3::expr_vector &method,
                         z3::optimize &sat_solver,
                         std::vector<std::vector<int> > block, 
                         std::vector<std::pair<int, int> > point[], 
                         int aug);
    char GetValue(z3::model &sat_model, const z3::expr &t);
    void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > point[]);
    message SolveSat(int n_, int m_, int D_, 
                     std::vector<std::vector<int> > block, 
                     int aug);
public:
    RoutingSatAlgoFlow() : RoutingSatAlgo() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

class RoutingSatAlgoFlowPrune : public RoutingSatAlgo{
    int layer_cnt;
private:
    int idx(int x, int y, int type);
    // Get the index of No.[d] pair, ([x], [y]) position, direction type [type].
    int getindx(int d, int x, int y, int type);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y);
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
public:
    RoutingSatAlgoFlowPrune() : RoutingSatAlgo() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

class RoutingSatAlgoPointPrune : public RoutingSatAlgo{
private:
    int idx(int x, int y);
    // Get the index of No.[d] pair, ([x], [y]) position.
    int getindx(int d, int x, int y);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y);
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
public:
    RoutingSatAlgoPointPrune() : RoutingSatAlgo() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};


#endif //ROUTINGALGO_H_