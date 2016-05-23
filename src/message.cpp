#include "message.h"
#include <cmath>

int getmin(int x, int y){
    return x < y ? x : y;
}

message::message(int d, int n, int m, std::vector<std::vector<int> > block) : d_(d), n_(n), m_(m) {
	board.resize(n + 1);
	for (int i = 1; i <= n; ++i)
		board[i].resize(m + 1);
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j)
			board[i][j] = getmin(block[i][j], 0);
	route.resize(d + 1);
    ok_ = 0;
}

void message::display() const{
	for (int i = 1; i <= n_; ++i){
		for (int j = 1; j <= m_; ++j)
			if (board[i][j] > 0)
                std::cout << board[i][j];
            else std::cout << (board[i][j] < 0 ? '*' : '.');
        std::cout << std::endl;
	}
}