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
                                         std::vector<std::pair<int, int> > point[], 
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
                        t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
                        total_length = total_length + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
                    }
                sat_solver.add(t <= 2);
            }
        }
    
    // the source and sink terms
    for (int i = 1; i <= D; ++i){
        int sx = point[0][i].first,
        sy = point[0][i].second;
        int tx = point[1][i].first,
        ty = point[1][i].second;
        expr s = zero, t = zero;
        for (int k = 0; k < 4; ++k){
            if (idx(sx, sy, k))
                s = s + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, sx, sy, k)], one, zero));
            if (idx(tx, ty, k))
                t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, tx, ty, k)], one, zero));
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
                    t_sum = t_sum + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
                }
                sat_solver.add(!t_or || (t_sum == 2));
            }
        }
}

char RoutingSatAlgoFlow::GetValue(model &sat_model, const expr &t){
    std::stringstream st;
    st << sat_model.eval(t);
    return st.str()[0];
}

void RoutingSatAlgoFlow::FindPath(message &results,
                                  model &sat_model,
                                  const expr_vector &method,
                                  std::vector<std::pair<int, int> > point[]){
    for (int i = 1; i <= D; ++i){
        int x = point[0][i].first, y = point[0][i].second, pk = 0;
        for (; ; ){
            if (x == point[1][i].first && y == point[1][i].second){
                results.push(i, x, y);
                break;
            }
            if (x == point[0][i].first && y == point[0][i].second){
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
    // find the source(point[0]) and sink(point[1])
    std::vector<std::pair<int, int> > point[2];
    point[0].resize(D_ + 1);
    point[1].resize(D_ + 1);
    n = n_, m = m_, D = D_;
    
    int *cnt = new int[D + 1];
    for (int i = 1; i <= D; ++i) cnt[i] = 0;
    
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            if (block[i][j] > 0)
                point[cnt[block[i][j]]++][block[i][j]] = std::make_pair(i, j);
    delete []cnt;

    layer_cnt = (m - 1) * n + (n - 1) * m;


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
    EstablishModels(total_pairs, total_length, method, sat_solver, block, point, aug);


    std::cout << "RoutingSatAlgoFlow::Solve() INFO: starting solving model" << std::endl;
    // solving the model
    optimize::handle opt = sat_solver.maximize(total_pairs);
    if (aug)
    	opt = sat_solver.minimize(total_length);

    message results(D, n, m, block);
    if (sat_solver.check()){
    	if (aug == 0){
    		std::stringstream st;
			st << sat_solver.lower(opt);
			int d = st.str()[0] - '0';
			std::cout << d << std::endl;
			if (d < D)
				results.set_totpair(d);
			else return SolveSat(n, m, D, block, 1);
    	}
    	else
    		std::cout << sat_solver.upper(opt) << std::endl;
        
        model sat_model = sat_solver.get_model();
        std::cout << sat_model.eval(total_pairs) << std::endl;

        // find the path
        FindPath(results, sat_model, method, point);
    }
    return results;
}

message RoutingSatAlgoFlow::Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
	return SolveSat(n_, m_, D_, block, 0);
}

// RoutingSatAlgoFlowPrune
int RoutingSatAlgoFlowPrune::idx(int x, int y, int k){
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

int RoutingSatAlgoFlowPrune::getindx(int d, int x, int y, int k){
    if (idx(x, y, k) == 0)
        puts("Some Errors has happen");
    return (d - 1) * layer_cnt + idx(x, y, k) - 1;
}

void RoutingSatAlgoFlowPrune::getTerm(expr &term, const expr_vector &method, int d, int x, int y){
    for (int u = 0; u < 4; ++u)
        if (idx(x, y, u))
            term = term + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, x, y, u)], one, zero));
}

message RoutingSatAlgoFlowPrune::SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug){
    // find the source(point[0]) and sink(point[1])
    std::vector<std::pair<int, int> > point[2];
    point[0].resize(D_ + 1);
    point[1].resize(D_ + 1);
    n = n_, m = m_, D = D_;
    
    int *cnt = new int[D + 1];
    for (int i = 1; i <= D; ++i) cnt[i] = 0;
    
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            if (block[i][j] > 0)
                point[cnt[block[i][j]]++][block[i][j]] = std::make_pair(i, j);
    delete []cnt;
    
    layer_cnt = (m - 1) * n + (n - 1) * m;
    std::cout << ("RoutingSatAlgoFlow::Solve() INFO: starting defining variables") << std::endl;
    // add sat terms
    expr_vector method(sat_context);
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= layer_cnt; ++i){
            std::stringstream method_name;
            method_name << "method_" << d << "_" << i;
            method.push_back(sat_context.bool_const(method_name.str().c_str()));
        }
    
    std::cout << "RoutingSatAlgoFlow::Solve() INFO: starting establishing model" << std::endl;
    // establish the models
    
    optimize sat_solver(sat_context);
    params sat_para(sat_context);
    sat_para.set("priority",sat_context.str_symbol("pareto"));
    sat_solver.set(sat_para);
    
    expr total_length = zero;
    expr total_pairs = zero;
    
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
                        t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
                        total_length = total_length + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
                    }
                sat_solver.add(t <= 2);
            }
        }
    
    // the source and sink terms
    for (int i = 1; i <= D; ++i){
        int sx = point[0][i].first,
        sy = point[0][i].second;
        int tx = point[1][i].first,
        ty = point[1][i].second;
        expr s = zero, t = zero;
        for (int k = 0; k < 4; ++k){
            if (idx(sx, sy, k))
                s = s + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, sx, sy, k)], one, zero));
            if (idx(tx, ty, k))
                t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, tx, ty, k)], one, zero));
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
                    t_sum = t_sum + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
                }
                sat_solver.add(!t_or || (t_sum == 2));
            }
        }

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

    std::cout << "RoutingSatAlgoFlow::Solve() INFO: starting solving model" << std::endl;
    // solving the model
    optimize::handle opt = sat_solver.maximize(total_pairs);
    if (aug)
        opt = sat_solver.minimize(total_length);
    
    message results(D, n, m, block);
    if (sat_solver.check()){
        if (aug == 0){
            std::stringstream st;
            st << sat_solver.lower(opt);
            int d = st.str()[0] - '0';
            std::cout << d << std::endl;
            if (d < D)
                results.set_totpair(d);
            else return SolveSat(n, m, D, block, 1);
        }
        else
            std::cout << sat_solver.upper(opt) << std::endl;
        
        model sat_model = sat_solver.get_model();
        std::cout << sat_model.eval(total_pairs) << std::endl;
        
        results.set_ok(1);
        // find the path
        for (int i = 1; i <= D; ++i){
            int x = point[0][i].first, y = point[0][i].second, pk = 0;
            for (; ; ){
                if (x == point[1][i].first && y == point[1][i].second){
                    results.push(i, x, y);
                    break;
                }
                if (x == point[0][i].first && y == point[0][i].second){
                    int t = 0;
                    for (int k = 0; k < 4; ++k){
                        if (idx(x, y, k) == 0) continue;
                        std::stringstream st;
                        st << sat_model.eval(method[getindx(i, x, y, k)]);
                        if (st.str()[0] == 't'){
                            x = x + fx[k], y = y + fy[k];
                            pk = k;
                            t = 1;
                            break;
                        }
                    }
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
                        std::stringstream st;
                        st << sat_model.eval(method[getindx(i, x, y, k)]);
                        if (st.str()[0] == 't'){
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
    return results;
}

message RoutingSatAlgoFlowPrune::Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
    return SolveSat(n_, m_, D_, block, 0);
}

// RoutingSatAlgoPointPrune
int RoutingSatAlgoPointPrune::idx(int x, int y){
    return (x - 1) * m + y;
}

int RoutingSatAlgoPointPrune::getindx(int d, int x, int y){
    return (d - 1) * n * m + idx(x, y) - 1;
}

message RoutingSatAlgoPointPrune::SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug){
    // find the source(point[0]) and sink(point[1])
    std::vector<std::pair<int, int> > point[2];
    point[0].resize(D_ + 1);
    point[1].resize(D_ + 1);
    n = n_, m = m_, D = D_;
    
    int *cnt = new int[D + 1];
    for (int i = 1; i <= D; ++i) cnt[i] = 0;
    
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            if (block[i][j] > 0)
                point[cnt[block[i][j]]++][block[i][j]] = std::make_pair(i, j);
    delete []cnt;
    
    std::cout << ("RoutingSatAlgoPointPrune::Solve() INFO: starting defining variables") << std::endl;
    // add sat terms
    expr_vector method(sat_context);
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= n * m; ++i){
            std::stringstream method_name;
            method_name << "method_" << d << "_" << i;
            method.push_back(sat_context.bool_const(method_name.str().c_str()));
        }
    
    std::cout << "RoutingSatAlgoPointPrune::Solve() INFO: starting establishing model" << std::endl;
    // establish the models
    
    optimize sat_solver(sat_context);
    params sat_para(sat_context);
    sat_para.set("priority",sat_context.str_symbol("pareto"));
    sat_solver.set(sat_para);
    
    expr total_length = zero;
    expr total_pairs = zero;
    
    // the limit terms.
    std::cout << ("limit terms") << std::endl;
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
                    t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j)], one, zero));
                    total_length = total_length + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j)], one, zero));
                }
                sat_solver.add(t <= 1);
            }
        }
    
    // the source and sink terms
    std::cout << ("source and sink terms") << std::endl;
    for (int i = 1; i <= D; ++i){
        int sx = point[0][i].first,
        sy = point[0][i].second;
        int tx = point[1][i].first,
        ty = point[1][i].second;
        if (aug == 0)
            sat_solver.add((method[getindx(i, sx, sy)] && method[getindx(i, tx, ty)])
                            || (!method[getindx(i, sx, sy)] && !method[getindx(i, tx, ty)]));
        else sat_solver.add(method[getindx(i, sx, sy)] && method[getindx(i, tx, ty)]);
        total_pairs = total_pairs + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, sx, sy)], one, zero));;
    }
    
    // the connection terms
    std::cout << ("connection terms") << std::endl;
    for (int d = 1; d <= D; ++d)
        for (int i = 1; i <= n; ++i){
            for (int j = 1; j <= m; ++j){
                if (block[i][j] != d && block[i][j] != 0) continue;
                expr t_sum = zero;
                for (int k = 0; k < 4; ++k){
                    int x = i + fx[k],
                        y = j + fy[k];
                    if (block[x][y] != d && block[x][y] != 0) continue;
                    t_sum = t_sum + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, x, y)], one, zero));
                }
                sat_solver.add(!method[getindx(d, i, j)] || (t_sum == (1 + (block[i][j] == 0))));
            }
        }
    // prune
    std::cout << "prune terms" << std::endl;
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
                t_sum = t_sum + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j)], one, zero));
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
                t_sum = t_sum + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j)], one, zero));
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

    std::cout << "RoutingSatAlgoPointPrune::Solve() INFO: starting solving model" << std::endl;
    // solving the model
    optimize::handle opt = sat_solver.maximize(total_pairs);
    if (aug)
        opt = sat_solver.minimize(total_length);
    
    message results(D, n, m, block);
    if (sat_solver.check()){
        if (aug == 0){
            std::stringstream st;
            st << sat_solver.lower(opt);
            int d = st.str()[0] - '0';
            std::cout << d << std::endl;
            if (d < D)
                results.set_totpair(d);
            else return SolveSat(n, m, D, block, 1);
        }
        else
            std::cout << sat_solver.upper(opt) << std::endl;
        
        model sat_model = sat_solver.get_model();
        std::cout << sat_model.eval(total_pairs) << std::endl;
        
        results.set_ok(1);
        // find the path
        for (int i = 1; i <= D; ++i){
            int x = point[0][i].first, y = point[0][i].second;
            std::stringstream st;
            st << sat_model.eval(method[getindx(i, x, y)]);
            if (st.str()[0] == 'f')
                continue;
            int pk = -1;
            for (; ; ){
                results.push(i, x, y);
                if (x == point[1][i].first && y == point[1][i].second)
                    break;
                for (int k = 0; k < 4; ++k){
                    int tx = x + fx[k],
                        ty = y + fy[k];
                    if (tx < 1 || ty < 1 || tx > n || ty > m) continue;
                    std::stringstream st;
                    st << sat_model.eval(method[getindx(i, tx, ty)]);
                    if (st.str()[0] == 't' && k != (pk ^ 1)){
                        x = tx;
                        y = ty;
                        pk = k;
                        break;
                    }
                }
            }
        }
    }
    return results;
}

message RoutingSatAlgoPointPrune::Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
    return SolveSat(n_, m_, D_, block, 0);
}