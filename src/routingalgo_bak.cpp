#include "routingalgo.h"

#include <cstdio>
#include <algorithm>

using namespace z3;

const int fx[4] = {0, 0, -1, 1},
		  fy[4] = {1, -1, 0, 0};
const int mx[6] = {0, 0, 0, 1, 1, 2},
		  my[6] = {1, 2, 3, 2, 3, 3};

int RoutingSatAlgo::getindx(int d, int x, int y, int k){
    int p = (x - 1) * m + y;
    return (d - 1) * n * m * 6 + (p - 1) * 6 + k;
}

void RoutingSatAlgo::getTerm(expr &term, const expr_vector &method, int d, int x, int y, int k, int block_type){
    if (x < 1 || y < 1 || x > n || y > m)
        return;
    if (block_type == 1000)
        return;
    if (block_type)
        term = to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, x, y, k)], one, zero));
    else{
        for (int u = 0; u < 6; ++u)
            if (mx[u] == k || my[u] == k)
                term = term + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, x, y, u)], one, zero));
    }
}

message RoutingSatAlgo::SolveSat(int n_, int m_, int D_, std::vector<std::vector<int> > block, int aug_sel, int aug_limit_t){
	// find the source(point[0]) and sink(point[1])
	std::vector<std::pair<int, int> > point[2];
	point[0].resize(D_ + 1);
	point[1].resize(D_ + 1);
	n = n_, m = m_, D = D_;

	//one = sat_context.int_val(1);
	//zero = sat_context.int_val(0);
	
	int *cnt = new int[D + 1];
	for (int i = 1; i <= D; ++i) cnt[i] = 0;

	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j)
			if (block[i][j] > 0)
				point[cnt[block[i][j]]++][block[i][j]] = std::make_pair(i, j);
	delete []cnt;
    
	//std::cout << ("RoutingSatAlgo::Solve() INFO: starting defining variables") << std::endl;
	// add sat terms
	expr_vector method(sat_context);
	for (int d = 1; d <= D; ++d)
		for (int i = 1; i <= n; ++i)
			for (int j = 1; j <= m; ++j)
				for (int k = 0; k < 6; ++k){
					std::stringstream method_name;
					method_name << "method_" << d << " " << i << "_" << j << "_" << k;
					method.push_back(sat_context.bool_const(method_name.str().c_str()));
				}

	//std::cout << "RoutingSatAlgo::Solve() INFO: starting establishing model" << std::endl;
	// establish the models

	solver sat_solver(sat_context);

	expr total_length = zero;

	// the limit terms.
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j){
			if (block[i][j]){
				// if it is a barrier or is not in the corresponding layer
				// set the term to 0
				for (int d = 1; d <= D; ++d)
					for (int k = 0; k < 6; ++k)
						if (block[i][j] != d || k > 3) sat_solver.add(!method[getindx(d, i, j, k)]);
			}
			else{
				// force that every block can only be visited once
				expr t = zero;
				for (int d = 1; d <= D; ++d)
					for (int k = 0; k < 6; ++k){
						t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
						total_length = total_length + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(d, i, j, k)], one, zero));
					}
				sat_solver.add(t <= 1);
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
			s = s + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, sx, sy, k)], one, zero));
			t = t + to_expr(sat_context, Z3_mk_ite(sat_context, method[getindx(i, tx, ty, k)], one, zero));
		}
		if ((aug_sel & (1 << i - 1)) == 0)
			sat_solver.add(s == 0 && t == 0);
		else sat_solver.add(s == 1 && t == 1);
	}

    // the connection terms
	for (int d = 1; d <= D; ++d)
		for (int i = 1; i <= n; ++i){
			for (int j = 1; j <= m; ++j){
				for (int k = 0; k < 4; k++){
					// the connection between [i,j] and [x,y]
					int x = i + fx[k];
					int y = j + fy[k];

					expr oute = zero,
						 ine = zero;
					getTerm(oute, method, d, i, j, k, block[i][j]);
					getTerm(ine, method, d, x, y, k ^ 1, block[x][y]);
					sat_solver.add(oute == ine);
				}
			}
		}

	//if (aug_limit_t < n * m)
	//	sat_solver.add(total_length <= aug_limit_t);
	//std::cout << "RoutingSatAlgo::Solve() INFO: starting solving model" << std::endl;
	// solving the model

	message results(D, n, m, block);
	if (sat_solver.check()){
		model sat_model = sat_solver.get_model();
		results.set_ok(1);
		// find the path
		for (int i = 1; i <= D; ++i){
            int x = point[0][i].first, y = point[0][i].second, pk = 0;
			for (; ; ){
				results.push(i, x, y);
				if (x == point[1][i].first && y == point[1][i].second)
					break;
				if (x == point[0][i].first && y == point[0][i].second)
					for (int k = 0; k < 4; ++k){
                        std::stringstream st;
						st << sat_model.eval(method[getindx(i, x, y, k)]);
						if (st.str()[0] == 't'){
							x = x + fx[k], y = y + fy[k];
							pk = k;
							break;
						}
					}
				else
					for (int k = 0; k < 6; ++k){
                        std::stringstream st;
						st << sat_model.eval(method[getindx(i, x, y, k)]);
						if (st.str()[0] == 't'){
                            int u1 = mx[k], u2 = my[k];
                            int u = (u1 == (pk ^ 1)) ? u2 : u1;
                            x = x + fx[u], y = y + fy[u];
                            pk = u;
							break;
						}
					}
			}
		}
		//std::cout << "RoutingSatAlgo::Solve() INFO: answer has been calculated successfully: " << std::endl;
		//results.display();
	}
	//printf("find result");
	return results;
}

int calc_num(int x){
	int tot = 0;
	for (int k = x; k; k -= k & (-k))
		++tot;
	return tot;
}

int cmp(int x, int y){
	return calc_num(x) < calc_num(y);
}

message RoutingSatAlgo::SolveMaxPairs(int n_, int m_, int D_, std::vector<std::vector<int> > block){
	std::vector<int> aug_list;
	aug_list.clear();
	for (int i = 0; i < (1 << D_) - 1; ++i)
		aug_list.push_back(i);
	std::sort(aug_list.begin(), aug_list.end(), cmp);
	for (int i = (1 << D_) - 2; i >= 0; --i){
		printf("RoutingSatAlgo::SolveMaxPairs() INFO: Try type %d\n", aug_list[i]);
		message result = SolveSat(n_, m_, D_, block, aug_list[i], n_ * m_);
		if (result.ok())
			return result;
	}
}

message RoutingSatAlgo::SolveMinLength(int n_, int m_, int D_, std::vector<std::vector<int> > block){
	int l = 0, r = n_ * m_;
	while (l + 1 < r){
		int mid = (l + r) >> 1;
        printf("RoutingSatAlgo::SolveMaxPairs() INFO: Try length %d\n", mid);
		message res = SolveSat(n_, m_, D_, block, (1 << D_) - 1, mid);
		if (res.ok())
			r = mid;
		else l = mid;
	}
	return SolveSat(n_, m_, D_, block, (1 << D_) - 1, r);
}

message RoutingSatAlgo::Solve(int n_, int m_, int D_, std::vector<std::vector<int> > block){
	//puts("hear");
    return SolveSat(n_, m_, D_, block, (1 << D_) - 1, n_ * m_);
	if (SolveSat(n_, m_, D_, block, (1 << D_) - 1, n_ * m_).ok() == 0)
		return SolveMaxPairs(n_, m_, D_, block);
	else return SolveMinLength(n_, m_, D_, block);
}
