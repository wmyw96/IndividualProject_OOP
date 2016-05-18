#include <iostream>
#include <z3++.h>
using namespace std;
using namespace z3;

int main() {
    context c;
    optimize opt(c);
    params p(c);
    p.set("priority",c.str_symbol("pareto"));
    opt.set(p);
    expr x = c.int_const("x");
    expr y = c.int_const("y");
    opt.add(10 >= x && x >= 0);
    opt.add(10 >= y && y >= 0);
    opt.add(x + y <= 11);
    opt.add(y - x >= 5);
    optimize::handle h1 = opt.maximize(x);
    //optimize::handle h2 = opt.maximize(y);
    while (true) {
        if (sat == opt.check()) {
            std::cout << opt.lower(h1) << std::endl;
            model m = opt.get_model();
            cout << m.eval(y) << endl;
            //std::cout << x << ": " << opt.lower(h1) << " " << y << ": " << opt.lower(h2) << "\n";
        }
        else {
            break;
        }
    }
}
