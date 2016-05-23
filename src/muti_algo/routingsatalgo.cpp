#include "routingalgo.h"

#include <cstdio>
#include <algorithm>

using namespace z3;

const int fx[4] = {0, 0, 1, -1},
fy[4] = {1, -1, 0, 0};
const int mx[6] = {0, 0, 0, 1, 1, 2},
my[6] = {1, 2, 3, 2, 3, 3};

char RoutingSatAlgo::GetValue(model &sat_model, const expr &t){
    std::stringstream st;
    st << sat_model.eval(t);
    return st.str()[0];
}

expr RoutingSatAlgo::BoolToInt(const expr &t){
    return to_expr(sat_context, Z3_mk_ite(sat_context, t, one, zero));
}

void RoutingSatAlgo::GetStartEndPoints(std::vector<std::pair<int, int> > &start_point,
                                       std::vector<std::pair<int, int> > &end_point,
                                       std::vector<std::vector<int> > block){
    std::vector<std::pair<int, int> > point[2];
    point[0].resize(D + 1);
    point[1].resize(D + 1);
    
    int *cnt = new int[D + 1];
    for (int i = 1; i <= D; ++i) cnt[i] = 0;
    
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            if (block[i][j] > 0)
                point[cnt[block[i][j]]++][block[i][j]] = std::make_pair(i, j);
    delete []cnt;
    
    start_point = point[0];
    end_point = point[1];
}

message RoutingSatAlgo::GetAns(const expr &total_pairs,
                               const expr &total_length,
                               const expr_vector &method,
                               optimize &sat_solver,
                               std::vector<std::vector<int> > block,
                               std::vector<std::pair<int, int> > start_point,
                               std::vector<std::pair<int, int> > end_point,
                               int aug){
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
            if (d < D)
                results.set_totpair(d);
            else return SolveSat(n, m, D, block, 1);
        }
        
        model sat_model = sat_solver.get_model();
        
        // find the path
        FindPath(results, sat_model, method, start_point, end_point);
    }
    return results;
}