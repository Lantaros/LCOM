#include <stdio.h>
#include "video.h"
#include <sys/mman.h>

//static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res = 1024;	        /* Horizontal resolution in pixels */
static unsigned v_res = 768;	        /* Vertical resolution in pixels */
//static unsigned bits_per_pixel = 8; /* Number of VRAM bits per pixel */


int draw_square(char* video_mem, unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	unsigned short i, y_initial = y;

	for(; y < (y_initial + size); y++)
	{
		for(i = 0; i < size; i++)
			video_mem[x+i+(h_res * y)] = (char) color;
	}

	return 0;
}

int draw_line(char* video_mem, unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)
{
	printf("Xi: %d  Yi: %d\n", xi, yi);
	printf("Xf: %d  Yf: %d\n", xf, yf);

	if(yi > yf)//Invert points, only if yi > yf
	{
		unsigned short tmp = yf;
		yf = yi;
		yi = tmp;

		tmp = xf;
		xf = xi;
		xi = tmp;
	}

	if(xf > h_res || yf > v_res) //Coordinates out of bounds
		return 1;

	if(yi == yf)
	{
		for(;xi <= xf; xi++)
		{
			video_mem[xi + yi* h_res] = color;
		}
	}
	else if(xi == xf)
		{
			printf("Entrou\n");
			for(;yi <= yf; yi++)
			{
				draw_point(video_mem, xi, yi, color);
			}
		}
	else //Diagonal lines
	{

		float slope = (yf - yi)/(xf - xi);
		int b = yi - slope * xi;
		printf("Xi: %d  Yi: %d\n", xi, yi);
		printf("Xf: %d  Yf: %d\n", xf, yf);
		printf("Slope: %d\nb: %d\n", slope, b);

		for(;yi < yf; yi++)
		{
			for(;xi < xf; xi++)
				draw_point(video_mem, xi, (int) (slope * xi + b), color);
		}

	}
	return 0;
}

//Copied from VESA 2.0 Docummentation file
int draw_line_v2(char* video_mem, unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)
{
    int x1 = xi, y1 = yi, x2 = xf, y2 = yf;
    int d;
    int dx, dy;
    int Eincr, NEincr;
    int yincr;
    int t;

#define ABS(a) ((a) >= 0 ? (a) : -(a))

    dx = ABS(x2 - x1);
    dy = ABS(y2 - y1);
    if(dy <= dx)
    {
        if(x2 < x1)
        {
            t = x2; x2 = x1; x1 = t;
            t = y2; y2 = y1; y1 = t;
        }
        if(y2 > y1)
            yincr = 1;
        d = 2*dy - dx;
        Eincr = 2*dy;
        NEincr = 2*(dy - dx);

        for(x1++; x1 <= x2; x1++)
        {
            if(d < 0)
                d += Eincr;
            else
            {
                d += NEincr;
                y1 += yincr;
            }
            //draw pixel in x1 y1
            video_mem[x1 + y1* h_res] = color;
        }
    }
    else
    {
        if(y2 < y1)
        {
            t = x2; x2 = x1; x1 = t;
            t = y2; y2 = y1; y1 = t;
        }
        if(x2 > x1)
            yincr = 1;
        else
            yincr = -1;
        d = 2*dx - dy;
        Eincr = 2*dx;
        NEincr = 2*(dx - dy);
        //draw pixel in x1 y1
        video_mem[x1 + y1* h_res] = color;

        for(y1++; y1 <= y2; y1++)
        {
            if(d < 0)
                d += Eincr;
            else
            {
                d += NEincr;
                x1 += yincr;
            }
            //draw pixel in x1 y1
            video_mem[x1 + y1* h_res] = color;
        }

    }
    return 0;
}

int draw_point(char* video_mem, unsigned short x, unsigned short y, unsigned long color)
{
	video_mem[x + y* h_res] = color;
	return 0;
}

int print_pixmap(char* video_mem, char * pixmap, unsigned short x, unsigned short y, unsigned short x_size, unsigned short y_size)
{
	unsigned short i, j;
	for(j = 0; j < y_size; j++)
	{
		for(i = 0; i < x_size; i++)
			video_mem[(x+i)+((y+j) * h_res)] = pixmap[i + (j * x_size)];
	}

		return 0;
}


int delete_pixmap(char* video_mem, char * pixmap, unsigned short x, unsigned short y, unsigned short x_size,unsigned short y_size)
{
	unsigned short i, j;
		for(j = 0; j < y_size; j++)
		{
			for(i = 0; i < x_size; i++)
			{
				if(pixmap[i + (j * x_size)] != 0)
					video_mem[(x+i)+((y+j) * h_res)] = 0;
			}
		}
		return 0;
}


int clear_video_screen(char* video_mem)
{
	unsigned short y, x;
		for(y = 0; y <= v_res; y++)
		{
			for(x = 0; x <= h_res; x++)
				video_mem[x + y* h_res] = 0;
		}

			return 0;
}


