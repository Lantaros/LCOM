#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "mouse.h"
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include "kbd.h"



int driver_receive(int num, message* numnum, int* numnumnum);

int mouse_hook_id = 12;

int mouse_subscribe_int(void)
{

	//returns the bit number that will be set in msg.NOTIFY_ARG upon a Mouse interrupt

	int hookCopy = mouse_hook_id;

	if (sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&mouse_hook_id) != OK)
	{
		printf("Error: mouse_subscribe_int: sys_irqsetpolicy failed\n");
		return -1;	//Asked in documentation
	}

	if (sys_irqenable(&mouse_hook_id) != OK)
	{
		printf("Error: mouse_subscribe_int: sys_irqsetpolicy failed\n");
		return -1;	//Asked in documentation
	}
	return hookCopy;
}

int mouse_unsubscribe_int(void)
{

	if (sys_irqdisable(&mouse_hook_id) != OK)
	{
		printf("Error: mouse_unsubscribe_int: sys_irqdisable failed\n");
		return 1;
	}

	if (sys_irqrmpolicy(&mouse_hook_id) != OK)
	{
		printf("Error: mouse_unsubscribe_int: sys_irqrmpolicy failed\n");
		return 1;
	}
	return 0;
}

int mouse_packet_ignore(int irq_set_mouse)
{
	int r, i;
	int ipc_status;
	message msg;
	unsigned long trash;

	for (i = 0; i < 3; i++)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with: %d", r);
			return 1;
		}

		if (is_ipc_notify(ipc_status)) //received notification
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & irq_set_mouse)
				{
					sys_inb(IN_BUF, &trash);
					//printf("byte nr %d processed\n", i);

				}
			}
		}
	}

	//printf("reached end of packet_ignore\n");
	return 0;
}

int mouse_int_handler(unsigned char *packet_byte)
{
	unsigned int timeout_tries = TIMEOUT;
	unsigned long stat_reg_val, byte = 0;

	while (timeout_tries)
	{

		if (sys_inb(KBD_STAT_REG, &stat_reg_val) != OK)
		{
			printf("Error in mouse_int_handler(): sys_inb() failed 0x%x\n",
					stat_reg_val);
			return 1;
		}

		if (!(stat_reg_val & (BIT(0) | BIT(5)))) //bit 0 == 1 output buffer full, bit 5 (AUX) - data available for reading
		{
//			printf("Error in mouse_int_handler(): stat_reg_val invalid 0x%x\n",
//					stat_reg_val);
			tickdelay(micros_to_ticks(DELAY_US));
		}

		else
		{
			if (sys_inb(OUT_BUF, &byte) != OK)
			{
				printf("Error in mouse_int_handler(): nothing to read\n");
				return 1;
			}

			*packet_byte = (unsigned char) byte;
			return 0; //successfully read byte
		}

		if (stat_reg_val & TIMEOUT_ERR)
		{
			printf("Error in mouse_int_handler(): TIMEOUT bit(6) active\n");
			return 1;
		}

		timeout_tries--;
	}

	printf("\nExceeded tries in mouse_handler\n");
	return 1;
}

int mouse_print_packet(unsigned char *packet)
{
	short x, y;

	printf("B1=0x%x", packet[0]);
	printf("  B2=0x%x", packet[1]);
	printf("  B3=0x%x", packet[2]);
	printf("  LB=%d", packet[0] & BIT(0));
	printf("  MB=%d", (packet[0] & BIT(2)) >> 2);
	printf("  RB=%d", (packet[0] & BIT(1)) >> 1);
	printf("  XOV=%d", (packet[0] & BIT(6)) >> 6);
	printf("  YOV=%d", (packet[0] & BIT(7)) >> 7);
	printf("  XS=%d", (packet[0] & BIT(4)) >> 4);
	printf("  YS=%d\n\n", (packet[0] & BIT(5)) >> 5);

	if (packet[0] & BIT(4)) // X
		x = 0xFF00 | packet[1];
	else
		x = packet[1];

	if (packet[0] & BIT(5)) // Y
		y = 0xFF00 | packet[2];
	else
		y = packet[2];

	printf("   deltaX= %d ", x);
	printf("deltaY= %d\n\n", y);

	return 0;
}

int mouse_read_packet(unsigned char *packet, int irq_set_mouse)
{
	int r;
	int ipc_status;
	message msg;
	//unsigned long trash;

	unsigned int count_packet = 0;

	//sys_inb(OUT_BUF, &trash);

	while (count_packet < 3)
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
				if (msg.NOTIFY_ARG & irq_set_mouse)
				{
					if (mouse_int_handler(&packet[count_packet]) != OK)
					{
						printf("mouse_int_handler() failed\n");
					}
					count_packet++;

					//Sync mouse data
					if (!(packet[0] & BIT(3)))
					{
						//printf("Invalid 1 Byte, 0x%x\n", packet[0]);
						count_packet = 0;
					}

				}
			}
		}
	}

	return 0;

}

int write_command_mouse(unsigned long port, unsigned long cmd)
{
	if (write_command_kbc(KBD_STAT_REG, MOUSE_CMD) != OK)
	{
		printf("Error in write_command_mouse: error in write_command_kbc\n");
	}

	unsigned long mouse_response;

//	if (clear_in_buf() != OK)
//	{
//		printf("Error in write_command_mouse: error in clear_in_buf()");
//	}

	if (sys_outb(port, cmd) != OK)
	{
		printf("Error in write_command_mouse: error in sys_outb\n");
	}

	if (sys_inb(OUT_BUF, &mouse_response) != OK)
	{
		printf(
				"Error in write_command_mouse: error in sys_inb: processing mouse response\n");
		return 1;
	}

	switch (mouse_response)
	{
	case ACK:
		break;
	case NACK:
		printf("write_command_mouse: NACK\n");

		if (sys_outb(port, cmd) != OK)
		{

			printf("Error in write_command_mouse: error in sys_outb\n");
		}

		if (sys_inb(OUT_BUF, &mouse_response) != OK)
		{
			printf(
					"Error in write_command_mouse: error in sys_inb: processing mouse response\n");
			return 1;
		}
	case ERROR:
		printf("write_command_mouse: ERROR\n");
		return 1;
		break;
	}

	return 0;
}

int get_config(unsigned char *packet) //Subb pedir e Ler
{

	unsigned int index = 0;
	unsigned long read = 0;
	//mouse_read_packet(packet, irq_set_mouse);

	if (write_command_mouse(IN_BUF, MOUSE_ENABLE_DATA_REPORTING) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	if (write_command_mouse(IN_BUF, MOUSE_STATUS_REQUEST) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	while (index < 3)
	{

		if (sys_inb(OUT_BUF, &read) != OK)
		{
			printf("Error in get_config(): error in sys_inb()\n");
			return 1;
		}

		packet[index] = (unsigned char) read;
		read = 0;
		index++;
	}

	if (write_command_mouse(IN_BUF, MOUSE_DISABLE_DATA_REPORTING) != OK)
	{
		printf("Error in mouse_read_packet: error in write_command\n");
		return 1;
	}

	printf("reached end of get_config() \n");
	return 0;
}

int print_config(unsigned char *packet)
{
	//	Bit 7	   Bit 6  Bit 5	 Bit 4	  Bit 3	Bit 2	Bit 1	Bit 0
	//Byte 1	Always 0   Mode	  Enable Scaling  Always 0	Left Btn	Middle Btn	Right Btn
	//Byte 2	Resolution
	//Byte 3	Sample Rate

	printf(" .:MOUSE CONFIGURATION:.\n\n");

	printf("    B1=0x%x\n", packet[0] & 0xff);
	printf("    B2=0x%x\n", packet[1] & 0xff);
	printf("    B3=0x%x\n\n", packet[2] & 0xff);

	printf(" Buttons\n\n");

	printf("   LB=%d", packet[0] & BIT(0));
	printf("   MB=%d", (packet[0] & BIT(2)) >> 2);
	printf("   RB=%d\n\n", (packet[0] & BIT(1)) >> 1);

	printf(" MODE\n\n");
	if (packet[0] & BIT(6))
		printf("   REMOTE MODE\n\n");
	else
		printf("   STREAM MODE\n\n");

	if (packet[0] & BIT(5))
		printf(" DATA REPORTING ENABLED\n\n");
	else
		printf(" DATA REPORTING DISABLED\n\n");

	if (packet[0] & BIT(4))
		printf(" SCALLING: 2:1\n");
	else
		printf(" SCALLING: 1:1\n");

	printf(" SAMPLE RATE: %d\n\n", packet[2]);
	printf(" RESOLUTION\n\n   ");
	switch (packet[1])
	{
	case 0:
		printf("1 count/mm\n");
		break;
	case 1:
		printf("2 count/mm\n");
		break;
	case 2:
		printf("4 count/mm\n");
		break;
	case 3:
		printf("8 count/mm\n");
		break;
	}

	return 0;
}

int check_readable(void)
{
	unsigned long stat_reg_val = 0;
	unsigned int tries = TIMEOUT;

	do
	{
		if (sys_inb(KBD_STAT_REG, &stat_reg_val) != OK)
		{
			printf("Error in mouse_int_handler(): sys_inb() failed 0x%x\n",
					stat_reg_val);
			return 1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		tries--;
	} while (!(stat_reg_val & BIT(0)) || tries);

	if (!tries)
	{
		printf("check_readable(): IN_BUF empty\n");
		return 1;
	}

	return 0;
}

//int process_event(ev_type_t *evt, unsigned char* packet, char signal)
//{
//	switch (state)
//	{
//	case INIT:
//		if (packet[0] & BIT(1))
//			evt->type = RDOWN;
//		break;
//
//	case COUNT:
//
//		if (distance_to_go <= 0)
//			evt->type = SUCCEED;
//
//		if ((packet[0] & BIT(1))) //IF RDOWN
//		{
//
//			if (mouse_positive_slope(packet, signal) != -1)
//				evt->type = PSLOPE;
//			else
//				evt->type = NPSLOPE;
//		}
//		else
//			evt->type = RUP;
//
//	}
//
//	return;
//
//}
//
//void state_machine(ev_type_t* evt, unsigned char *packet, int distance, unsigned int *distance_to_go, char signal)
//{
//
//	switch (state)
//	{
//	case INIT:
//		*distance_to_go = abs(distance);
//		if (evt->type == RDOWN)
//			state = COUNT;
//		break;
//
//	case COUNT:
//		if (evt->type == PSLOPE)
//		{
//			state = COUNT;
//			*distance_to_go -= mouse_positive_slope(packet, signal);
//			break;
//		}
//
//
//		if (evt->type == (RUP || NPSLOPE)
//			state = INIT;
//
//		break;
//
//		if (evt->type == SUCCESS)
//			state = EXIT;
//			break;
//	}
//
//	return;
//}
//
//int mouse_positive_slope(unsigned char *packet,char signal)
//{
//	if(packet[0] & (BIT(4) || BIT(5) && signal))
//		{
//		if(packet[0] & BIT(5)) //Y SIGN
//			return 0xFF00 | packet[2];
//		else
//			return packet[2];
//
//		}
//	else
//		return -1;
//}
