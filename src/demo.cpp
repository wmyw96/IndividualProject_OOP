#include "demo.h"
#include <stdio.h>
#include <fstream>

using namespace std;

#define MAIN_W 5
#define MAIN_H 5
#define RIGHT_W 5
#define RIGHT_H 5

void printText(string str, int width){
	printf("+");
	for (int i = 1; i <= width + 4; ++i)
		printf("=");
	printf("+\n");

	printf("|  ");
	int rowcnt = 0;
	for (int i = 0; i < str.size(); ++i, ++rowcnt){
		if (str[i] != '\n')
			printf("%c", str[i]);
		if (i == 0) rowcnt = 1;
		if (rowcnt == width || str[i] == '\n'){
			for (int j = 1; j <= width - rowcnt + 1; ++j)
				printf(" ");
			printf("  |\n");
			if (i + 1 < str.size())
				printf("|  ");
			rowcnt = 0;
		}
	}

	printf("+");
	for (int i = 1; i <= width + 4; ++i)
		printf("=");
	printf("+\n");
}

void StateEntrance::handle(Demo &game){
	system("clear");
	string s1 = "Welcome to the simple demo of routing problem\n";
	printText(s1, 50);
	printf("Enter the width and the height of the board (e.g. '6 8')\n");
	printf("If you want to exit the demo, enter '0 0'\n");

	int x = 0, y = 0;
	scanf("%d %d", &x, &y);
	if (x == 0 || y == 0){
		game.ask_exit = 1;
		return;
	}
	game.width = y;
	game.height = x;
	game.board.resize(game.height + 2);
	for (int i = 0; i <= game.height + 1; ++i){
		game.board[i].resize(game.width + 2);
	}
	game.state = new StateMain();
}

void StateMain::handle(Demo &game){
	system("clear");
	string s1 = "Press 'i' to edit the map\n";
	if (game.error != "") s1 = game.error + s1, game.error = "";
	string s2 = "Press 's' to save the map\n";
	string s3 = "Press 'e' to calulate the map\n";
	string s4 = "Press 'x' to get to the entrance\n";
	printText(s1 + s2 + s3 + s4, 51);
	
	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");

	for (int i = 1; i <= game.height; ++i){
		printf("|  ");
		int used = game.width * 2 - 1;
		int emp = 51 - used; emp /= 2;

		for (int j = 1; j <= emp; ++j)
			printf(" ");
		for (int j = 1; j <= game.width; ++j){
			if (game.board[i][j] < 0)
				printf("#");
			else if (game.board[i][j] == 0)
					printf(".");
				else printf("%d", game.board[i][j]);
			if (j < game.width) printf(" ");
		}

		for (int j = 1; j <= emp; ++j)
			printf(" ");

		printf("  |\n");
	}
	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");

	printf("+");
	for (int i = 1; i <= 51 + 4; ++i)
		printf("=");
	printf("+\n");

	char op;
    scanf(" %c", &op);
	if (op == 'i'){
		game.nx = 1;
		game.ny = 1;
		game.state = new StateEdit();
	}
	else 
		if (op == 's'){
			game.state = new StateSave();
		}
		else 
			if (op == 'e'){
				if (game.filename == ""){
					game.error = "ERROR: Please save the file first\n";
				}
		 		else{
		 			// a simple check
		 			int ok = 1;
		 			for (int t = 1; t <= 9; ++t){
		 				int cnt = 0;
		 				for (int i = 1; i <= game.height; ++i)
		 					for (int j = 1; j <= game.width; ++j)
		 						if (game.board[i][j] == t) cnt++;
		 				if (cnt != 0 && cnt != 2)
		 					ok = 0;
		 			}

		 			if (ok == 0){
		 				game.filename = "";
		 				game.error = "ERROR: Illegal map. and you need to resave the file\n";
		 			}
		 			else{
		 				
		 				game.ans = game.solver.solver(game.filename);

		 				printf("Press 'Enter' key to continue\n");
		 				char ch; scanf("%c", &ch);
		 				scanf("%c", &ch);
		 				game.state = new StateDisplay();
		 			}
		 		}
		 	}
			else 
		 		if (op == 'x'){
		 			game.state = new StateEntrance();
		 		}
}

char printPos(int x){
	if (x < 0) return '#';
	else if (x == 0)
			return '.';
		else return '0' + x;
}

void StateEdit::handle(Demo &game){
	system("clear");
	string s1 = "Press 'x' to view your map\n";
	string s2 = "Press 'w', 's', 'a', 'd' to up/down/left/right\n";
	string s3 = "Press 'o' to and a obstacle\n";
	string s4 = "Press number '1'-'9' to add pairs\n";
	printText(s1 + s2 + s3 + s4, 51);

	printf("|                                                       |\n");
	printf("|     The current thing in the current position is      |\n");
	printf("|                          [%c]                          |\n", printPos(game.board[game.nx][game.ny]));
	game.filename = "";
	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");
	
	for (int i = 1; i <= game.height; ++i){
		printf("|  ");

		int used = game.width * 2 - 1;
		int emp = 51 - used; emp /= 2;

		for (int j = 1; j <= emp; ++j)
			printf(" ");
		for (int j = 1; j <= game.width; ++j){
			if (i == game.nx && j == game.ny)
				printf("*");
			else
				printf("%c", printPos(game.board[i][j]));
			if (j < game.width) printf(" ");
		}

		for (int j = 1; j <= emp; ++j)
			printf(" ");

		printf("  |\n");
	}
	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");

	printf("+");
	for (int i = 1; i <= 51 + 4; ++i)
		printf("=");
	printf("+\n");

	char op;
	scanf(" %c", &op);
	if (op == 'x'){
		game.state = new StateMain();
	}
	else{
		if (op == 'w' && game.nx >= 2) game.nx--;
		if (op == 's' && game.nx < game.height) game.nx++;
		if (op == 'a' && game.ny >= 2) game.ny--;
		if (op == 'd' && game.ny < game.width) game.ny++;
		if (op >= '0' && op <= '9')
			game.board[game.nx][game.ny] = op - '0';
		if (op == 'o')
			game.board[game.nx][game.ny] = -1;
	}
}

void StateSave::handle(Demo &game){
	system("clear");
	string s1 = "Save the file\n";
	if (game.error != "") s1 = game.error + s1, game.error = "";
	printText(s1, 50);
	printf("Enter the file name: \n");

	cin >> game.filename;
	string file = game.filename;

	std::ofstream out(file.c_str());
    if (!out.is_open()){
        game.error = "ERROR: Can't open file '" + file + "'";
        return;
    }

    for (int i = 1; i <= game.height; ++i){
    	for (int j = 1; j <= game.width; ++j)
    		out << printPos(game.board[i][j]);
    	out << std::endl;
    }
    out.close();
	game.state = new StateMain();
}

void detailed_display(int n, int m, vector<vector<int> > board){
	system("clear");
	string s1 = "Detailed information\n";
	printText(s1, 51);

	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");
	
	for (int i = 1; i <= n; ++i){
		printf("|  ");

		int used = m * 2 - 1;
		int emp = 51 - used; emp /= 2;

		for (int j = 1; j <= emp; ++j)
			printf(" ");
		for (int j = 1; j <= m; ++j){
			printf("%c", printPos(board[i][j]));
			if (j < m) printf(" ");
		}

		for (int j = 1; j <= emp; ++j)
			printf(" ");

		printf("  |\n");
	}
	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");

	printf("+");
	for (int i = 1; i <= 51 + 4; ++i)
		printf("=");
	printf("+\n");

}
void StateDisplay::handle(Demo &game){
	message ans = game.ans;
	system("clear");
	string s1 = "Press 'x' to view your map\n";
	string s2 = "Press number '1'-'9' to view detailed information\n";
	printText(s1 + s2, 51);

	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");
	
	for (int i = 1; i <= game.height; ++i){
		printf("|  ");

		int used = game.width * 2 - 1;
		int emp = 51 - used; emp /= 2;

		for (int j = 1; j <= emp; ++j)
			printf(" ");
		for (int j = 1; j <= game.width; ++j){
			printf("%c", printPos(ans.board[i][j]));
			if (j < game.width) printf(" ");
		}

		for (int j = 1; j <= emp; ++j)
			printf(" ");

		printf("  |\n");
	}
	printf("|  ");
	for (int j = 1; j <= 51; ++j)
		printf(" ");
	printf("  |\n");

	printf("+");
	for (int i = 1; i <= 51 + 4; ++i)
		printf("=");
	printf("+\n");

	char op;
	scanf(" %c", &op);
	if (op == 'x'){
		game.state = new StateMain();
	}
	else{
		if (op >= '0' && op <= '9'){
			int num = op - '0';

			vector<vector<int> > display_board;
			display_board.resize(game.height + 1);
			for (int i = 1; i <= game.height; ++i){
				for (int j = 0; j <= game.width; ++j)
					display_board[i].push_back(0);
			}
			for (int i = 0; i < ans.route[num].size(); ++i){
				pair<int, int> pos = ans.route[num][i];
				display_board[pos.first][pos.second] = num;
				detailed_display(game.height, game.width, display_board);
				int pp = 0;
				for (int j = 1; j <= 100000000; ++j) pp = (pp + 3) % 5 + 2;
			}
		}
	}
}

