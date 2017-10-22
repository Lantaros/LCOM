#ifndef __GAME_H
#define __GAME_H

#include <minix/sysutil.h>
#include <minix/drivers.h>

//Our Includes
#include "mouse.h"
#include "video.h"
#include "video_gr.h"
#include "boat.h"
#include "obstacle.h"
#include "enemy.h"

#define BITMAP_MISSING 3
#define MAX_OBSTACLES 5
#define MAX_ENEMIES 50

typedef enum {MENU, GAME, END_GAME} game_states_t; //Not Needed


typedef struct
{
	char* video_mem;
	char* background_buffer;
	char* buffer_video2;
	unsigned int video_mem_size;

	boat_t* boat;
	unsigned long score;
	game_states_t state;

	enemy_t* enemies[MAX_ENEMIES];
	unsigned int enemies_size;
	obstacle_t* obstacles[MAX_OBSTACLES];
	unsigned int obstacles_size;

	unsigned long scancode;
	position_t mouse_pos;
	mouse_t* mouse;
	char second_passed_flag;

	//BMP
	Bitmap* hp[2];
	Bitmap* numbers[10];
	
		//Menu
		Bitmap* title_bmp;
		Bitmap* button_bmp;
		Bitmap*  mouse_bmp;
		//END
		Bitmap* high_score_bmp;
	
	//Enemy
	Bitmap* enemy_bmp[8];
	//Cannonball
	Bitmap* cannonball_bmp;
	//ROCK (Obstacle)
	Bitmap* obstacle_bmp;
}game_t;

void set_enemies_change_orient_flag(game_t* game, char bool);

int mouse_update_position(mouse_t* mouse, position_t* position);

int game_start(game_t* game);

void game_menu(game_t* game);

void game_score(game_t* game);

int game_update(game_t* game);

void game_end(game_t* game);

void print_hp(Bitmap** hp, Bitmap** numbers, char* buffer , unsigned int number, unsigned int x, unsigned int y);

void draw_number(Bitmap** array, char* buffer , unsigned int number, unsigned int x, unsigned int y);

int load_numbers(game_t* game);

void delete_numbers(Bitmap** numbers);

int enemy_rand_constructor(position_t boat_pos, obstacle_t** obstacles, unsigned int obstacles_size, enemy_t** enemies, unsigned int* enemies_size, cannon_t** cannons);

void enemy_generator(game_t* game);

#endif
