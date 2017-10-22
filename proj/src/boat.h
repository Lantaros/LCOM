#ifndef _BOAT_H
#define _BOAT_H

#include <stdlib.h>
#include <stdio.h>
/*#include "mouse.h"
#include "kbd.h"*/
//#include "vbe.h"
#include "video_gr.h"
#include "bitmap.h"
#include "utilities.h"

#include "enemy.h"
#include "obstacle.h"
#include "cannonball.h"

#include "mouse.h"


#define BOAT_LENGTH 188
#define BOAT_WIDTH 74
#define CANNONS_NUMBER 2

typedef enum {ALIVE, DEAD} boat_states_t;

typedef struct
{
	unsigned int hp;
	boat_states_t state;
	unsigned int width;
	unsigned int length;
	int speed;
	orient_t orientation;
	position_t position;
	Bitmap* bitmaps[8]; //8 directions
	char dmg_flag;
	cannon_t* cannons[CANNONS_NUMBER];
} boat_t;



/**
 * @brief boat constructor
 *
 * @param health of the boat, or amount of damage it can take until it dies
 * @param width width that is given to the boat
 * @param length length that is given to the boat
 * @param orientation direction the boat is currently facing (in this case, when it spawns)
 * @param position position the boat will spawn in when the game starts
 * 
 * @return a pointer to the boat on sucess, NULL otherwise
 */
boat_t* boat_constructor(unsigned int hp, unsigned int width, unsigned int length, orient_t orientation, position_t position);


/**
 * @brief deletes the boat
 *
 * @param boat info regarding the boat that will be deleted
 */
void boat_delete(boat_t* boat);


/**
 * @brief draws the boat
 *
 * @param boat info regarding the boat that will be drawn
 * @param buffer video buffer where the boat will be drawn to
 *
 * @return return 0 upon success,  non-zero otherwise
 */
int boat_draw(boat_t* boat, char* buffer);


/**
 * @brief moves the boat with the mouse
 *
 * @param boat info regarding the boat that will move
 * @param scancode received keyboard scancode
 * @param obstacles array with in-game obstacles for colision calculating purposes
 * @param obstacles_size number of elements in the aforementioned array (number of current in-game obstacles)
 * @param enemies array with in-game enemies for colision calculating purposes
 * @param enemies_size number of elements in the aforementioned array (number of current in-game enemies)
 *
 * @return returns 0 upon success, non-zero otherwise
 */
int boat_move(boat_t* boat, const unsigned long* scancode, obstacle_t** obstacles, unsigned int obstacles_size, enemy_t** enemies, unsigned int* enemies_size);


/**
 * @brief loads the bitmap sprite into the boat
 *
 * @param boat info regarding the boat where the bitmaps will be loaded to
 *
 * @return return 0 upon success, non-zero otherwise
 */
int load_boat_sprites(boat_t* boat);


/**
 * @brief increments the current boat direction
 *
 * @param boat info regarding the boat whose direction will be incremented
 * @param direction direction to which the boat will rotate (>0 = clockwise, <0 = counter-clockwise)
 */
void boat_orientation_increment(boat_t* boat, int direction);


/**
 * @brief updates the current boat health and checks if it has died
 *
 * @param boat info regarding the boat
 * @param dmg damage the boat receives; if the boat health is set to <= 0, the state of the boat is updated to "DEAD"
 */
void boat_dmg(boat_t* boat, unsigned int dmg);

int boat_fire_cannons(boat_t* boat, mouse_t* mouse);

void boat_decrease_cannon_cooldown(boat_t* boat);

#endif
