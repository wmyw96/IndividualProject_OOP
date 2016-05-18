#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <vector>
#include <iostream>

class message{
private:
	const int n_, m_, d_;
	int totpair_, totlength_, ok_;
	std::vector<std::vector<int> > board;
	std::vector<std::vector<std::pair<int, int> > > route;
public:
	message(int d, int n, int m, std::vector<std::vector<int> > block);
	int ok() const{
		return ok_;
	}
	int totpair() const{
		return totpair_;
	}
	int totlength() const{
		return totlength_;
	}
	void set_totpair(int v){
		totpair_ = v;
	}
	void set_totlength(int v){
		totlength_ = v;
	}
	void set_ok(int v){
		ok_ = v;
	}
	void push(int d, int x, int y){
        route[d].push_back(std::make_pair(x, y));
		board[x][y] = d;
	}
	void display();
};

#endif //MESSAGE_H_