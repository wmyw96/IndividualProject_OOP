#include <iostream>
#include <cstdio>
using namespace std;

int main(){
	double av[111], average, max, min, sd;
	int n, m;
	memset(av, 0, sizeof av);
	freopen("report.out", "r", stdin);
	for (int i = 1; i <= 3 * 9; ++i){
		scanf("(%d,%d) %lf%lf%lf%lf\n", &n, &m, &average, &max, &min, &sd);
		av[n] += average;
		printf("%d %d\n", n, m);
	}
	for (int i = 1; i <= 12; ++i)
		std::cout << av[i] / m << std::endl;
}