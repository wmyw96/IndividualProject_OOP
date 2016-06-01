#ifndef EVALUTAE_H_
#define EVALUTAE_H_

#include "message.h"
#include "checker.h"
#include "routing.h"
#include "routingalgo.h"
#include <cstring>
#include <cmath>

class DataGenerator{
public:
	DataGenerator() {}
	virtual ~DataGenerator() {}
	virtual void generate(int seed, int n, int m, int d, double obstacle_rate, const std::string& filename) = 0;
};

class RandomDataGenerator : public DataGenerator{
public:
	RandomDataGenerator() {}
	~RandomDataGenerator() {}
	void generate(int seed, int n, int m, int d, double obstacle_rate, const std::string& filename);
};

class Time{
	std::vector<double> rec_list_;
	int tot_;
	double sum_;
	double max_time_;
	double min_time_;
	double square_sum_;
public:
	Time() {
		rec_list_.clear();
		tot_ = 0;
		sum_ = max_time_ = square_sum_ = 0;
		min_time_ = 1e9;
	}
	~Time() {}
	void push_back(double rec_value){
		rec_list_.push_back(rec_value);
		tot_ ++;
		sum_ = sum_ + rec_value;
		if (rec_value > max_time_)
			max_time_ = rec_value;
		if (rec_value < min_time_)
			min_time_ = rec_value;
		square_sum_ = square_sum_ + rec_value * rec_value;
	}
	double max_time() const{
		return max_time_;
	}
	double min_time() const{
		return min_time_;
	}
	double sum() const{
		return sum_;
	}
	double average() const{
		return sum_ / tot_;
	}
	double standard_deviation() const{
		return std::sqrt((square_sum_ - sum_ / tot_ * sum_) / tot_);
	}
};

class Evaluation{
	DataGenerator *gen;
	RoutingSolver solver_a, solver_b;
	int n, m, d, numTest;
	double obstacle_rate;
public:
	Time rec_a, rec_b;
	Evaluation(DataGenerator *gen1, RoutingAlgo *algoa, RoutingAlgo *algob, 
				int _n, int _m, int _d, double _ob_r, 
				int _numTest) : rec_a(), rec_b(), solver_a(algoa), solver_b(algob), n(_n), m(_m), d(_d), obstacle_rate(_ob_r), numTest(_numTest){
		std::cout << "Evaluation Board Established" << std::endl;
		gen = gen1;
	}
	void Execute(int id);
	~Evaluation() {
		std::cout << "Evaluation Ended" << std::endl;
	}
};

#endif // EVALUTAE_H_