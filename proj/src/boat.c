#include "boat.h"

//Boat Speed Macros
#define SPEED_ACEL 2
#define SPEED_NULL 0
#define SPEED_MAX 4

#define DIAGONAL_RATIO 0.8

//Rotations
#define CLOCK_WISE 1
#define ANTI_CLOCK_WISE -1



boat_t* boat_constructor(unsigned int hp, unsigned int width, unsigned int length, orient_t orientation, position_t position)
{
	boat_t* boat = malloc(sizeof(boat_t));
	if(boat == NULL)
	{
		printf("boat::boat_constructor: error in boat creation\n;");
		return NULL;
	}


	boat->hp = hp;
	boat->state = ALIVE;
	boat->width = width;
	boat->length = length;
	boat->orientation = orientation;
	boat->position = position;
	boat->speed = SPEED_NULL;

	boat->dmg_flag = TRUE;

	boat->cannons[0] = cannon_constructor(LEFT);
	boat->cannons[1] = cannon_constructor(RIGHT);
	
	return boat;
}


void boat_delete(boat_t* boat)
{	unsigned int i;

	for (i = 0; i < 8; i++)
	{
		deleteBitmap(boat->bitmaps[i]);
	}

	for(i = 0; i < sizeof(boat->cannons)/sizeof(cannon_t*); i++)
	{
		cannon_delete(boat->cannons[i]);
	}

	free(boat);
}



int boat_draw(boat_t* boat, char* buffer)
{
	drawBitmap(buffer, boat->bitmaps[boat->orientation], boat->position.x, boat->position.y, ALIGN_CENTER);
	//printf("Boat Orientation: %d\n",boat->orientation);
	return 0;
}

void boat_dmg(boat_t* boat, unsigned int dmg)
{ 
	if(boat->dmg_flag)
	{
		if( (int)boat->hp - (int)dmg > 0)
		{
			boat->hp -= dmg;
			boat->dmg_flag = FALSE;
		}
		else
			boat->state = DEAD; 
	}
}

int boat_move(boat_t* boat, const unsigned long* scancode, obstacle_t** obstacles, unsigned int obstacles_size, enemy_t** enemies, unsigned int* enemies_size)
{
	unsigned int i;
	//printf("boat->boat_keys.right: 0x%x\n",boat->boat_keys.right);
	//printf("boat->boat_keys.up: 0x%x\n",boat->boat_keys.up);

	if(W_MK == *scancode)
	{	
			//printf("\nnorth increment successful\n");
			if(boat->speed + SPEED_ACEL < SPEED_MAX)
				boat->speed += SPEED_ACEL;
			else
				boat->speed = SPEED_MAX;
	}
	
	else if(S_MK == *scancode)
	{
			//printf("\nsouth increment sucessful\n");
			if(boat->speed - SPEED_ACEL > SPEED_NULL)
				boat->speed -= SPEED_ACEL;
			else
				boat->speed = SPEED_NULL;
	}

	else if(A_MK == *scancode)
	{
			//printf("INCREMENT -\n");
			boat_orientation_increment(boat, ANTI_CLOCK_WISE);
			//boat->orientation--;
	}

	else if(D_MK == *scancode)
	{
			//printf("INCREMENT +\n");
			boat_orientation_increment(boat, CLOCK_WISE);
	}

/*----------------MOVEMENT-----------------------*/
	switch(boat->orientation)
	{
		case NORTH:

			if( ((int)(boat->position.y - boat->speed - boat->length/2)) < 0)
			{
				printf("Fora limites Sup\n");
				return 1;
			}
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x -  boat->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(boat->position.y - boat->length/3  <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y - boat->length/3 >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			 	  if(boat->position.x + boat->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x -  boat->width/2 <= enemies[i]->position.x + enemies[i]->length/2)
					{
					 if(boat->position.y - boat->length/3  <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y - boat->length/3 >= enemies[i]->position.y - enemies[i]->length/2)
					  {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}
				

				boat->position.y -= boat->speed;
			}
			break;

		case NORTHEAST:
			
			if( ((int)(boat->position.y - boat->speed - boat->length/2)) < 0 || boat->position.x + boat->speed + boat->length/2 > getHorResolution())
				return 1;
			else
			{

				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x - obstacles[i]->length/2 <= obstacles[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x - enemies[i]->length/2 <= enemies[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= enemies[i]->position.y - enemies[i]->length/2)
					  {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}


				boat->position.x += boat->speed;
				boat->position.y -= boat->speed;
			}			
			break;

		case EAST:

			if(boat->position.x + boat->speed + boat->length/2 > getHorResolution())
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->length/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x - boat->length/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(boat->position.y - boat->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y + boat->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->length/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x - boat->length/2 <= enemies[i]->position.x + enemies[i]->length/2)
					{
				/*y*/ if(boat->position.y - boat->width/2  <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y + boat->width/2  >= enemies[i]->position.y - enemies[i]->length/2)
					  {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}

				boat->position.x += boat->speed;
			}
			break;

		case SOUTHEAST:

			if(boat->position.x + boat->speed + boat->length/2 > getHorResolution() || boat->position.y + boat->speed + boat->length/2 > getVerResolution())
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x - obstacles[i]->length/2 <= obstacles[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y + boat->length/3 <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y - boat->length/3  >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x - enemies[i]->length/2 <= enemies[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= enemies[i]->position.y - enemies[i]->length/2)
					  {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}

				boat->position.x += boat->speed;
				boat->position.y += boat->speed;
			}
			break;

		case SOUTH:

			if(boat->position.y + boat->speed + boat->length/2 > getVerResolution())
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x -  boat->width/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(boat->position.y + boat->length/3  >= obstacles[i]->position.y - obstacles[i]->length/2 && boat->position.y - boat->length/3 <= obstacles[i]->position.y + obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x -  boat->width/2 <= enemies[i]->position.x + enemies[i]->length/2)
					{
				/*y*/ if(boat->position.y + boat->length/3  >= enemies[i]->position.y - enemies[i]->length/2 && boat->position.y - boat->length/3 <= enemies[i]->position.y + enemies[i]->length/2)
					  {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}

				boat->position.y += boat->speed;
			}
			break;

		case SOUTHWEST:

			if(boat->position.y + boat->speed + boat->length/2 > getVerResolution() || ((int)(boat->position.x - boat->speed - boat->length/2)) < 0)
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x - obstacles[i]->length/2 <= obstacles[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	//boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x - enemies[i]->length/2 <= enemies[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= enemies[i]->position.y - enemies[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	//boat_dmg(boat, enemies[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				boat->position.x -= boat->speed;
				boat->position.y += boat->speed;
			}
			break;

		case WEST:

			if( ((int)(boat->position.x - boat->speed - boat->length/2)) < 0)
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x - boat->length/3 >= obstacles[i]->position.x - obstacles[i]->length/3  && boat->position.x - boat->length/2 <= obstacles[i]->position.x + obstacles[i]->length/2)
					{
				/*y*/ if(boat->position.y - boat->width/2  <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y + boat->width/2  >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x - boat->length/3 >= enemies[i]->position.x - enemies[i]->length/3  && boat->position.x - boat->length/2 <= enemies[i]->position.x + enemies[i]->length/2)
					{
				/*y*/ if(boat->position.y - boat->width/2  <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y + boat->width/2  >= enemies[i]->position.y - enemies[i]->length/2)
					  {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}


				boat->position.x -= boat->speed;
			}
			break;

		case NORTHWEST:
			if( ((int)(boat->position.x - boat->speed - boat->length/2)) < 0 || ((int)(boat->position.y - boat->speed - boat->length/2)) < 0)
				return 1;
			else
			{
				//Colisions with Objects
				for(i = 0; i < obstacles_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= obstacles[i]->position.x - obstacles[i]->length/2  && boat->position.x - obstacles[i]->length/2 <= obstacles[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= obstacles[i]->position.y + obstacles[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= obstacles[i]->position.y - obstacles[i]->length/2)
					  {
					  	printf("BATEU\n");	
					  	//boat_dmg(boat, obstacles[i]->colision_dmg);
					  	return 1;
					  }
					}
				}

				//Colisions with Enemies
				for(i = 0; i < *enemies_size; i++)
				{
			  /*x*/ if(boat->position.x + boat->width/2 >= enemies[i]->position.x - enemies[i]->length/2  && boat->position.x - enemies[i]->length/2 <= enemies[i]->position.x + ROCK_LENGTH/2)
					{
				/*y*/ if(boat->position.y - boat->length/3 - boat->speed <= enemies[i]->position.y + enemies[i]->length/2 && boat->position.y - boat->length/3 - boat->speed >= enemies[i]->position.y - enemies[i]->length/2)
					 {
					  	printf("APAGOU COBRA\n");
					  	boat_dmg(boat, enemies[i]->attack);
					    delete_enemy_array(enemies, i, enemies_size);
					  	break;
					  }
					}
				}

				boat->position.x -= boat->speed;
				boat->position.y -= boat->speed;
			}
			break;
	}

	return 0;
}


int load_boat_sprites(boat_t* boat)
{
	Bitmap* test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_N.bmp");

	if(test_boat_bmp == NULL)  //If one loads, the others must load as well
		printf("Imagem nao carregou\n");

	boat->bitmaps[0] = test_boat_bmp;


	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_NE.bmp");
	boat->bitmaps[1] = test_boat_bmp;

	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_E.bmp");
	boat->bitmaps[2] = test_boat_bmp;

	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_SE.bmp");
	boat->bitmaps[3] = test_boat_bmp;

	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_S.bmp");
	boat->bitmaps[4] = test_boat_bmp;

	printf("Boat Length: %d\n",test_boat_bmp->bitmapInfoHeader.height);


	if(test_boat_bmp == NULL)
		printf("Imagem Sul nao carregou\n");

	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_SW.bmp");
	boat->bitmaps[5] = test_boat_bmp;

	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_W.bmp");
	boat->bitmaps[6] = test_boat_bmp;

	test_boat_bmp = loadBitmap("/tmp/resources/blue_ship_NW.bmp");
	boat->bitmaps[7] = test_boat_bmp;

	return 0;
}


void boat_orientation_increment(boat_t* boat, int direction)
{
	//printf("AQUI\n");
	//positive direction = clockwise rotation
	//negative direction = counter-clockwise direction
	if(direction > 0)
	{
		switch(boat->orientation)
		{
		case NORTH:
			boat->orientation = NORTHEAST;
			break;
		case NORTHEAST:
			boat->orientation = EAST;
			break;
		case EAST:
			boat->orientation = SOUTHEAST;
			break;
		case SOUTHEAST:
			boat->orientation = SOUTH;
			break;
		case SOUTH:
			boat->orientation = SOUTHWEST;
			break;
		case SOUTHWEST:
			boat->orientation = WEST;
			break;
		case WEST:
			boat->orientation = NORTHWEST;
			break;
		case NORTHWEST:
			boat->orientation = NORTH;
			break;
		default:
			printf("\nError in boat_orientation_increment\n");
		}
	}

	else
	{
		switch(boat->orientation)
		{
		case NORTH:
			boat->orientation = NORTHWEST;
			break;
		case NORTHWEST:
			boat->orientation = WEST;
			break;
		case WEST:
			boat->orientation = SOUTHWEST;
			break;
		case SOUTHWEST:
			boat->orientation = SOUTH;
			break;
		case SOUTH:
			boat->orientation = SOUTHEAST;
			break;
		case SOUTHEAST:
			boat->orientation = EAST;
			break;
		case EAST:
			boat->orientation = NORTHEAST;
			break;
		case NORTHEAST:
			boat->orientation = NORTH;
			break;
		default:
			printf("\nError in boat_orientation_increment\n");
		}

	}
}

int boat_fire_cannons(boat_t* boat, mouse_t* mouse)
{
	unsigned int i;
	cannon_side_t fire_side;

	if(mouse->left_button)
		fire_side = LEFT;
	else 
	{
		if(mouse->right_button)
			fire_side = RIGHT;
		 else
			return 1;
	}

		for(i = 0; i < CANNONS_NUMBER; i++)
		{
			if(boat->cannons[i]->side == fire_side)
			{
				if(cannon_fire(boat->cannons[i], boat->position, boat->orientation))
					return 0;
			}
		}

		return 1;
}

void boat_decrease_cannon_cooldown(boat_t* boat)
{
	unsigned int i;

	for (i = 0; i < CANNONS_NUMBER; i++)
	{
		cannon_decrease_cooldown(boat->cannons[i]);
	}
}


