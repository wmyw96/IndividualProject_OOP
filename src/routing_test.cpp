#include <iostream>
#include "routing.h"
#include "routingalgo.h"

using namespace std;

int main(){
    RoutingSatAlgoFlow algo1;
    RoutingSolver s1(&algo1);
    s1.solver("a.in");
    RoutingSatAlgoPointPrune algo2;
    RoutingSolver s2(&algo2);
    s2.solver("a.in");
}