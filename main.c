#include <SDL/SDL.h>

#include "gameboard.h"

int main(int argc, char **argv)
{
	/*
		Seed the random numbers
	*/

	/* 
		NOTE: time(NULL) returns the current time from the 
		Unix Epoch (Jan 1st 1970) in seconds)
	*/
	srand(time(NULL)); 

	gameboard_init();

	while(gameboard_process_input())
	{
		gameboard_render();
	}


	gameboard_deinit();
	return 0;
}
