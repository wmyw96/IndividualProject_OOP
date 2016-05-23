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
public:
    z3::context sat_context; // a component of z3 solver to define expressions
    z3::expr one, zero;      // const one(1) and zero(0)
    int n, m, D;             // the maze size = n * m, the number of pairs = D
    char GetValue(z3::model &sat_model, const z3::expr &t);
    void GetStartEndPoints(std::vector<std::pair<int, int> > &start_point, 
                           std::vector<std::pair<int, int> > &end_point,
                           std::vector<std::vector<int> > block);
    virtual message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug) = 0;
    virtual void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > start_point, 
                  std::vector<std::pair<int, int> > end_point) = 0;
    message GetAns(const z3::expr &total_pairs,
                   const z3::expr &total_length,
                   const z3::expr_vector &method,
                   z3::optimize &sat_solver,
                   std::vector<std::vector<int> > block,
                   std::vector<std::pair<int, int> > start_point, 
                   std::vector<std::pair<int, int> > end_point,
                   int aug);
    z3::expr BoolToInt(const z3::expr &t);
    RoutingSatAlgo() : sat_context(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) { }
    ~RoutingSatAlgo() {}
};

class RoutingSatAlgoFlow : public RoutingSatAlgo{
protected:
    int layer_cnt;
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
                         std::vector<std::pair<int, int> > start_point, 
                         std::vector<std::pair<int, int> > end_point,
                         int aug);
    void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > start_point, 
                  std::vector<std::pair<int, int> > end_point);
    message SolveSat(int n_, int m_, int D_, 
                     std::vector<std::vector<int> > block, 
                     int aug);
public:
    RoutingSatAlgoFlow() : RoutingSatAlgo() {}
    ~RoutingSatAlgoFlow() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

class RoutingSatAlgoFlowPrune : public RoutingSatAlgoFlow{
protected:
    void PruneTerms(const z3::expr_vector &method, 
                    z3::optimize &sat_solver, 
                    std::vector<std::vector<int> > block);
    // A concrete implement of the sat model algorithm
    message SolveSat(int n_, int m_, int D_, 
                     std::vector<std::vector<int> > block, 
                     int aug);
public:
    RoutingSatAlgoFlowPrune() : RoutingSatAlgoFlow() {}
    ~RoutingSatAlgoFlowPrune() {}
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
        return SolveSat(n_, m_, D_, block, 0);
    }
};

class RoutingSatAlgoPointPrune : public RoutingSatAlgo{
protected:
    int idx(int x, int y);
    // Get the index of No.[d] pair, ([x], [y]) position.
    int getindx(int d, int x, int y);
    // Get the term
    void AddSatTerms(z3::expr_vector &method);
    void EstablishModels(z3::expr &total_pairs,
                         z3::expr &total_length,
                         const z3::expr_vector &method,
                         z3::optimize &sat_solver,
                         std::vector<std::vector<int> > block, 
                         std::vector<std::pair<int, int> > start_point, 
                         std::vector<std::pair<int, int> > end_point,
                         int aug);
    void PruneTerms(const z3::expr_vector &method, 
                    z3::optimize &sat_solver, 
                    std::vector<std::vector<int> > block);
    void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > start_point, 
                  std::vector<std::pair<int, int> > end_point);
public:
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
    RoutingSatAlgoPointPrune() : RoutingSatAlgo() {}
    ~RoutingSatAlgoPointPrune() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
        return SolveSat(n_, m_, D_, block, 0);
    }
};


#endif //ROUTINGALGO_H_