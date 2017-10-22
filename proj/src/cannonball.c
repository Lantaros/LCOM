#include "cannonball.h"
#define CANNON_BALL_SPEED  5
#define READY 0
#define OUT_OF_BOUNDRIES 1
#define DELETE_BALL 1
#define CANNON_BALL_ATK 50
#define CANNON_BALL_RANGE 400
#define CANNON_BALL_WIDTH 20
#define GAP 5
#define BALL_GAP (BOAT_WIDTH + CANNON_BALL_WIDTH + GAP)

#define KILL_ENEMY_SCORE 100

#define OK 0
//I' ll not include a header (boat.h), just for 2 macros
#define BOAT_WIDTH 74
#define CANNONS_NUMBER 2


cannonball_t* cannonball_constructor(unsigned int attack, unsigned int range, unsigned int width, orient_t orientation, position_t position)
{
	cannonball_t* cannonball = malloc(sizeof(cannonball_t));
	if(cannonball == NULL)
	{
		printf("cannonball::cannonball_constructor: error in cannonball creation\n;");
		return NULL;
	}

	cannonball->attack = attack;
	cannonball->range = range;
	cannonball->width = width;
	cannonball->orientation = orientation;
	cannonball->position = position;
	cannonball->speed = CANNON_BALL_SPEED;

	
	return cannonball;
}


void cannonball_delete(cannonball_t* cannonball)
{
	free(cannonball);
}



int cannonball_draw(cannonball_t* cannonball, Bitmap* bitmap, char* buffer)
{
	drawBitmap(buffer, bitmap, cannonball->position.x, cannonball->position.y, ALIGN_CENTER);
	return 0;
}



int cannonball_move(cannonball_t* cannonball, enemy_t** enemies, obstacle_t** obstacles, unsigned int obstacles_size, unsigned int* enemies_size, unsigned long* score)
{
	unsigned int i;
	switch(cannonball->orientation)
		{
			case NORTH:

				if( ((int)(cannonball->position.y - cannonball->width/2)) < 0)
				{
					return OUT_OF_BOUNDRIES;
				}
				else
				{
					//Colisions with Objects
					for(i = 0; i < obstacles_size; i++)
					{
				  /*x*/ if(cannonball->position.x + cannonball->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && cannonball->position.x -  cannonball->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
						{
					/*y*/ if(cannonball->position.y - cannonball->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && cannonball->position.y - cannonball->width/2 >= obstacles[i]->position.y - obstacles[i]->length/2)
						  {
						  	return DELETE_BALL;
						  }
						}
					}

					//Colisions with Enemies
					for(i = 0; i < *enemies_size; i++)
					{
				 	  if(cannonball->position.x + cannonball->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && cannonball->position.x -  cannonball->width/2 <= enemies[i]->position.x + enemies[i]->length/2)
						{
						 if(cannonball->position.y - cannonball->width/2  <= enemies[i]->position.y + enemies[i]->length/2 && cannonball->position.y - cannonball->width/2 >= enemies[i]->position.y - enemies[i]->length/2)
						  {
						  	if(enemy_dmg(enemies[i], cannonball->attack) != OK)
						  	{
						    	delete_enemy_array(enemies, i, enemies_size);
						    	*score += KILL_ENEMY_SCORE;
						  	}
						  	
						  	return DELETE_BALL;
						  }
						}
					}
					

					cannonball->position.y -= cannonball->speed;
				}
				break;

			case NORTHEAST:
				
				if( ((int)(cannonball->position.y - cannonball->width/2)) < 0 || cannonball->position.x  + cannonball->width/2 > getHorResolution())
					return OUT_OF_BOUNDRIES;
				else
				{

					//TODO Collisions DIAGONALLY

					cannonball->position.x += cannonball->speed;
					cannonball->position.y -= cannonball->speed;
				}			
				break;

			case EAST:

				if(cannonball->position.x  + cannonball->width/2 > getHorResolution())
					return OUT_OF_BOUNDRIES;
				else
				{
					//Colisions with Objects
					for(i = 0; i < obstacles_size; i++)
					{
				  /*x*/ if(cannonball->position.x + cannonball->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && cannonball->position.x - cannonball->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
						{
					/*y*/ if(cannonball->position.y - cannonball->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && cannonball->position.y + cannonball->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2)
						  {
						  	return DELETE_BALL;
						  }
						}
					}

					//Colisions with Enemies
					for(i = 0; i < *enemies_size; i++)
					{
				  /*x*/ if(cannonball->position.x + cannonball->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && cannonball->position.x - cannonball->width/2 <= enemies[i]->position.x + enemies[i]->length/2)
						{
					/*y*/ if(cannonball->position.y - cannonball->width/2  <= enemies[i]->position.y + enemies[i]->length/2 && cannonball->position.y + cannonball->width/2  >= enemies[i]->position.y - enemies[i]->length/2)
						 {
						  	if(enemy_dmg(enemies[i], cannonball->attack) != OK)
						  	{
						    	delete_enemy_array(enemies, i, enemies_size);
						    	*score += KILL_ENEMY_SCORE;
						  	}
						  	
						  	return DELETE_BALL;
						  }
						}
					}

					cannonball->position.x += cannonball->speed;
				}
				break;

			case SOUTHEAST:

				if(cannonball->position.x + cannonball->width/2 > getHorResolution() || cannonball->position.y  + cannonball->width/2 > getVerResolution())
					return OUT_OF_BOUNDRIES;
				else
				{
					

					cannonball->position.x += cannonball->speed;
					cannonball->position.y += cannonball->speed;
				}
				break;

			case SOUTH:

				if(cannonball->position.y + cannonball->width/2 > getVerResolution())
					return 1;
				else
				{
					//Colisions with Objects
					for(i = 0; i < obstacles_size; i++)
					{
				  /*x*/ if(cannonball->position.x + cannonball->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && cannonball->position.x -  cannonball->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
						{
					/*y*/ if(cannonball->position.y + cannonball->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2 && cannonball->position.y - cannonball->width/2 <= obstacles[i]->position.y + obstacles[i]->length/2)
						  {
						  	return DELETE_BALL;
						  }
						}
					}

					//Colisions with Enemies
					for(i = 0; i < *enemies_size; i++)
					{
				  /*x*/ if(cannonball->position.x + cannonball->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && cannonball->position.x -  cannonball->width/2 <= enemies[i]->position.x + enemies[i]->length/2)
						{
					/*y*/ if(cannonball->position.y + cannonball->width/2  >= enemies[i]->position.y - enemies[i]->length/2 && cannonball->position.y - cannonball->width/2 <= enemies[i]->position.y + enemies[i]->length/2)
						  {
						  	if(enemy_dmg(enemies[i], cannonball->attack) != OK)
						  	{
						    	delete_enemy_array(enemies, i, enemies_size);
						    	*score += KILL_ENEMY_SCORE;
						  	}
						  	
						  	return DELETE_BALL;
						  }
						}
					}

					cannonball->position.y += cannonball->speed;
				}
				break;

			case SOUTHWEST:

				if(cannonball->position.y + cannonball->width/2 > getVerResolution() || ((int)(cannonball->position.x - cannonball->width/2)) < 0)
					return OUT_OF_BOUNDRIES;
				else
				{
					cannonball->position.x -= cannonball->speed;
					cannonball->position.y += cannonball->speed;
				}
				break;

			case WEST:

				if( ((int)(cannonball->position.x  - cannonball->width/2)) < 0)
					return OUT_OF_BOUNDRIES;
				else
				{
					//Colisions with Objects
					for(i = 0; i < obstacles_size; i++)
					{
				  /*x*/ if(cannonball->position.x - cannonball->width/2 >= obstacles[i]->position.x - obstacles[i]->length/3  && cannonball->position.x - cannonball->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
						{
					/*y*/ if(cannonball->position.y - cannonball->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && cannonball->position.y + cannonball->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2)
						  {
						  	return DELETE_BALL;
						  }
						}
					}

					//Colisions with Enemies
					for(i = 0; i < *enemies_size; i++)
					{
				  /*x*/ if(cannonball->position.x - cannonball->width/2 >= enemies[i]->position.x - enemies[i]->length/3  && cannonball->position.x - cannonball->width/2 <= enemies[i]->position.x + enemies[i]->length/2)
						{
					/*y*/ if(cannonball->position.y - cannonball->width/2  <= enemies[i]->position.y + enemies[i]->length/2 && cannonball->position.y + cannonball->width/2  >= enemies[i]->position.y - enemies[i]->length/2)
						 {
						  	if(enemy_dmg(enemies[i], cannonball->attack) != OK)
						  	{
						    	delete_enemy_array(enemies, i, enemies_size);
						    	*score += KILL_ENEMY_SCORE;
						  	}
						  	
						  	return DELETE_BALL;
						  }
						}
					}


					cannonball->position.x -= cannonball->speed;
				}
				break;

			case NORTHWEST:
				if( ((int)(cannonball->position.x  - cannonball->width/2)) < 0 || ((int)(cannonball->position.y  - cannonball->width/2)) < 0)
					return OUT_OF_BOUNDRIES;
				else
				{
					cannonball->position.x -= cannonball->speed;
					cannonball->position.y -= cannonball->speed;
				}
				break;
		}

		return 0;
}

//Cannon 
cannon_t* cannon_constructor(cannon_side_t side)
{
	cannon_t* cannon = malloc(sizeof(cannon_t));

	if(cannon == NULL)
	{
		printf("cannon::cannon_constructor: error in cannon creation\n;");
		return NULL;
	}

	//cannon->cannonballs = malloc(*sizeof(cannonball_t*));

	cannon->side = side;
	cannon->cannon_cooldown = 0;
	cannon->number_cannonballs = 0;

	return cannon;
}

void cannon_delete(cannon_t* cannon)
{
	unsigned int i;

	for(i = 0; i < cannon->number_cannonballs; i++)
	{
		cannonball_delete(cannon->cannonballs[i]);
	}

	free(cannon);
}

void delete_cannonball_array(cannonball_t** cannonballs, unsigned int index, unsigned int* number_cannonballs)
{
	cannonball_delete(cannonballs[index]);

	unsigned int i;
	for(i = index; i < *number_cannonballs - 1; i++)
	{
		cannonballs[i] = cannonballs[i + 1];
	}

	*number_cannonballs = *number_cannonballs - 1;

	cannonballs[*number_cannonballs] = NULL;
}

int cannon_fire(cannon_t* cannon, position_t boat_pos, orient_t boat_orient)
{
	if(cannon->cannon_cooldown != READY || cannon->number_cannonballs == MAX_CANNON_BALLS)
		return 1;

	orient_t ball_orient;
	position_t ball_pos = boat_pos;

	if(cannon->side == LEFT)
	{
		printf("DISPAROU Esquerdo\n");
		switch(boat_orient)
		{
			case NORTH:

			 	ball_orient = WEST;
			 	ball_pos.x -= BALL_GAP;
		 	break;

		 	case NORTHEAST:

			 	ball_orient = NORTHWEST;
			 	ball_pos.y -= BALL_GAP;
			 	ball_pos.x -= BALL_GAP;
		 	break;

		 	case EAST:

			 	ball_orient = NORTH;
			 	ball_pos.y -= BALL_GAP;
		 	break;

		 	case SOUTHEAST:

			 	ball_orient = NORTHEAST;
			 	ball_pos.y -= BALL_GAP;
			 	ball_pos.x += BALL_GAP;
		 	break;

		 	case SOUTH:

			 	ball_orient = EAST;
			 	ball_pos.x += BALL_GAP;
		 	break;

		 	case SOUTHWEST:

			 	ball_orient = SOUTHEAST;
			 	ball_pos.y += BALL_GAP;
			 	ball_pos.x += BALL_GAP;
		 	break;

		 	case WEST:

			 	ball_orient = SOUTH;
			 	ball_pos.y += BALL_GAP;
		 	break;

		 	case NORTHWEST:

			 	ball_orient = SOUTHWEST;
			 	ball_pos.y += BALL_GAP;
			 	ball_pos.x -= BALL_GAP;
		 	break;


		}
	}
	else //Cannon on the right side of the ship
	{
		printf("DISPAROU Direito\n");
		switch(boat_orient)
		{
			case NORTH:

			 	ball_orient = EAST;
			 	ball_pos.x += BALL_GAP;
		 	break;

		 	case NORTHEAST:

			 	ball_orient = SOUTHEAST;
			 	ball_pos.y += BALL_GAP;
			 	ball_pos.x += BALL_GAP;
		 	break;

		 	case EAST:
		 		printf("SUL!!\n");
			 	ball_orient = SOUTH;
			 	ball_pos.y += BALL_GAP;
		 	break;

		 	case SOUTHEAST:

			 	ball_orient = SOUTHWEST;
			 	ball_pos.y += BALL_GAP;
			 	ball_pos.x -= BALL_GAP;
		 	break;

		 	case SOUTH:

			 	ball_orient = WEST;
			 	ball_pos.x -= BALL_GAP;
		 	break;

		 	case SOUTHWEST:

			 	ball_orient = NORTHWEST;
			 	ball_pos.y -= BALL_GAP;
			 	ball_pos.x -= BALL_GAP;
		 	break;

		 	case WEST:

			 	ball_orient = NORTH;
			 	ball_pos.y -= BALL_GAP;
		 	break;

		 	case NORTHWEST:

			 	ball_orient = NORTHEAST;
			 	ball_pos.y -= BALL_GAP;
			 	ball_pos.x += BALL_GAP;
		 	break;
		}
	}

	cannon->cannonballs[cannon->number_cannonballs] = cannonball_constructor(CANNON_BALL_ATK, CANNON_BALL_RANGE, CANNON_BALL_WIDTH, ball_orient, ball_pos);
	cannon->cannon_cooldown = RELOAD_TIME;
	cannon->number_cannonballs++;
	return 0;
}

void cannon_move_balls(cannon_t* cannon, enemy_t** enemies, obstacle_t** obstacles, unsigned int obstacles_size, unsigned int* enemies_size, unsigned long* score)
{
	unsigned int i;
	for(i = 0; i < cannon->number_cannonballs; i++)
	{
		if(cannonball_move(cannon->cannonballs[i], enemies, obstacles, obstacles_size, enemies_size, score))
			delete_cannonball_array(cannon->cannonballs, i, &cannon->number_cannonballs);
	}
}


void cannon_decrease_cooldown(cannon_t* cannon)
{
	if( cannon->cannon_cooldown - 1 > 0)
	 cannon->cannon_cooldown--;
	else
		cannon->cannon_cooldown = 0;
}

void cannons_draw(cannon_t** cannons, Bitmap* bitmap, char* buffer)
{
	unsigned int i, j;

//
	for (i = 0; i < CANNONS_NUMBER; i++)
	{
		for(j = 0; j < cannons[i]->number_cannonballs; j++)
		{
			cannonball_draw(cannons[i]->cannonballs[j], bitmap, buffer);
		}
	}
	
}
