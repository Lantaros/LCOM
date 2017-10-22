#include <stdio.h>
#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "test5.h"
#include <limits.h>
#include <errno.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <string.h>

#include "pixmap.h"



//Prototypes
static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv) {
	sef_startup(); //initializes service - initialization of privileged user processes

	if (argc == 1) { /* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <number of packets>\"\n"
			"\t service run %s -args \"async <decimal no. - time>\"\n"
			"\t service run %s -args \"config\"\n"
			"\t service run %s -args \"gesture <decimal no. - lentght>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	//test_init arguments
	unsigned short mode;
	unsigned short delay;

	//test_square arguments
	unsigned short x, y;
	unsigned short size;
	unsigned short square_color;

	//test_line arguments
	unsigned short xi, yi, xf, yf;
	unsigned long line_color;

	//test_xpm arguments
	// xi, yi;
	//char xpm;

	//test_move arguments
	// xi, yi;
	//char move_xpm;
	unsigned short hor;
	short delta;
	unsigned short time;

	//test_controller arguments
	//(void)....

	if (strncmp(argv[1], "init", strlen("init")) == 0) {

		if (argc != 4) {
			printf("video: wrong no. of arguments for test_init()\n");
			return 1;
		}

		mode = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		delay = parse_ulong(argv[3], 10);

		if (mode == ULONG_MAX || delay == ULONG_MAX)
		{
			return 1;
		}

		printf("video::test_packet(%d, %d)\n", mode, delay);
		return test_init(mode, delay) != NULL;
	}

	else if (strncmp(argv[1], "square", strlen("square")) == 0) {

		if (argc != 6) {
			printf("video: wrong no. of arguments for test_square()\n");
			return 1;
		}

		x = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		y = parse_ulong(argv[3], 10);
		size = parse_ulong(argv[4], 10);
		square_color = parse_ulong(argv[5], 10);

		if (x == ULONG_MAX || y == ULONG_MAX || size == ULONG_MAX || square_color == ULONG_MAX)
			return 1;

		printf("video::test_square(%d, %d, %d, %d)\n", x, y, size,
				square_color);
		return test_square(x, y, size, square_color);
	}

	else if (strncmp(argv[1], "line", strlen("line")) == 0) {

		if (argc != 7) {
			printf("video: wrong no of arguments for test_line()\n");
			return 1;
		}

		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		xf = parse_ulong(argv[4], 10);
		yf = parse_ulong(argv[5], 10);
		line_color = parse_ulong(argv[6], 10);

		if (xi  == ULONG_MAX || yi == ULONG_MAX || xf  == ULONG_MAX|| yf  == ULONG_MAX || line_color == ULONG_MAX)
			return 1;

		printf("video::test_line(%d, %d, %d, %d, %d)\n", xi, yi, xf, yf,
				line_color);
		return test_line(xi, yi, xf, yf, line_color);
	}

	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {

		if (argc != 5) {
			printf("video: wrong no of arguments for test_xpm()\n");
			return 1;
		}

		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);

		if (xi == ULONG_MAX || yi == ULONG_MAX)
			return 1;

		//Get XPM
		char ** xpm;
		xpm = get_xpm(argv[4]);

		if(xpm == NULL)
		{
			printf("Invalid xpm %s\n", argv[4]);
			return 1;
		}

		printf("video::test_xpm(%d, %d, xpm[])\n", xi, yi);
		return test_xpm(xi, yi, xpm);
	}

	else if (strncmp(argv[1], "move", strlen("move")) == 0) {

		if (argc != 8) {
			printf("video: wrong no of arguments for test_move()\n");
			return 1;
		}

		xi = parse_ulong(argv[2], 10);
		yi = parse_ulong(argv[3], 10);
		hor = parse_ulong(argv[5], 10);
		delta = parse_ulong(argv[6], 10);
		time = parse_ulong(argv[7], 10);

		if (xi == ULONG_MAX || yi == ULONG_MAX|| hor == ULONG_MAX|| time == ULONG_MAX|| delta == ULONG_MAX)
			return 1;

		//Get XPM
		char ** xpm;
		xpm = get_xpm(argv[4]);

		if(xpm == NULL)
		{
			printf("Invalid xpm %s\n", argv[4]);
			return 1;
		}

		printf("video::test_move(%d, %d, xpm[], %d, %d, %d)\n", xi, yi, hor,
				delta, time);
		return test_move(xi, yi, xpm, hor, delta, time);
	}

	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {

		if (argc != 2) {
			printf("video: wrong no of arguments for test_controlelr()\n");
			return 1;
		}

		return test_controller();

	} else {
		printf("video: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}
	if (endptr == str) {
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}
	/* Successful conversion */
	return val;
}
