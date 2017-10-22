//Library Includes
#include <stdio.h>
#include <minix/sysutil.h>
//Our
#include "game.h"


int main()
{
	sef_startup();
	//sys_enable_iop(SELF);

	//printf("ALL Boundries\nL: %d\nR: %dU: %d\nL: %d\n", LEFT_BOUNDRY, RIGHT_BOUNDRY, UPPER_BOUNDRY, LOWER_BOUNDRY);
	game_t* game = malloc(sizeof(game_t));
	//printf("Game address: %x\n", game);

	game->video_mem = vg_init(VIDEO_MODE_16BITS, &game->video_mem_size);

	game_start(game);

	game_score(game);

	game_end(game);

	vg_exit();


	return 0;
}
