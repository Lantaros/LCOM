#pragma once


//#define BIT(n) (0x01 << n)
#define singleBit(byte) (0x01 & byte)
#define swap(type, i, j) {type t = i; i = j; j = t;}

//-----Movement Scancodes------------
#define W_MK 0x11
#define W_BRK 0x91

#define A_MK 0x1E
#define A_BRK 0x9E

#define S_MK 0x1F
#define S_BRK 0x9F

#define D_MK 0x20
#define D_BRK 0xA0
//-------------------------------------

#define TRUE 1
#define FALSE 0

/* ORIENTATION Clock-Wise
#define NORTH 	  	0
#define NORTHEAST 	1
#define EAST 	  	2
#define SOUTHEAST   3
#define SOUTH 	  	4
#define SOUTHWEST 	5
#define WEST 	  	6
#define NORTHWEST 	7
*/

typedef enum {NORTH = 0, NORTHEAST = 1, EAST = 2, SOUTHEAST = 3, SOUTH = 4, SOUTHWEST = 5, WEST = 6, NORTHWEST = 7} orient_t;

//Position
#define X_ORIGIN 0
#define Y_ORIGIN 0

#define X_LIMIT 1280
#define Y_LIMIT 1024

typedef struct
{
	int x;
	int y;
} position_t;

