#include "bitmap.h" //Ferrolho's BMP Code
#include <minix/drivers.h>
#include <minix/sysutil.h>
//Our Includes
//Timer
#include "i8254.h"
#include "timer.h"
//KBD
unsigned int ass_handler(); //kbd_interrupt in Assembly
#include "i8042.h"
#include "kbd.h"

//Graphics
#include "video.h"
#include "video_gr.h"

//Game files and structs
#include "boat.h"
#include "obstacle.h"
#include "game.h"
#include "utilities.h"

#include <time.h> // For srand(time(NULL))
#include <math.h>

#define LETTER_SIZE 25
#define LIFE_INFO_X 1150
#define LIFE_INFO_Y 10
#define TIME_SCORE_INC 10

#define MIN_GAP 5
#define INVALID_POSITION 1
#define MAX_ENEMIES_REACHED 2

//Title
#define TITLE_LENGTH 400
#define TITLE_HEIGHT 100

//Mouse
#define MOUSE_START_X 30
#define MOUSE_START_Y 30
#define MOUSE_WIDTH 20

int driver_receive(int type, message* msg, int* ipc_status);


void set_enemies_change_orient_flag(game_t* game, char bool)
{
	unsigned int i;
	for(i = 0; i < game->enemies_size; i++)
	 game->enemies[i]->change_orientatiom_flag = bool;
}


int game_start(game_t* game)
{

	//Flags
	game->second_passed_flag = FALSE;
	game->score = 0;
	game->state = MENU;

	game->mouse_pos.x = MOUSE_START_X;
	game->mouse_pos.y = MOUSE_START_Y;

	//Array sizes
	game->enemies_size = 0;
	game->obstacles_size = 0;


	//Alocation of secondary video buffers
	game->buffer_video2 = malloc(sizeof(char) * game->video_mem_size);
	game->background_buffer = malloc(sizeof(char) * game->video_mem_size);

	//Load Sprits

	Bitmap* background = loadBitmap("/tmp/resources/the_big_sea_v3.bmp");

	game->obstacle_bmp = loadBitmap("/tmp/resources/rock_v2.bmp");

	game->title_bmp = loadBitmap("/tmp/resources/plunder.bmp");
	game->high_score_bmp = loadBitmap("/tmp/resources/highscores.bmp");
	game->button_bmp = loadBitmap("/tmp/resources/play.bmp");
	game->mouse_bmp = loadBitmap("/tmp/resources/mouse.bmp");
	
	if(load_numbers(game) != OK)
		return BITMAP_MISSING;

	load_enemy_sprites(game->enemy_bmp);

	game->hp[0] = loadBitmap("/tmp/resources/H.bmp");
	game->hp[1] = loadBitmap("/tmp/resources/P.bmp");

	game->cannonball_bmp = loadBitmap("/tmp/resources/cannonball.bmp");
	if(game->cannonball_bmp == NULL)
		printf("cannonball not found\n");

	drawBitmap(game->background_buffer, background, 0, 0, ALIGN_LEFT);
	deleteBitmap(background);


	position_t pos;
	pos.x = 100;
	pos.y = 100;

	position_t pos_rock;
	pos_rock.x = 200;
	pos_rock.y = 200;

	position_t pos_enemy;
	pos_enemy.x = 400;
	pos_enemy.y = 200;

	game->mouse = malloc(sizeof(mouse_t));

	//game->boat = boat_constructor(100.0, test_boat_bmp->bitmapInfoHeader.width, test_boat_bmp->bitmapInfoHeader.height, SOUTH, pos, test_boat_keys);
	game->boat = boat_constructor(100.0, 40, BOAT_LENGTH, SOUTH, pos);
	load_boat_sprites(game->boat);



	 
	/*game->obstacles_size++;
	game->obstacles[0]= obstacle_constructor(ROCK_DMG, ROCK_LENGTH, ROCK_LENGTH, pos_rock);*/

	generate_obstacles(game->obstacles, &game->obstacles_size);
	

	/*game->enemies[0] = enemy_constructor(100, 20, 10, 10, NORTH, pos_enemy);
	game->enemies_size++;*/

//--------------------------------------------------------------------------------------------------
	//Main Interruption Handling
	int r;
	int ipc_status;
	message msg;

	//timer input handling
	int irq_set_timer = BIT(timer_subscribe_int());

	//keyboard input handling
	int irq_set_kbd = BIT(kbd_subscribe_int());
	game->scancode = 0;
	

	//mouse input handling
	int irq_set_mouse = BIT(mouse_subscribe_int());	
	unsigned char packet[3];
	unsigned int count_packet = 0;

	//Enable mouse data reporting
	if (write_command_mouse(IN_BUF, MOUSE_SET_STREAM_MODE) != OK)
	{
		printf("game::error enabling mouse stream mode\n");
		return 1;
	}

	if (write_command_mouse(IN_BUF, MOUSE_ENABLE_DATA_REPORTING) != OK)
	{
		printf("game::error enabling mouse data reporting\n");
		return 1;
	}

	mouse_packet_ignore(irq_set_mouse);
	printf("\nAfter Packet Ignore\n");

	while (game->state != END_GAME && game->scancode != ESC_BRK)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}



		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_timer)
				{
					timer_int_handler();
					
					switch(game->state)
					{
						case MENU:
							game_menu(game);
						break;	
					
						case GAME:
							if(game_update(game) != OK)	
							{
								game->state = END_GAME;				
								printf("Player died\n");
							}
						break;

						case END_GAME: //Its going to exit
						break;
					}
					
					//printf("after game update\n");

					if(getInterruptCounter()%30 == OK) //Half a second
					{
						set_enemies_change_orient_flag(game, TRUE);
					}

					if(getInterruptCounter() == 60)
					{
						setInterruptCounter(0);
						game->second_passed_flag = TRUE;
					}
				}

				if (msg.NOTIFY_ARG & irq_set_kbd)
				{				
					kbd_int_handler(&game->scancode);
				}
				//printf("\ntesto 3\n");

				if (msg.NOTIFY_ARG & irq_set_mouse)
				{
					if (mouse_int_handler(&packet[count_packet]) != OK)
					{
						printf("game: mouse_int_handler failed\n");
					}

					count_packet++;

					if (!(packet[0] & BIT(3))) //sync mouse data
					{
						printf("Invalid 1 Byte, 0x%x\n", packet[0]);
						count_packet = 0;
					}

					
				}

				if (count_packet == 3)
				{
					count_packet = 0;
					convert_packet_struct(packet, game->mouse);
					print_mouse_struct(game->mouse);
					//mouse_print_packet(packet);
				}
			}
		}

	}


	//game_end(game);

	kbd_unsubscribe_int();

	if (write_command_mouse(IN_BUF, MOUSE_DISABLE_DATA_REPORTING) != OK)
	{
		printf("game: error while disabling mouse data reporting\n");
	}


	printf("\nAfter interruption loop\n");

	clear_in_buf(); //might not need this

	mouse_unsubscribe_int();


	//printf("\nAfter Mouse unsubscribe\n");

	return 0;
}

void game_menu(game_t* game)
{
	memcpy(game->buffer_video2, game->background_buffer, game->video_mem_size);
	drawBitmap(game->buffer_video2, game->title_bmp, 1280, 100, ALIGN_LEFT);
	
	drawBitmap(game->buffer_video2, game->button_bmp, 600, 600, ALIGN_CENTER);
	
	drawBitmap(game->buffer_video2, game->mouse_bmp, game->mouse_pos.x, game->mouse_pos.y, ALIGN_CENTER);

	memcpy(game->video_mem, game->buffer_video2, game->video_mem_size);

	mouse_update_position(game->mouse, &game->mouse_pos);

	if(game->mouse->left_button)
	{
		if(game->mouse_pos.x - MOUSE_WIDTH/2 >= 600 - TITLE_LENGTH/2 && game->mouse_pos.x + MOUSE_WIDTH/2 <= 600 + TITLE_LENGTH/2)
			if(game->mouse_pos.y - MOUSE_WIDTH/2 >= 600 - TITLE_HEIGHT/2 && game->mouse_pos.y + MOUSE_WIDTH/2 <= 600 + TITLE_HEIGHT/2)
				{
					printf("DEBUG::BUTTON Pressed\n");
					game->state = GAME;
				}
	}

	//game->state = GAME;
}

int game_update(game_t* game)
{
	unsigned int i;
	//printf("Entrou game_update()\n");
	switch(game->boat->state)
	{
		case ALIVE:

			if(game->second_passed_flag)
			{
				game->boat->dmg_flag = TRUE;
				boat_decrease_cannon_cooldown(game->boat); //Decreases all cannons cooldown by 1 second
				game->score += TIME_SCORE_INC;

				game->second_passed_flag = FALSE;
			}

			enemy_generator(game);

			memcpy(game->buffer_video2, game->background_buffer, game->video_mem_size);//Copy background to scondary video buffer

			boat_fire_cannons(game->boat, game->mouse);

			boat_move(game->boat, &game->scancode, game->obstacles, game->obstacles_size, game->enemies, &game->enemies_size);

			for(i = 0; i < game->enemies_size; i++)
				enemy_move(game->enemies[i], game->obstacles, game->obstacles_size);

			for(i = 0; i < CANNONS_NUMBER; i++)
				cannon_move_balls(game->boat->cannons[i], game->enemies, game->obstacles, game->obstacles_size, &game->enemies_size, &game->score);

			boat_draw(game->boat, game->buffer_video2);
			
			for(i = 0; i < game->obstacles_size; i++)
				obstacle_draw(game->obstacles[i], game->obstacle_bmp, game->buffer_video2);

			

			for(i = 0; i < game->enemies_size; i++)
			{
				enemy_draw(game->enemies[i], game->enemy_bmp, game->buffer_video2);
			}

			cannons_draw(game->boat->cannons, game->cannonball_bmp, game->buffer_video2); //Draw all instantiated cannon balls

			print_hp(game->hp, game->numbers, game->buffer_video2, game->boat->hp, LIFE_INFO_X, LIFE_INFO_Y);

			memcpy(game->video_mem, game->buffer_video2, game->video_mem_size); //Copy next frame to the mapped video memory


			game->scancode = 0; //Clear scancode variable
		break;

		case DEAD:
			return 1;
	}
	return 0;
}

void print_hp(Bitmap** hp, Bitmap** numbers, char* buffer , unsigned int number, unsigned int x, unsigned int y)
{
	draw_number(numbers, buffer, number, x, y);

	drawBitmap(buffer, hp[1], x - 3*LETTER_SIZE, y, ALIGN_LEFT); //P - 3 max number of Health algarisms
	drawBitmap(buffer, hp[0], x - 4*LETTER_SIZE, y, ALIGN_LEFT); //H
}

void draw_number(Bitmap** array, char* buffer , unsigned int number, unsigned int x, unsigned int y)
{
	for(; number > 0; number /= 10)
	{
		drawBitmap(buffer, array[number % 10], x, y, ALIGN_LEFT);
		x -= LETTER_SIZE;
	}
}

int load_numbers(game_t* game)
{
	game->numbers[0] = loadBitmap("/tmp/resources/0.bmp");

	if(game->numbers[0] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[1] = loadBitmap("/tmp/resources/1.bmp");

	if(game->numbers[1] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[2] = loadBitmap("/tmp/resources/2.bmp");

	if(game->numbers[2] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[3] = loadBitmap("/tmp/resources/3.bmp");

	if(game->numbers[3] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[4] = loadBitmap("/tmp/resources/4.bmp");

	if(game->numbers[4] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[5] = loadBitmap("/tmp/resources/5.bmp");

	if(game->numbers[5] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[6] = loadBitmap("/tmp/resources/6.bmp");

	if(game->numbers[6] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[7] = loadBitmap("/tmp/resources/7.bmp");

	if(game->numbers[7] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[8] = loadBitmap("/tmp/resources/8.bmp");

	if(game->numbers[8] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	game->numbers[9] = loadBitmap("/tmp/resources/9.bmp");

	if(game->numbers[9] == NULL)
	{
		delete_numbers(game->numbers);
		return 1;
	}

	return 0;
}

void enemy_generator(game_t* game)
{
	unsigned int score_max_enemies = (unsigned int) log(game->score) % MAX_ENEMIES ;
	int return_val = -1;

	while(score_max_enemies)
	{
		return_val = enemy_rand_constructor(game->boat->position, game->obstacles, game->obstacles_size, game->enemies, &game->enemies_size, game->boat->cannons);
			
			if(return_val == MAX_ENEMIES_REACHED)
				break;
		else if(return_val == INVALID_POSITION)
			continue;
		else
			score_max_enemies --;


	}
}

int enemy_rand_constructor(position_t boat_pos, obstacle_t** obstacles, unsigned int obstacles_size, enemy_t** enemies, unsigned int* enemies_size, cannon_t** cannons)
{
	unsigned int i, j;
	srand (time(NULL));

	if(*enemies_size == MAX_ENEMIES)
		return MAX_ENEMIES_REACHED;

	orient_t orientation = rand() % 7;

	position_t position = {X_ORIGIN + ENEMY_WIDTH/2 , Y_ORIGIN + ENEMY_WIDTH/2};

		position.x +=  (rand() + ENEMY_WIDTH/2 + MIN_GAP) % (X_LIMIT - ENEMY_WIDTH/2);
		
		position.y +=  (rand() + ENEMY_WIDTH/2 + MIN_GAP) % (Y_LIMIT - ENEMY_WIDTH/2);

		//Boat
		if( (position.x + ENEMY_WIDTH/2 == boat_pos.x - BOAT_LENGTH/2 || position.x - ENEMY_WIDTH/2 == boat_pos.x + BOAT_LENGTH/2)
			&& (position.y + ENEMY_WIDTH/2 == boat_pos.y - BOAT_LENGTH/2 || position.y - ENEMY_WIDTH/2 == boat_pos.y + BOAT_LENGTH/2))
				return INVALID_POSITION;

		//Obstacles
		for(i = 0; i < obstacles_size; i++)
		{
			if( (position.x + ENEMY_WIDTH/2 == obstacles[i]->position.x - ROCK_LENGTH/2 || position.x - ENEMY_WIDTH/2 == obstacles[i]->position.x + ROCK_LENGTH/2)
				&& (position.y + ENEMY_WIDTH/2 == obstacles[i]->position.y - ROCK_LENGTH/2 || position.y - ENEMY_WIDTH/2 == obstacles[i]->position.y + ROCK_LENGTH/2))
				return INVALID_POSITION;
		}

		//Enemies
		for(i = 0; i < *enemies_size; i++)
		{
			if( (position.x + ENEMY_WIDTH/2 == enemies[i]->position.x - ROCK_LENGTH/2 || position.x - ENEMY_WIDTH/2 == enemies[i]->position.x + ROCK_LENGTH/2)
				&& (position.y + ENEMY_WIDTH/2 == enemies[i]->position.y - ROCK_LENGTH/2 || position.y - ENEMY_WIDTH/2 == enemies[i]->position.y + ROCK_LENGTH/2))
				return INVALID_POSITION;
		}

		//Cannons
		for(i = 0; i < sizeof(cannons)/sizeof(cannon_t); i++)
		{
			for(j = 0; j < cannons[i]->number_cannonballs; j++)
			{
				if( (position.x + ENEMY_WIDTH/2 == cannons[i]->cannonballs[j]->position.x - ROCK_LENGTH/2 || position.x - ENEMY_WIDTH/2 == cannons[i]->cannonballs[j]->position.x + ROCK_LENGTH/2)
					&& (position.y + ENEMY_WIDTH/2 == cannons[i]->cannonballs[j]->position.y - ROCK_LENGTH/2 || position.y - ENEMY_WIDTH/2 == cannons[i]->cannonballs[j]->position.y + ROCK_LENGTH/2))
					return INVALID_POSITION;
			}
		}


		enemies[*enemies_size] = enemy_constructor(ENEMY_HP, ENEMY_ATK, ENEMY_WIDTH, ENEMY_LENGTH, orientation, position);
		*enemies_size = *enemies_size + 1;

return 0;
}

void delete_numbers(Bitmap** numbers)
{
	unsigned int i;

	for(i = 0; i < sizeof(numbers)/sizeof(Bitmap*); i++)
	{
		deleteBitmap(numbers[i]);
	}
}

int mouse_update_position(mouse_t* mouse, position_t* position)
{
	if(position->x + mouse->delta_x >  MOUSE_WIDTH && position->x + mouse->delta_x < getHorResolution() - MOUSE_WIDTH)
		position->x += mouse->delta_x;

	if(position->y + mouse->delta_y > MOUSE_WIDTH && position->y + mouse->delta_y < getVerResolution() - MOUSE_WIDTH)
		position->y += mouse->delta_y;

	return 0;
}

void game_score(game_t* game)
{
	memcpy(game->buffer_video2, game->background_buffer, game->video_mem_size);

	drawBitmap(game->buffer_video2, game->high_score_bmp, 600, 200, ALIGN_CENTER);

	draw_number(game->numbers, game->buffer_video2, (unsigned int) game->score, 100, 500);

	memcpy(game->video_mem, game->buffer_video2, game->video_mem_size);

	sleep(10);
}

void game_end(game_t* game)
{
	//TODO: Garbage collection
	//free(game->video_mem);
	free(game->background_buffer);
	free(game->buffer_video2);

	boat_delete(game->boat);

	free(game->mouse);

	//Bitmaps
	deleteBitmap(game->hp[0]);
	deleteBitmap(game->hp[1]);
	delete_bitmap_array(game->enemy_bmp);
	delete_numbers(game->numbers);
	deleteBitmap(game->cannonball_bmp);
	deleteBitmap(game->mouse_bmp);
	deleteBitmap(game->high_score_bmp);
	deleteBitmap(game->title_bmp);
	deleteBitmap(game->obstacle_bmp);
	deleteBitmap(game->button_bmp);

	free(game);
}