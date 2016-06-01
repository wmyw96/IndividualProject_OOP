// This program is contributed to get the average of
// all tests (based on the report.out)

#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]){
	if (argc != 2){
		puts("Format: [exe] [number of different N]");
		return 0;
	}

	int num = atoi(argv[1]);
	if (num > 10){
		puts("Number of N is too large");
		return 0;
	}

	double av[111], average, max, min, sd;
	int n, m;
	memset(av, 0, sizeof av);
	freopen("report.out", "r", stdin);
	for (int i = 1; i <= num * 9; ++i){
		scanf("(%d,%d) %lf%lf%lf%lf\n", &n, &m, &average, &max, &min, &sd);
		av[n] += average;
		printf("%d %d\n", n, m);
	}
	for (int i = 1; i <= 12; ++i)
		std::cout << av[i] / m << std::endl;
}