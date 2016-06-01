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
    RoutingAlgo() {}
    virtual ~RoutingAlgo() {}
    virtual message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block) = 0;
};

/** 
  * \class <RoutingSatAlgo> [routingalgo.h]
  * \class RoutingSatAlgo
  * \brief An abstract class of algorithm implement of sat model
  * \author YiHong Gu
  * \date 2016-05-25
  *
  * No details.
*/
class RoutingSatAlgo : public RoutingAlgo{
public:
    z3::context sat_context; // a component of z3 solver to define expressions
    z3::expr one, zero;      // const one(1) and zero(0)
    int n, m, D;             // the maze size = n * m, the number of pairs = D

    // This is just an adaptor
    // It convert the expression of z3 to a char ('t' or 'f')
    char GetValue(z3::model &sat_model, const z3::expr &t);

    // Get the origin and terminal
    void GetStartEndPoints(std::vector<std::pair<int, int> > &start_point, 
                           std::vector<std::pair<int, int> > &end_point,
                           std::vector<std::vector<int> > block);

    virtual message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug) = 0;

    virtual void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > start_point, 
                  std::vector<std::pair<int, int> > end_point) = 0;

    // Get the answer according to the arguments [aug]
    message GetAns(const z3::expr &total_pairs,
                   const z3::expr &total_length,
                   const z3::expr_vector &method,
                   z3::optimize &sat_solver,
                   std::vector<std::vector<int> > block,
                   std::vector<std::pair<int, int> > start_point, 
                   std::vector<std::pair<int, int> > end_point,
                   int aug);

    // Convert a bool expression in z3 to a int expression
    z3::expr BoolToInt(const z3::expr &t);
    RoutingSatAlgo() : sat_context(), one(sat_context.int_val(1)), zero(sat_context.int_val(0)) { }
    ~RoutingSatAlgo() {}
};

/** 
  * \class <RoutingSatAlgoFlow> [routingalgo.h]
  * \class RoutingSatAlgoFlow
  * \brief An abstract class of algorithm implement of using network flow model(Model 2)
  * \author YiHong Gu
  * \date 2016-05-25
  *
  * see the whitepaper
*/
class RoutingSatAlgoFlow : public RoutingSatAlgo{
protected:
    // The number of variable per layer
    int layer_cnt;
    // Get the index of ([x], [y]) position, direction type [type].
    int idx(int x, int y, int type);
    // Get the index of No.[d] pair, ([x], [y]) position, direction type [type].
    int getindx(int d, int x, int y, int type);
    // Get the term
    void getTerm(z3::expr &term, const z3::expr_vector &method, int d, int x, int y);
    // Define binary variables of the model
    void AddSatTerms(z3::expr_vector &method);
    // Define contraints of the model
    void EstablishModels(z3::expr &total_pairs,
                         z3::expr &total_length,
                         const z3::expr_vector &method,
                         z3::optimize &sat_solver,
                         std::vector<std::vector<int> > block, 
                         std::vector<std::pair<int, int> > start_point, 
                         std::vector<std::pair<int, int> > end_point,
                         int aug);
    // Find the final path
    void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > start_point, 
                  std::vector<std::pair<int, int> > end_point);
    // The main process of solving the problem
    message SolveSat(int n_, int m_, int D_, 
                     std::vector<std::vector<int> > block, 
                     int aug);
public:
    RoutingSatAlgoFlow() : RoutingSatAlgo() {}
    ~RoutingSatAlgoFlow() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block);
};

/** 
  * \class <RoutingSatAlgoFlowPrune> [routingalgo.h]
  * \class RoutingSatAlgoFlowPrune
  * \brief An abstract class of algorithm implement of using network flow model (Model 2) and prune some conditions
  * \author YiHong Gu
  * \date 2016-05-25
  *
  * see the whitepaper
*/
class RoutingSatAlgoFlowPrune : public RoutingSatAlgoFlow{
protected:
    // Add some Prune Contraints
    void PruneTerms(const z3::expr_vector &method, 
                    z3::optimize &sat_solver, 
                    std::vector<std::vector<int> > block);
    // The Main Process of Solving the Problem
    message SolveSat(int n_, int m_, int D_, 
                     std::vector<std::vector<int> > block, 
                     int aug);
public:
    RoutingSatAlgoFlowPrune() : RoutingSatAlgoFlow() {}
    ~RoutingSatAlgoFlowPrune() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
        return SolveSat(n_, m_, D_, block, 0);
    }
};

/** 
  * \class <RoutingSatAlgoPointPrune> [routingalgo.h]
  * \class RoutingSatAlgoPointPrune
  * \brief An abstract class of algorithm implement of using the mixed model (Model 3) and prune some conditions
  * \author YiHong Gu
  * \date 2016-05-25
  *
  * see the whitepaper
*/
class RoutingSatAlgoPointPrune : public RoutingSatAlgo{
protected:
    // Get the index of ([x], [y]) position
    int idx(int x, int y);
    // Get the index of No.[d] pair, ([x], [y]) position.
    int getindx(int d, int x, int y);
    // Define binary variables of the model
    void AddSatTerms(z3::expr_vector &method);
    // Define contraints of the model
    void EstablishModels(z3::expr &total_pairs,
                         z3::expr &total_length,
                         const z3::expr_vector &method,
                         z3::optimize &sat_solver,
                         std::vector<std::vector<int> > block, 
                         std::vector<std::pair<int, int> > start_point, 
                         std::vector<std::pair<int, int> > end_point,
                         int aug);
    // Add some prune contraints
    void PruneTerms(const z3::expr_vector &method, 
                    z3::optimize &sat_solver, 
                    std::vector<std::vector<int> > block);
    // Find the final path
    void FindPath(message &results,
                  z3::model &sat_model,
                  const z3::expr_vector &method,
                  std::vector<std::pair<int, int> > start_point, 
                  std::vector<std::pair<int, int> > end_point);
public:
    // The main process of solving the problem
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
    RoutingSatAlgoPointPrune() : RoutingSatAlgo() {}
    ~RoutingSatAlgoPointPrune() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
        return SolveSat(n_, m_, D_, block, 0);
    }
};

class RoutingSatAlgoPointPrunePlus : public RoutingSatAlgoPointPrune{
protected:
    // Add some more prune contraints
    void MorePruneTerms(const z3::expr_vector &method, 
                    z3::optimize &sat_solver, 
                    std::vector<std::vector<int> > block);
public:
    // The main process of solving the problem
    message SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug);
    RoutingSatAlgoPointPrunePlus() : RoutingSatAlgoPointPrune() {}
    ~RoutingSatAlgoPointPrunePlus() {}
    // A concrete implement of the sat model algorithm
    message Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
        return SolveSat(n_, m_, D_, block, 0);
    }
};


#endif //ROUTINGALGO_H_