#include "enemy.h"
#include <time.h>
#define INCREMENT  22
#define OK 0
#define DEAD 1


enemy_t* enemy_constructor(unsigned int hp, unsigned int attack, unsigned int width, unsigned int length, orient_t orientation, position_t position)
{
	enemy_t* enemy = malloc(sizeof(enemy_t));
	if(enemy == NULL)
	{
		printf("enemy::enemy_constructor: error in enemy creation\n;");
		return NULL;
	}

	enemy->hp = hp;
	enemy->attack = attack;
	enemy->width = width;
	enemy->length = length;
	enemy->orientation = orientation;
	enemy->position = position;
	enemy->speed = ENEMY_SPEED;

	enemy->change_orientatiom_flag = TRUE;


	return enemy;
}

int enemy_dmg(enemy_t* enemy, unsigned int dmg)
{ 
		if(enemy->hp - dmg > 0)
		{
			enemy->hp -= dmg;
		}
		else
			return DEAD;

	return 0;
}

void enemy_delete(enemy_t* enemy)
{
	free(enemy);
}

int load_enemy_sprites(Bitmap** enemies)
{
	enemies[0] = loadBitmap("/tmp/resources/snake_N.bmp");

	if(enemies[0] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[1] = loadBitmap("/tmp/resources/snake_NE.bmp");

	if(enemies[1] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[2] = loadBitmap("/tmp/resources/snake_E.bmp");

	if(enemies[2] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[3] = loadBitmap("/tmp/resources/snake_SE.bmp");

	if(enemies[3] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[4] = loadBitmap("/tmp/resources/snake_S.bmp");

	if(enemies[4] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[5] = loadBitmap("/tmp/resources/snake_SW.bmp");

	if(enemies[5] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[6] = loadBitmap("/tmp/resources/snake_W.bmp");

	if(enemies[6] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	enemies[7] = loadBitmap("/tmp/resources/snake_NW.bmp");

	if(enemies[7] == NULL)
	{
		delete_bitmap_array(enemies);
		return 1;
	}

	return 0;
}



int enemy_draw(enemy_t* enemy, Bitmap** bitmaps, char* buffer)
{
	drawBitmap(buffer, bitmaps[enemy->orientation], enemy->position.x, enemy->position.y, ALIGN_CENTER);
	return 0;
}

void enemy_orientation_increment(enemy_t* enemy, int direction)
{
	//positive direction = clockwise rotation
	//negative direction = counter-clockwise direction
	if(direction > 0)
		{
		switch(enemy->orientation)
		{
		case NORTH:
			enemy->orientation = NORTHEAST;
		case NORTHEAST:
			enemy->orientation = EAST;
		case EAST:
			enemy->orientation = SOUTHEAST;
		case SOUTHEAST:
			enemy->orientation = SOUTH;
		case SOUTH:
			enemy->orientation = SOUTHWEST;
		case SOUTHWEST:
			enemy->orientation = WEST;
		case WEST:
			enemy->orientation = NORTHWEST;
		case NORTHWEST:
			enemy->orientation = NORTH;
		default:
			printf("\nError in enemy_orientation_increment\n");
		}
	}
	else
	{
		switch(enemy->orientation)
		{
		case NORTH:
			enemy->orientation = NORTHWEST;
		case NORTHWEST:
			enemy->orientation = WEST;
		case WEST:
			enemy->orientation = SOUTHWEST;
		case SOUTHWEST:
			enemy->orientation = SOUTH;
		case SOUTH:
			enemy->orientation = SOUTHEAST;
		case SOUTHEAST:
			enemy->orientation = EAST;
		case EAST:
			enemy->orientation = NORTHEAST;
		case NORTHEAST:
			enemy->orientation = NORTH;
		default:
			printf("\nError in enemy_orientation_increment\n");
		}

	}
}



int enemy_move(enemy_t* enemy, obstacle_t** obstacles, unsigned int obstacles_size)
{
	unsigned int i;
	
	if(enemy->change_orientatiom_flag)
	{
		enemy->orientation = rand() % 7;
		enemy->change_orientatiom_flag = FALSE;
	}
	

	//printf("\n\nENEMY GENERATED DIRECTION: %d\n\n", enemy->orientation);

switch(enemy->orientation) //MOVEMENT
	{
		case NORTH:

			if( ((int)(enemy->position.y - enemy->speed - enemy->length/2)) < 0)
			{
				return 1;
			}
			else
			{

				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(enemy->position.x + enemy->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && enemy->position.x -  enemy->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(enemy->position.y - enemy->length/3  <= obstacles[i]->position.y + obstacles[i]->length/2 && enemy->position.y - enemy->length/3 >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	//enemy_dmg(enemy, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}
				enemy->position.y -= enemy->speed;
			}
			break;

		case NORTHEAST:
			
			if( ((int)(enemy->position.y - enemy->speed - enemy->length/2)) < 0 || enemy->position.x + enemy->speed + enemy->length/2 > getHorResolution())
				return 1;
			else
			{
				enemy->position.x += enemy->speed;
				enemy->position.y -= enemy->speed;
			}			
			break;

		case EAST:

			if(enemy->position.x + enemy->speed + enemy->length/2 > getHorResolution())
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(enemy->position.x + enemy->length/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && enemy->position.x - enemy->length/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(enemy->position.y - enemy->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && enemy->position.y + enemy->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	//enemy_dmg(enemy, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}
				enemy->position.x += enemy->speed;
			}
			break;

		case SOUTHEAST:

			if(enemy->position.x + enemy->speed + enemy->length/2 > getHorResolution() || enemy->position.y + enemy->speed + enemy->length/2 > getVerResolution())
				return 1;
			else
			{
				enemy->position.x += enemy->speed;
				enemy->position.y += enemy->speed;
			}
			break;

		case SOUTH:

			if(enemy->position.y + enemy->speed + enemy->length > getVerResolution())
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(enemy->position.x + enemy->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && enemy->position.x -  enemy->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(enemy->position.y + enemy->length/3  >= obstacles[i]->position.y - obstacles[i]->length/2 && enemy->position.y - enemy->length/3 <= obstacles[i]->position.y + obstacles[i]->length/2)
					  {
					  	//enemy_dmg(enemy, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}
				enemy->position.y += enemy->speed;
			}
			break;

		case SOUTHWEST:

			if(enemy->position.y + enemy->speed + enemy->length > getVerResolution() || ((int)(enemy->position.x - enemy->speed - enemy->length)) < 0)
				return 1;
			else
			{
				enemy->position.x -= enemy->speed;
				enemy->position.y += enemy->speed;
			}
			break;

		case WEST:

			if( ((int)(enemy->position.x - enemy->speed - enemy->length)) < 0)
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(enemy->position.x - enemy->length/3 >= obstacles[i]->position.x - obstacles[i]->length/3  && enemy->position.x - enemy->length/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(enemy->position.y - enemy->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && enemy->position.y + enemy->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	//enemy_dmg(enemy, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				enemy->position.x -= enemy->speed;
			}
			break;

		case NORTHWEST:
			if( ((int)(enemy->position.x - enemy->speed - enemy->length)) < 0 || ((int)(enemy->position.y - enemy->speed - enemy->length)) < 0)
				return 1;
			else
			{
				enemy->position.x -= enemy->speed;
				enemy->position.y -= enemy->speed;
			}
			break;
	}

	return 0;
}

void delete_enemy_array(enemy_t** enemies, unsigned int index, unsigned int* enemies_size)
{
	enemy_delete(enemies[index]);

	unsigned int i;
	for(i = index; i < *enemies_size - 1; i++)
	{
		enemies[i] = enemies[i + 1];
	}

	*enemies_size = *enemies_size - 1;

	enemies[*enemies_size] = NULL;
}


