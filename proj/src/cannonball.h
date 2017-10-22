#ifndef _CANNONBALL_H
#define _CANNONBALL_H

#include <stdlib.h>
#include <stdio.h>
/*#include "mouse.h"
#include "kbd.h"*/
//#include "vbe.h"
#include "video_gr.h"
#include "bitmap.h"
#include "obstacle.h"
#include "enemy.h"


#define RELOAD_TIME 5 //5 seconds of reload time until cannon is ready to shoot again
#define MAX_CANNON_BALLS 5
//typedef enum {ALIVE, DEAD} cannonball_states_t;
//typedef enum {NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST} cannonball_orient_t;

typedef enum {LEFT, RIGHT} cannon_side_t;

typedef struct
{
	unsigned int attack;
	unsigned int range;
	unsigned int width;
	unsigned int speed;
	orient_t orientation;
	position_t position;
} cannonball_t;

typedef struct
{
	cannon_side_t side;
	cannonball_t* cannonballs[MAX_CANNON_BALLS];
	unsigned int number_cannonballs; 
	int cannon_cooldown; 
} cannon_t;





//Creates a cannonball
/**
 * @brief cannonball constructor
 *
 * @param width width that is given to the cannonball
 * @param length length that is given to the cannonball
 * @param orientation direction the cannonball will be facing when it spawns
 * @param position position the cannonball will spawn in when the game starts
 * @param bitmap sprite that will represent the cannonball in the game
 * @param cannonball_keys the keys assigned to the control of the cannonball
 *
 * @return a pointer to the cannonball on sucess, NULL otherwise
 */
//cannonball_t* cannonball_constructor(double hp, unsigned int width, unsigned int length, cannonball_orient_t orientation, position_t position, Bitmap* bitmap, cannonball_keys_t cannonball_keys);
cannonball_t* cannonball_constructor(unsigned int attack, unsigned int range, unsigned int width, orient_t orientation, position_t position);

/**
 * @brief deletes the cannonball
 *
 * @param cannonball info regarding the cannonball that will be deleted
 */
void cannonball_delete(cannonball_t* cannonball);


void delete_cannonball_array(cannonball_t** cannonballs, unsigned int index, unsigned int* number_cannonballs);

/**
 * @brief draws the cannonball
 *
 * @param cannonball info regarding the cannonball that will be drawn
 *
 * @return return 0 upon success,  non-zero otherwise
 */
int cannonball_draw(cannonball_t* cannonball, Bitmap* bitmap, char* buffer);


/**
 * @brief moves the cannonball with the mouse
 *
 * @param cannonball info regarding the cannonball that will move
 *
 */
int cannonball_move(cannonball_t* cannonball, enemy_t** enemies, obstacle_t** obstacles, unsigned int obstacles_size, unsigned int* enemies_size, unsigned long* score);


/*---------------CANNON-------------------------*/
cannon_t* cannon_constructor(cannon_side_t side);

/**
*@brief If possible fires the cannon passed as argument
*@param cannon cannon pointer
*@param boat_pos position of boat, where the cannon is
*@param boat_orient orientation of the boat at the moment
*@return 0 on successful shot, 1 if didn't shoot
*/
int cannon_fire(cannon_t* cannon, position_t boat_pos, orient_t boat_orient);

void cannon_move_balls(cannon_t* cannon, enemy_t** enemies, obstacle_t** obstacles, unsigned int obstacles_size, unsigned int* enemies_size, unsigned long* score);

void cannon_decrease_cooldown(cannon_t* cannon);

void cannon_delete(cannon_t* cannon);

void cannons_draw(cannon_t** cannons, Bitmap* bitmap, char* buffer);
#endif
