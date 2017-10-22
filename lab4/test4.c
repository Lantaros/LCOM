#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>

#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "mouse.h"
#include "kbd.h"
#include <stdio.h>



int driver_receive(int num, message* numnum, int* numnumnum);

int test_packet(unsigned short cnt)
{

	int irq_set_mouse;
	unsigned char packet[3]; //Array for a 3 byte packet

	irq_set_mouse = BIT(mouse_subscribe_int());

	//Enable mouse data reporting
	if (write_command_mouse(IN_BUF, MOUSE_SET_STREAM_MODE) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	if (write_command_mouse(IN_BUF, MOUSE_ENABLE_DATA_REPORTING) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	//mouse_packet_ignore(irq_set_mouse); //Ignore one invalid packet
	//mouse_packet_ignore(irq_set_mouse); //Ignore one invalid packet

	while (cnt > 0)
	{
		mouse_read_packet(packet, irq_set_mouse);
		mouse_print_packet(packet);
		cnt--;
	}

	//Disable mouse data reporting
	if (write_command_mouse(IN_BUF, MOUSE_DISABLE_DATA_REPORTING) != OK)
	{
		printf("Error in mouse_int_handler: error in write_command_mouse\n");
	}

	clear_in_buf();

	mouse_unsubscribe_int();

	return 0;
}

int test_async(unsigned short idle_time)
{
	unsigned int time_after_mouse_int = 0;
	unsigned int count_packet = 0;

	int irq_set_timer, irq_set_mouse;

	unsigned char packet[3]; //Mouse Packet

	irq_set_timer = BIT(timer_subscribe_int());
	irq_set_mouse = BIT(mouse_subscribe_int());

	//Enable mouse data reporting
	if (write_command_mouse(IN_BUF, MOUSE_SET_STREAM_MODE) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	if (write_command_mouse(IN_BUF, MOUSE_ENABLE_DATA_REPORTING) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	//Interruption Cycle
	int r;
	int ipc_status;
	message msg;

	mouse_packet_ignore(irq_set_mouse);

	while (time_after_mouse_int < idle_time)
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
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & irq_set_timer)
				{
					timer_int_handler();
					if (getInterruptCounter() == 60)
					{
						setInterruptCounter(0);
						time_after_mouse_int++;

						printf("Since last mouse event %d of %d seconds.\n",
								time_after_mouse_int, idle_time);
					}
				}

				if (msg.NOTIFY_ARG & irq_set_mouse)
				{

					if (mouse_int_handler(&packet[count_packet]) != OK)
					{
						printf("mouse_int_handler() failed\n");
					}

					//Sync mouse data
					if (!(packet[0] & BIT(3)))
					{
						//printf("Invalid 1 Byte, 0x%x\n", packet[0]);
						count_packet = 0;
					}

					if (count_packet == 2) // Finished a whole packet
					{
						mouse_print_packet(packet);
						time_after_mouse_int = 0;
					}

					count_packet = (count_packet + 1) % 3; //Next iteration's index

				}
			}
		}

	}

	//Disable mouse data reporting
	if (write_command_mouse(IN_BUF, MOUSE_DISABLE_DATA_REPORTING) != OK)
	{
		printf("Error in mouse_int_handler: error in write_command_mouse\n");
	}

	clear_in_buf();

	mouse_unsubscribe_int();
	timer_unsubscribe_int();

	return 0;
}

int test_config(void)
{

	int irq_set_mouse;

	irq_set_mouse = BIT(mouse_subscribe_int());

	unsigned char config[3];

	if (get_config(config) != OK)
	{
		printf("Error in test_config(): error in get_config()\n");
		return 1;
	}
	if (print_config(config) != OK)
	{
		printf("Error in test_config(): error in print_config()\n");
		return 1;
	}

	mouse_unsubscribe_int();

	return 0;
}
//
//int test_gesture(short length)
//{
//	int irq_set_mouse;
//	unsigned char packet[3]; //Array for a 3 byte packet
//	unsigned int distance_to_go;
//	char sign;
//
//	//Length Signal
//	if(length >= 0)
//		sign = 0;
//	else
//		sign = 1;
//
//	irq_set_mouse = BIT(mouse_subscribe_int());
//
//	//Enable mouse data reporting
//	if (write_command_mouse(IN_BUF, MOUSE_SET_STREAM_MODE) != OK)
//	{
//		printf("Error in test_gesture: error in write_command\n");
//		return 1;
//	}
//
//	if (write_command_mouse(IN_BUF, MOUSE_ENABLE_DATA_REPORTING) != OK)
//	{
//		printf("Error in test_gesture: error in write_command\n");
//		return 1;
//	}
//
//
//	//State machine variables
//	ev_type_t evt;
//	static state_t state = INIT;
//	//-----------------------------------
//
//
//	while (state != EXIT)
//	{
//		mouse_read_packet(packet, irq_set_mouse);
//		mouse_print_packet(packet);
//
//		process_event(&evt, packet, sign);
//
//		state_machine(&evt, packet, length, &distance_to_go, sign);
//	}
//
//	//Disable mouse data reporting
//		if (write_command_mouse(IN_BUF, MOUSE_DISABLE_DATA_REPORTING) != OK)
//		{
//			printf("Error in test_gesture: error in write_command_mouse\n");
//		}
//
//		clear_in_buf();
//
//		mouse_unsubscribe_int();
//
//
//	return 0;
//}
