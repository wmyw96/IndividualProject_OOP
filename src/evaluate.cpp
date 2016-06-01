#include "evaluate.h"

#include <vector>
#include <fstream>
#include <ratio>
#include <ctime>
#include <chrono>
#include <cstdlib>

void RandomDataGenerator::generate(int seed, int n, int m, int d, double obstacle_rate, const std::string& filename){
	srand(seed);

	std::vector<std::vector<int> > block;
	block.clear();
	block.resize(n + 1);
	for (int i = 1; i <= n; ++i){
		block[i].resize(m + 1);
		for (int j = 1; j <= m; ++j)
			block[i][j] = 0;
	}

	int left_empty = n * m - 2 * d;
	if (left_empty < 0){
		std::cout << "RandomDataGenerator::generate() ERROR: Don't have enough grids" << std::endl;
		return;
	}

	// add pairs
	for (int i = 1; i <= d; ++i)
		for (int k = 0; k < 2; ++k){
			int x = rand() % n + 1;
			int y = rand() % m + 1;
			while (block[x][y]){
				x = rand() % n + 1;
				y = rand() % m + 1;
			}
			block[x][y] = i;
		}

	// add obstacles
	if (left_empty > 0)
		for (int i = 1; i <= n; ++i)
			for (int j = 1; j <= m; ++j)
				if (block[i][j] == 0){
					int v = rand() % left_empty + 1;
					if (double(v) / left_empty <= obstacle_rate)
						block[i][j] = -1;
				}

	// output
	std::ofstream out(filename.c_str());
	if (!out.is_open()){
		std::cout << "RandomDataGenerator::generate() ERROR: File " << filename << " can't open" << std::endl;
	}
	for (int i = 1; i <= n; ++i){
		for (int j = 1; j <= m; ++j){
			if (block[i][j] > 0)
				out << block[i][j];
			if (block[i][j] == 0)
				out << '.';
			if (block[i][j] < 0)
				out << '#';
		}
		out << std::endl;
	}
}

void Evaluation::Execute(int id){
	int seed = 0;
	AnswerChecker checker;
	for (int i = 1; i <= numTest; ++i){
		char filename[] = "this_is_a_placeholder_this_is_a_placeholder_this_is_a_placeholder";
		sprintf(filename, "test_%d_%d.in", id, i);
		gen->generate(seed, n, m, d, obstacle_rate, std::string(filename));
		
		// get ans_a
		using namespace std::chrono;
		steady_clock::time_point clock_begin_a = steady_clock::now();
		message ans_a = solver_a.solver(filename);
		steady_clock::time_point clock_end_a = steady_clock::now();
		duration<double> time_span_a = duration_cast<duration<double> >(clock_end_a - clock_begin_a);
		rec_a.push_back((double)time_span_a.count());
		std::cout << "Evaluation::Execute() INFO: Time span of algorithm a: "
				  <<  (double)time_span_a.count() << "s" << std::endl;

		// get ans_b
		steady_clock::time_point clock_begin_b = steady_clock::now();
		message ans_b = solver_b.solver(filename);
		steady_clock::time_point clock_end_b = steady_clock::now();
		duration<double> time_span_b = duration_cast<duration<double> >(clock_end_b - clock_begin_b);
		rec_b.push_back((double)time_span_b.count());
		std::cout << "Evaluation::Execute() INFO: Time span of algorithm b: "
				  <<  (double)time_span_b.count() << "s" << std::endl;

		// get map
		std::vector<std::vector<int> > block = solver_a.getBlock(filename);

		// check legality of a
		CheckerMessage ma = checker.legal_check(block, ans_a);
		if (!ma.ok){
			std::cout << "Evaluation::Execute() INFO: algorithm a puts an illegal answer in group["
						<< id << "] testcase [" << i << "]" << std::endl;
			std::cout << "Details: " << ma.res << std::endl;
			assert(false);
		}
		// check legality of b
		CheckerMessage mb = checker.legal_check(block, ans_b);
		if (!mb.ok){
			std::cout << "Evaluation::Execute() INFO: algorithm b puts an illegal answer in group["
						<< id << "] testcase [" << i << "]" << std::endl;
			std::cout << "Details: " << mb.res << std::endl;
			assert(false);
		}

		// check the detailed ans
		CheckerMessage m_ab = checker.cmp_check(ans_a, ans_b);
		if (!m_ab.ok){
			std::cout << "Evaluation::Execute() INFO: algorithm a has a worse answer compared with algorithm b in group["
						<< id << "] testcase [" << i << "]" << std::endl;
			std::cout << "Details: " << m_ab.res << std::endl;
			assert(false);			
		}
		CheckerMessage m_ba = checker.cmp_check(ans_b, ans_a);
		if (!m_ba.ok){
			std::cout << "Evaluation::Execute() INFO: algorithm b has a worse answer compared with algorithm a in group["
						<< id << "] testcase [" << i << "]" << std::endl;
			std::cout << "Details: " << m_ba.res << std::endl;
			assert(false);
		}

		std::cout << "Evaluation::Execute() INFO: OK in group[" << id << "] testcase [" << i << "]" << std::endl;
		seed = (seed * 26 + 3) % 1000000007;
	}
}
