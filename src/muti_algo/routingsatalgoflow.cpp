#include "routingalgo.h"

#include <cstdio>
#include <algorithm>

using namespace z3;

const int fx[4] = {0, 0, 1, -1},
fy[4] = {1, -1, 0, 0};
const int mx[6] = {0, 0, 0, 1, 1, 2},
my[6] = {1, 2, 3, 2, 3, 3};

int RoutingSatAlgoFlow::idx(int x, int y, int k){
    if (k <= 1){
        if (k == 1) y--;
        if (y < 1 || y == m) return 0;
        return (x - 1) * (m - 1) + y;
    }
    else{
        if (k == 3) x--;
        if (x < 1 || x == n) return 0;
        return (x - 1) * m + y + (m - 1) * n;
    }
}

int RoutingSatAlgoFlow::getindx(int d, int x, int y, int k){
    if (idx(x, y, k) == 0)
        puts("Some Errors has happen");
    return (d - 1) * layer_cnt + idx(x, y, k) - 1;
}

void RoutingSatAlgoFlow::getTerm(expr &term, const expr_vector &method, int d, int x, int y){
    for (int u = 0; u < 4; ++u)
        if (idx(x, y, u))
            term = term + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, x, y, u)], one, zero));
}

void RoutingSatAlgoFlow::AddSatTerms(expr_vector &method){
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= layer_cnt; ++i){
            std::stringstream method_name;
            method_name << "method_" << d << "_" << i;
            method.push_back(sat_context.bool_const(method_name.str().c_str()));
        }
}

void RoutingSatAlgoFlow::EstablishModels(expr &total_pairs,
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
                    for (int k = 0; k < 4; ++k){
                        if (block[i][j] != d && idx(i, j, k))
                            sat_solver.add(!method[getindx(d, i, j, k)]);
                    }
            }
            else{
                // force that every block can only be visited once
                expr t = zero;
                for (int d = 1; d <= D; ++d)
                    for (int k = 0; k < 4; ++k){
                        if (idx(i, j, k) == 0) continue;
                        t = t + BoolToInt(method[getindx(d, i, j, k)]);
                        total_length = total_length + BoolToInt(method[getindx(d, i, j, k)]);
                    }
                sat_solver.add(t <= 2);
            }
        }
    
    // the source and sink terms
    for (int i = 1; i <= D; ++i){
        int sx = start_point[i].first,
        sy = start_point[i].second;
        int tx = end_point[i].first,
        ty = end_point[i].second;
        expr s = zero, t = zero;
        for (int k = 0; k < 4; ++k){
            if (idx(sx, sy, k))
                s = s + BoolToInt(method[getindx(i, sx, sy, k)]);
            if (idx(tx, ty, k))
                t = t + BoolToInt(method[getindx(i, tx, ty, k)]);
        }
        if (aug == 0){
            sat_solver.add((s == 1 && t == 1) || (s == 0 && t == 0));
        }
        else sat_solver.add(s == 1 && t == 1);
        total_pairs = total_pairs + s;
    }
    
    // the connection terms
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= n; ++i){
            for (int j = 1; j <= m; ++j){
                if (block[i][j]) continue;
                expr t_sum = zero;
                expr t_or = sat_context.bool_val(false);
                for (int k = 0; k < 4; ++k){
                    if (idx(i, j, k) == 0) continue;
                    t_or = t_or || method[getindx(d, i, j, k)];
                    t_sum = t_sum + BoolToInt(method[getindx(d, i, j, k)]);
                }
                sat_solver.add(!t_or || (t_sum == 2));
            }
        }
}

void RoutingSatAlgoFlow::FindPath(message &results,
                                  model &sat_model,
                                  const expr_vector &method,
                                  std::vector<std::pair<int, int> > start_point,
                                  std::vector<std::pair<int, int> > end_point){
    for (int i = 1; i <= D; ++i){
        int x = start_point[i].first, y = start_point[i].second, pk = 0;
        for (; ; ){
            if (x == end_point[i].first && y == end_point[i].second){
                results.push(i, x, y);
                break;
            }
            if (x == start_point[i].first && y == start_point[i].second){
                int t = 0;
                for (int k = 0; k < 4; ++k){
                    if (idx(x, y, k) == 0) continue;
                    if (GetValue(sat_model, method[getindx(i, x, y, k)]) == 't'){
                        x = x + fx[k], y = y + fy[k];
                        pk = k;
                        t = 1;
                        break;
                    }
                }
                // this is to check whether we select the pair
                if (t){
                    results.push(i, x - fx[pk], y - fy[pk]);
                    results.push(i, x, y);
                }
                else break;
            }
            else{
                results.push(i, x, y);
                for (int k = 0; k < 4; ++k){
                    if (idx(x, y, k) == 0) continue;
                    if (GetValue(sat_model, method[getindx(i, x, y, k)]) == 't'){
                        if (k == (pk ^ 1)) continue;
                        x = x + fx[k], y = y + fy[k];
                        pk = k;
                        break;
                    }
                }
            }
        }
    }
}

message RoutingSatAlgoFlow::SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug){
    n = n_, m = m_, D = D_;
    layer_cnt = (m - 1) * n + (n - 1) * m;
    std::vector<std::pair<int, int> > start_point, end_point;
    GetStartEndPoints(start_point, end_point, block);
    
    std::cout << ("RoutingSatAlgoFlow::Solve() INFO: starting defining variables") << std::endl;
    expr_vector method(sat_context);
    AddSatTerms(method);
    
    
    std::cout << "RoutingSatAlgoFlow::Solve() INFO: starting establishing model" << std::endl;
    optimize sat_solver(sat_context);
    params sat_para(sat_context);
    sat_para.set("priority",sat_context.str_symbol("pareto"));
    sat_solver.set(sat_para);
    expr total_length = zero;
    expr total_pairs = zero;
    EstablishModels(total_pairs, total_length, method, sat_solver, block, start_point, end_point, aug);
    
    
    std::cout << "RoutingSatAlgoFlow::Solve() INFO: starting solving model" << std::endl;
    return GetAns(total_pairs, total_length, method, sat_solver, block, start_point, end_point, aug);
}

message RoutingSatAlgoFlow::Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
    message results = SolveSat(n_, m_, D_, block, 0);
    return results;
}
