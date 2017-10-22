#ifndef _ENEMY_H
#define _ENEMY_H

#include <stdlib.h>
#include <stdio.h>
/*#include "mouse.h"
#include "kbd.h"*/
//#include "vbe.h"
#include "video_gr.h"
#include "bitmap.h"
#include "utilities.h"
#include "obstacle.h"

#define ENEMY_HP 100
#define ENEMY_ATK 20
#define ENEMY_SPEED 3
#define ENEMY_WIDTH 20
#define ENEMY_LENGTH 20

typedef struct
{
	unsigned int  hp;
	unsigned int attack;
	unsigned int width;
	unsigned int length;
	double speed;
	orient_t orientation;
	char change_orientatiom_flag;
	position_t position;
} enemy_t;

//Creates a enemy
/**
 * @brief enemy constructor
 *
 * @param width width that is given to the enemy
 * @param length length that is given to the enemy
 * @param orientation direction the enemy will be facing when it spawns
 * @param position position the enemy will spawn in when the game starts
 *
 * @return a pointer to the enemy on sucess, NULL otherwise
 */

enemy_t* enemy_constructor(unsigned int  hp, unsigned int attack, unsigned int width, unsigned int length, orient_t orientation, position_t position);

/**
 * @brief deletes the enemy
 *
 * @param enemy info regarding the enemy that will be deleted
 */
void enemy_delete(enemy_t* enemy);


int enemy_dmg(enemy_t* enemy, unsigned int dmg);

int load_enemy_sprites(Bitmap** enemies);

/**
 * @brief draws the enemy
 *
 * @param enemy info regarding the enemy that will be drawn
 *
 * @return return 0 upon success,  non-zero otherwise
 */
int enemy_draw(enemy_t* enemy, Bitmap** bitmaps, char* buffer);

int enemy_draw_orientation(enemy_t* enemy, char* buffer);

void enemy_orientation_increment(enemy_t* enemy, int direction);

/**
 * @brief moves the enemy with the mouse
 *
 * @param enemy info regarding the enemy that will move
 *
 */
int enemy_move(enemy_t* enemy, obstacle_t** obstacles, unsigned int obstacles_size);



void delete_enemy_array(enemy_t** enemies, unsigned int index, unsigned int* enemies_size);




#endif
