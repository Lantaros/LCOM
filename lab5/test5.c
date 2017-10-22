#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <machine/int86.h>

#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "mouse.h"
#include "kbd.h"
#include <stdio.h>

#include "test5.h"
#include "video.h"
#include "video_gr.h"

static char *video_mem;		/* Process address to which VRAM is mapped */

int driver_receive(int num, message* numnum, int* numnumnum);

char *read_xpm(char *map[], int *wd, int *ht);


void *test_init(unsigned short mode, unsigned short delay)
{
	char *base_physical_address = (char *) vg_init(mode);
	if(base_physical_address == NULL)
	{
		printf("vg_init() ERROR\n");
		return NULL;
	}

	int r, seconds_left = delay;
	int ipc_status;
	message msg;

	int irq_set_timer = BIT(timer_subscribe_int());

	while (seconds_left)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				//hardware interrupt notification
				if (msg.NOTIFY_ARG & irq_set_timer)
				{
					timer_int_handler();
					if(getInterruptCounter() == 60)
					{
						seconds_left--;
						setInterruptCounter(0);
					}
				}
			}
		}
	}

	timer_unsubscribe_int();

	vg_exit();
	printf("Base physical address: 0x%08x\n", base_physical_address);

	return (void*)0xE0000000;



}

int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	//Test if it fits the screen
	if(x+size > H_RES || y+size > V_RES)
	{
		printf("The square coordinates are invalid: (%d, %d)\n", x, y);
		return 1;
	}

	if(color > 63)
	{
				printf("Invalid color index: %d\n", color);
				return 1;
	}


	//---------------------------------------
	video_mem =vg_init(0x105);
	if(video_mem == NULL)
			{
				printf("vg_init() ERROR\n");
			}


//	unsigned int i;    Test one line
//	for(i =0;i < 100;i++)
//	{
//		video_mem[i+(H_RES*y)] = (char) color;
//	}

		draw_square(video_mem, x, y, size, color);

		int r;
		int ipc_status;
		message msg;

		int irq_set_kbd = BIT(kbd_subscribe_int());
		unsigned long scancode;

		while (scancode != ESC_BRK)
		{
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
			{
				printf("driver_receive failed with: %d", r);
				continue;
			}

			if (is_ipc_notify(ipc_status)) //received notification
			{
				switch (_ENDPOINT_P(msg.m_source))
				{
				case HARDWARE:
					//hardware interrupt notification
					if (msg.NOTIFY_ARG & irq_set_kbd)
					{
						kbd_int_handler(&scancode);
					}
				}
			}
		}

		kbd_unsubscribe_int();

	vg_exit();

	return 0;

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color)
{

	video_mem =vg_init(0x105);
		if(video_mem == NULL)
				{
					printf("vg_init() ERROR\n");
				}



			draw_line_v2(video_mem, xi, yi, xf, yf, color);

			int r;
			int ipc_status;
			message msg;

			int irq_set_kbd = BIT(kbd_subscribe_int());
			unsigned long scancode;

			while (scancode != ESC_BRK)
			{
				if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
				{
					printf("driver_receive failed with: %d", r);
					continue;
				}

				if (is_ipc_notify(ipc_status)) //received notification
				{
					switch (_ENDPOINT_P(msg.m_source))
					{
					case HARDWARE:
						if (msg.NOTIFY_ARG & irq_set_kbd)
						{
							kbd_int_handler(&scancode);
						}
					}
				}
			}

			kbd_unsubscribe_int();

		vg_exit();

	return 0;

}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	int x_size = (int) xpm[0], y_size = (int) xpm[1];

//	if(xi + x_size > H_RES || yi + y_size >V_RES)
//	{
//		printf("test_xpm:: The pixmap won't fit in this screen resolution\n");
//		return 1;
//	}

	video_mem = vg_init(0x105);
	if (video_mem == NULL)
	{
		printf("vg_init() ERROR\n");
	}

	char * pixmap;


	pixmap = read_xpm(xpm, &x_size,  &y_size);


	print_pixmap(video_mem, pixmap, xi, yi, x_size, y_size);
	//delete_pixmap(video_mem, pixmap, xi, yi, x_size, y_size);


	int r;
	int ipc_status;
	message msg;

	int irq_set_kbd = BIT(kbd_subscribe_int());
	unsigned long scancode;

	while (scancode != ESC_BRK)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_kbd)
				{
					kbd_int_handler(&scancode);
				}
			}
		}
	}

	kbd_unsubscribe_int();

	vg_exit();
	return 0;

}



int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time)
{
	video_mem = vg_init(0x105);

	if (video_mem == NULL)
	{
		printf("vg_init() ERROR\n");
		return 1;
	}

	char * pixmap;
	int x_size = (int) xpm[0], y_size = (int) xpm[1];

	pixmap = read_xpm(xpm, &x_size, &y_size);

	int r, seconds_left = time;
	int ipc_status;
	message msg;
	unsigned long scancode;
	unsigned int  time_between_frames = time / delta, time_next_frame = time_between_frames;
	printf("%d", time_between_frames);

	int irq_set_kbd = BIT(kbd_subscribe_int());
	int irq_set_timer = BIT(timer_subscribe_int());

	while (scancode != ESC_BRK && delta)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
			{
				printf("driver_receive failed with: %d", r);
				continue;
			}

			if (is_ipc_notify(ipc_status)) //received notification
			{
				switch (_ENDPOINT_P(msg.m_source))
				{
				case HARDWARE:
					//hardware interrupt notification
					if (msg.NOTIFY_ARG & irq_set_timer)
					{
						timer_int_handler();
						if (getInterruptCounter() == 60)
						{
							time_next_frame--;
							setInterruptCounter(0);
						}
					}
					if (!time_next_frame)
					{
						time_next_frame = time_between_frames;
						delete_pixmap(video_mem, pixmap, xi, yi, x_size, y_size);
						print_pixmap(video_mem, pixmap, xi, yi, x_size, y_size);

						delta--;

						xi++;
						//yi++;
					}

					if (msg.NOTIFY_ARG & irq_set_kbd)
					{
						kbd_int_handler(&scancode);
					}
				}
			}
		}

kbd_unsubscribe_int();
timer_unsubscribe_int();

vg_exit();

return 0;
}



int test_controller()
{



	return 0;

}
