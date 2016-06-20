#include "routing.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

std::vector<std::string> RoutingSolver::load(const std::string& file){
	std::ifstream in(file.c_str());
	std::vector<std::string> ret;
	ret.clear();
	if (!in.is_open()){
		printf("RoutingSolver::load() ERROR Can't open file '%s'\n", file.c_str());
		ret.push_back("ERROR");
		return ret;
	}
	std::string str;
	while (in >> str)
		ret.push_back(str);
	in.close();
	return ret;
}

std::vector<std::vector<int> > RoutingSolver::getBlock(const std::string &file){
	std::vector<std::string> board = load(file);
	// error
	
	int n = board.size();
	int m = board[0].size();
	int d = 0;
	
	std::vector<std::vector<int> > block;
	block.clear();
	if (board.size() == 1 && board[0] == "ERROR")
		return block;
	
	block.resize(n + 2);
	for (int i = 0; i <= n + 1; ++i){
		block[i].resize(m + 2);
		for (int j = 0; j <= m + 1; ++j)
			block[i][j] = -1;
	}
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j){
			char c = board[i - 1][j - 1];
			if (c == '#')
				block[i][j] = -1;
			else{
				block[i][j] = 0;
				if (c != '.')
					block[i][j] = c - '0';
				d = std::max(d, c - '0');
			}
		}
	
	return block;
}

message RoutingSolver::solver(const std::string& file){
	std::vector<std::string> board = load(file);
	// error
	if (board.size() == 1 && board[0] == "ERROR")
		return message();

	int n = board.size();
	int m = board[0].size();
	int d = 0;

	std::vector<std::vector<int> > block;
	block.clear();
	
	block.resize(n + 2);
	for (int i = 0; i <= n + 1; ++i){
		block[i].resize(m + 2);
		for (int j = 0; j <= m + 1; ++j)
			block[i][j] = -1;
	}
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j){
			char c = board[i - 1][j - 1];
			if (c == '#')
				block[i][j] = -1;
			else{
				block[i][j] = 0;
				if (c != '.')
					block[i][j] = c - '0';
				d = std::max(d, c - '0');
			}
		}
	
	message t = solve(n, m, d, block);
	return t;
}