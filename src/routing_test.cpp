#include <iostream>
#include "routing.h"
#include "routingalgo.h"
#include "message.h"
#include "checker.h"

int main(){
    /*RoutingSatAlgoFlow algo1;
    RoutingSolver s1(&algo1);
    s1.solver("a.in");*/
    RoutingSatAlgoPointPrunePlus algo2;
    RoutingSolver s2(&algo2);
    message ans2 = s2.solver("a");
    std::vector<std::vector<int> > block = s2.getBlock("a");
    ans2.display();
    AnswerChecker checker;
    CheckerMessage m = checker.legal_step_check(block, ans2);
    std::cout << m.res << std::endl;
}