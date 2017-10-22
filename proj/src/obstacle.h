#ifndef _obstacle_H
#define _obstacle_H

#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"
#include "utilities.h"

#define ROCK_LENGTH 49
#define ROCK_DMG 10

typedef struct
{
	unsigned int colision_dmg;
	unsigned int width;
	unsigned int length;
	position_t position;
}obstacle_t;



//Creates a obstacle
/**
 * @brief obstacle constructor
 *
 * @param width width that is given to the obstacle
 * @param length length that is given to the obstacle
 * @param position position the obstacle will spawn in when the game starts
 * @param bitmap sprite that will represent the obstacle in the game
 *
 * @return a pointer to the obstacle on sucess, NULL otherwise
 */
obstacle_t* obstacle_constructor(unsigned int colision_dmg, unsigned int width, unsigned int length, position_t position);


/**
 * @brief deletes the obstacle
 *
 * @param obstacle info regarding the obstacle that will be deleted
 */
void obstacle_delete(obstacle_t* obstacle);


/**
 * @brief draws the obstacle
 *
 * @param obstacle info regarding the obstacle that will be drawn
 *
 * @return return 0 upon success,  non-zero otherwise
 */
int obstacle_draw(obstacle_t* obstacle, Bitmap* bitmap, char* buffer);



/**
*@brief generates all obstacle in a game
*@param obstacles array where obstacles are stored
*@param obstacles_size current number of obstacles in game
*/
void generate_obstacles(obstacle_t** obstacles, unsigned int* obstacles_size);




#endif
