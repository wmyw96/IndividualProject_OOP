#include "message.h"
#include <cmath>

#include <cstdio>

int getmin(int x, int y){
    return x < y ? x : y;
}

message::message(int d, int n, int m, std::vector<std::vector<int> > block) : d_(d), n_(n), m_(m) {
	board.resize(n + 1);
	totpair_ = totlength_ = ok_ = 0;
	for (int i = 1; i <= n; ++i)
		board[i].resize(m + 1);
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j)
			board[i][j] = getmin(block[i][j], 0);
	route.resize(d + 1);
    ok_ = 0;
}

void message::display() const{
    if (totpair_ < d_)
        std::cout << "Maximize the total connected pairs: " << totpair_ << " pairs" << std::endl;
    else
        std::cout << "Minimize the total length: " << totlength_ << " grieds" << std::endl;
	for (int i = 1; i <= n_; ++i){
		for (int j = 1; j <= m_; ++j)
			if (board[i][j] > 0)
                std::cout << board[i][j];
            else std::cout << (board[i][j] < 0 ? '*' : '.');
        std::cout << std::endl;
	}
	
	for (int i = 1; i <= d_; ++i){
		for (int j = 0; j < route[i].size(); ++j)
			printf("(%d, %d) ", route[i][j].first, route[i][j].second);
		printf("\n");
	}
}