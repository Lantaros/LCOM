#include "obstacle.h"
#include <time.h>


#define MIN_GAP 10

obstacle_t* obstacle_constructor(unsigned int colision_dmg, unsigned int width, unsigned int length, position_t position)
{
	obstacle_t* obstacle = malloc(sizeof(obstacle_t));
	if(obstacle == NULL)
	{
		printf("obstacle::obstacle_constructor: error in obstacle creation\n;");
		return NULL;
	}

	obstacle->colision_dmg = colision_dmg;
	obstacle->width = width;
	obstacle->length = length;
	obstacle->position = position;

	return obstacle;
}


void obstacle_delete(obstacle_t* obstacle)
{
	free(obstacle);
}



int obstacle_draw(obstacle_t* obstacle, Bitmap* bitmap, char* buffer)
{
	drawBitmap(buffer, bitmap, obstacle->position.x, obstacle->position.y, ALIGN_CENTER);
	return 0;
}

void generate_obstacles(obstacle_t** obstacles, unsigned int* obstacles_size)
{
	unsigned int i;

	srand (time(NULL));
	
	for(i = *obstacles_size; i < 5; i++) 
	{
		position_t position = {X_ORIGIN + ROCK_LENGTH/2 , Y_ORIGIN + ROCK_LENGTH/2};

		position.x +=  (rand() + ROCK_LENGTH/2 + MIN_GAP) % (X_LIMIT - ROCK_LENGTH/2); //X/Y _LIMIT - ROCK_LENGTH/2 -> This no half of a rock gets outside of the screen
		
		position.y +=  (rand() + ROCK_LENGTH/2 + MIN_GAP) % (Y_LIMIT - ROCK_LENGTH/2); //X/Y _LIMIT - ROCK_LENGTH/2 -> This no half of a rock gets outside of the screen
		

		obstacles[i] = obstacle_constructor(ROCK_DMG, ROCK_LENGTH, ROCK_LENGTH, position);

		*obstacles_size = *obstacles_size + 1;
	}
}








