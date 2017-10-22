#ifndef __TEST5_H
#define __TEST5_H

void *test_init(unsigned short mode, unsigned short delay);

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color);

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color);

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]);

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time);

int test_controller();

#endif /* __TEST_5.H */
