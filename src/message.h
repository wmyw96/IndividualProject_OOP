#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <vector>
#include <iostream>

class message{
private:
	int n_, m_, d_;
	int totpair_, totlength_, ok_;
public:
	mutable std::vector<std::vector<int> > board;
	mutable std::vector<std::vector<std::pair<int, int> > > route;
	message(int d, int n, int m, std::vector<std::vector<int> > block);
	message() : n_(0), m_(0), d_(0) {totpair_ = totlength_ = ok_ = 0;}
	message(const message &b){
		n_ = b.n_;
		m_ = b.m_;
		d_ = b.d_;
		totpair_ = b.totpair_;
		totlength_ = b.totlength_;
		ok_ = b.ok_;
		board = b.board;
		route = b.route;
	}
	int d() const{
		return d_;
	}
	int n() const{
		return n_;
	}
	int m() const{
		return m_;
	}
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
	/*void set_totlength(int v){
		totlength_ = v;
	}*/
	void set_ok(int v){
		ok_ = v;
	}
	void push(int d, int x, int y){
		route[d].push_back(std::make_pair(x, y));
		totlength_ ++;
		board[x][y] = d;
	}
	void display() const;
	message& operator = (const message &b){
		if (this == &b)
			return *this;
		n_ = b.n_;
		m_ = b.m_;
		d_ = b.d_;
		totpair_ = b.totpair_;
		totlength_ = b.totlength_;
		ok_ = b.ok_;
		board = b.board;
		route = b.route;
		return *this;
	}
};

#endif //MESSAGE_H_