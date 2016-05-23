#include "routingalgo.h"

#include <cstdio>
#include <algorithm>

using namespace z3;

const int fx[4] = {0, 0, 1, -1},
fy[4] = {1, -1, 0, 0};
const int mx[6] = {0, 0, 0, 1, 1, 2},
my[6] = {1, 2, 3, 2, 3, 3};

int RoutingSatAlgoPointPrune::idx(int x, int y){
    return (x - 1) * m + y;
}

int RoutingSatAlgoPointPrune::getindx(int d, int x, int y){
    return (d - 1) * n * m + idx(x, y) - 1;
}

void RoutingSatAlgoPointPrune::AddSatTerms(expr_vector &method){
    // add sat terms
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= n * m; ++i){
            std::stringstream method_name;
            method_name << "method_" << d << "_" << i;
            method.push_back(sat_context.bool_const(method_name.str().c_str()));
        }
}

void RoutingSatAlgoPointPrune::EstablishModels(expr &total_pairs,
                                               expr &total_length,
                                               const expr_vector &method,
                                               optimize &sat_solver,
                                               std::vector<std::vector<int> > block,
                                               std::vector<std::pair<int, int> > start_point,
                                               std::vector<std::pair<int, int> > end_point,
                                               int aug){
    // the limit terms.
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j){
            if (block[i][j]){
                // if it is a barrier or is not in the corresponding layer
                // set the term to 0
                for (int d = 1; d <= D; ++d)
                    if (block[i][j] != d) sat_solver.add(!method[getindx(d, i, j)]);
            }
            else{
                // force that every block can only be visited once
                expr t = zero;
                for (int d = 1; d <= D; ++d){
                    t = t + BoolToInt(method[getindx(d, i, j)]);
                    total_length = total_length + BoolToInt(method[getindx(d, i, j)]);
                }
                sat_solver.add(t <= 1);
            }
        }
    
    // the source and sink terms
    for (int i = 1; i <= D; ++i){
        int sx = start_point[i].first,
        sy = start_point[i].second;
        int tx = end_point[i].first,
        ty = end_point[i].second;
        if (aug == 0)
            sat_solver.add((method[getindx(i, sx, sy)] && method[getindx(i, tx, ty)])
                           || (!method[getindx(i, sx, sy)] && !method[getindx(i, tx, ty)]));
        else sat_solver.add(method[getindx(i, sx, sy)] && method[getindx(i, tx, ty)]);
        total_pairs = total_pairs + BoolToInt(method[getindx(i, sx, sy)]);
    }
    
    // the connection terms
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= n; ++i){
            for (int j = 1; j <= m; ++j){
                if (block[i][j] != d && block[i][j] != 0) continue;
                expr t_sum = zero;
                for (int k = 0; k < 4; ++k){
                    int x = i + fx[k],
                    y = j + fy[k];
                    if (block[x][y] != d && block[x][y] != 0) continue;
                    t_sum = t_sum + BoolToInt(method[getindx(d, x, y)]);
                }
                sat_solver.add(!method[getindx(d, i, j)] || (t_sum == (1 + (block[i][j] == 0))));
            }
        }
}

void RoutingSatAlgoPointPrune::PruneTerms(const expr_vector &method,
                                          optimize &sat_solver,
                                          std::vector<std::vector<int> > block){
    // prune
    for (int i = 1; i <= n; ++i){
        for (int j = 1; j <= m; ++j){
            if (block[i][j]) continue;
            // forbid
            // . x
            // x ?
            if (block[i][j + 1]) continue;
            if (block[i + 1][j]) continue;
            if (block[i + 1][j + 1]) continue;
            expr t_sum = zero;
            expr t_or = sat_context.bool_val(false);
            for (int d = 1; d <= D; ++d){
                t_sum = t_sum + BoolToInt(method[getindx(d, i, j)]);
                t_or = t_or || (method[getindx(d, i + 1, j)] && method[getindx(d, i, j + 1)]);
            }
            sat_solver.add((t_sum > 0) || !t_or);
        }
    }
    for (int i = 1; i <= n; ++i){
        for (int j = 1; j <= m; ++j){
            // forbid
            // x .
            // ? x
            if (block[i][j - 1]) continue;
            if (block[i + 1][j]) continue;
            if (block[i + 1][j - 1]) continue;
            expr t_sum = zero;
            expr t_or = sat_context.bool_val(false);
            for (int d = 1; d <= D; ++d){
                t_sum = t_sum + BoolToInt(method[getindx(d, i, j)]);
                t_or = t_or || (method[getindx(d, i + 1, j)] && method[getindx(d, i, j - 1)]);
            }
            sat_solver.add((t_sum > 0) || !t_or);
        }
    }
    for (int i = 1; i < n; ++i){
        for (int j = 1; j < m; ++j){
            // forbid
            // x x
            // x x
            for (int d = 1; d <= D; ++d)
                sat_solver.add(!(method[getindx(d, i, j)] && method[getindx(d, i + 1, j)]
                                 && method[getindx(d, i, j + 1)] && method[getindx(d, i + 1, j + 1)]));
        }
    }
    
}

void RoutingSatAlgoPointPrune::FindPath(message &results,
                                        model &sat_model,
                                        const expr_vector &method,
                                        std::vector<std::pair<int, int> > start_point,
                                        std::vector<std::pair<int, int> > end_point){
    for (int i = 1; i <= D; ++i){
        int x = start_point[i].first, y = start_point[i].second;
        if (GetValue(sat_model, method[getindx(i, x, y)]) == 'f') continue;
        
        int pk = -1;
        for (; ; ){
            results.push(i, x, y);
            if (x == end_point[i].first && y == end_point[i].second) break;
            
            for (int k = 0; k < 4; ++k){
                int tx = x + fx[k],
                ty = y + fy[k];
                if (tx < 1 || ty < 1 || tx > n || ty > m) continue;
                if (GetValue(sat_model, method[getindx(i, tx, ty)]) == 't' && k != (pk ^ 1)){
                    x = tx;
                    y = ty;
                    pk = k;
                    break;
                }
            }
        }
    }
}

message RoutingSatAlgoPointPrune::SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug){
    n = n_, m = m_, D = D_;
    std::vector<std::pair<int, int> > start_point, end_point;
    GetStartEndPoints(start_point, end_point, block);
    
    std::cout << ("RoutingSatAlgoPointPrune::SolveSat() INFO: starting defining variables") << std::endl;
    expr_vector method(sat_context);
    AddSatTerms(method);
    
    std::cout << "RoutingSatAlgoPointPrune::SolveSat() INFO: starting establishing model" << std::endl;
    optimize sat_solver(sat_context);
    params sat_para(sat_context);
    sat_para.set("priority",sat_context.str_symbol("pareto"));
    sat_solver.set(sat_para);
    expr total_length = zero;
    expr total_pairs = zero;
    EstablishModels(total_pairs, total_length, method, sat_solver, block, start_point, end_point, aug);
    
    std::cout << "RoutingSatAlgoPointPrune::SolveSat() INFO: starting adding prune terms" << std::endl;
    PruneTerms(method, sat_solver, block);
    
    std::cout << "RoutingSatAlgoPointPrune::SolveSat() INFO: starting solving model" << std::endl;
    return GetAns(total_pairs, total_length, method, sat_solver, block, start_point, end_point, aug);
}