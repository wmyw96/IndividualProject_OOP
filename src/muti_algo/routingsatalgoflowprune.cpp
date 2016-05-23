#include "routingalgo.h"

#include <cstdio>
#include <algorithm>

using namespace z3;

const int fx[4] = {0, 0, 1, -1},
fy[4] = {1, -1, 0, 0};
const int mx[6] = {0, 0, 0, 1, 1, 2},
my[6] = {1, 2, 3, 2, 3, 3};


void RoutingSatAlgoFlowPrune::PruneTerms(const expr_vector &method,
                                         optimize &sat_solver,
                                         std::vector<std::vector<int> > block){
    // prune
    for (int i = 1; i <= n; ++i){
        for (int j = 1; j <= m; ++j){
            if (block[i][j]) continue;
            // forbid
            // . |
            // - +
            if (block[i][j + 1]) continue;
            if (block[i + 1][j]) continue;
            if (block[i + 1][j + 1]) continue;
            expr t_sum = zero;
            expr t_or = sat_context.bool_val(false);
            for (int d = 1; d <= D; ++d){
                getTerm(t_sum, method, d, i, j);
                t_or = t_or || (method[getindx(d, i + 1, j + 1, 1)] && method[getindx(d, i + 1, j + 1, 3)]);
            }
            sat_solver.add((t_sum > 0) || !t_or);
        }
    }
    for (int i = 1; i <= n; ++i){
        for (int j = 1; j <= m; ++j){
            // forbid
            // | .
            // + -
            if (block[i][j - 1]) continue;
            if (block[i + 1][j]) continue;
            if (block[i + 1][j - 1]) continue;
            expr t_sum2 = zero;
            expr t_or2 = sat_context.bool_val(false);
            for (int d = 1; d <= D; ++d){
                getTerm(t_sum2, method, d, i, j);
                t_or2 = t_or2 || (method[getindx(d, i + 1, j - 1, 0)] && method[getindx(d, i + 1, j - 1, 3)]);
            }
            sat_solver.add((t_sum2 > 0) || !t_or2);
        }
    }
    for (int i = 1; i < n; ++i){
        for (int j = 1; j < m; ++j){
            // forbid
            // | |
            if (idx(i, j, 2) == 0 || idx(i, j + 1, 2) == 0) continue;
            for (int d = 1; d <= D; ++d)
                sat_solver.add(!method[getindx(d, i, j, 2)] || !method[getindx(d, i, j + 1, 2)]);
        }
    }
    for (int i = 1; i < n; ++i){
        for (int j = 1; j < m; ++j){
            // forbid
            // -
            // -
            if (idx(i, j, 0) == 0 || idx(i + 1, j, 0) == 0) continue;
            for (int d = 1; d <= D; ++d)
                sat_solver.add(!method[getindx(d, i, j, 0)] || !method[getindx(d, i + 1, j, 0)]);
        }
    }
}

message RoutingSatAlgoFlowPrune::SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug){
    n = n_, m = m_, D = D_;
    layer_cnt = (m - 1) * n + (n - 1) * m;
    std::vector<std::pair<int, int> > start_point, end_point;
    GetStartEndPoints(start_point, end_point, block);
    
    std::cout << ("RoutingSatAlgoFlowPrune::SolveSat() INFO: starting defining variables") << std::endl;
    expr_vector method(sat_context);
    AddSatTerms(method);
    
    std::cout << "RoutingSatAlgoFlowPrune::SolveSat() INFO: starting establishing model" << std::endl;
    optimize sat_solver(sat_context);
    params sat_para(sat_context);
    sat_para.set("priority",sat_context.str_symbol("pareto"));
    sat_solver.set(sat_para);
    expr total_length = zero;
    expr total_pairs = zero;
    EstablishModels(total_pairs, total_length, method, sat_solver, block, start_point, end_point, aug);
    
    std::cout << "RoutingSatAlgoFlowPrune::SolveSat() INFO: starting adding prune terms" << std::endl;
    PruneTerms(method, sat_solver, block);
    
    std::cout << "RoutingSatAlgoFlowPrune::SolveSat() INFO: starting solving model" << std::endl;
    return GetAns(total_pairs, total_length, method, sat_solver, block, start_point, end_point, aug);
}
