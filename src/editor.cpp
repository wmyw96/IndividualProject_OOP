#include "demo.h"

using namespace std;

int main(){
	Demo player;
	while (1){
		int is_exit = player.handle();
		if (is_exit)
			break;
	}
}
