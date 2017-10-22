#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include "kbd.h"

static int kbd_hook_id = 1; //Global variable, accessible by all functions

int kbd_int_handler(unsigned long *scan_code) {

	//first byte of two byte scancode is usually 0xE0

	unsigned int timeout_tries = TIMEOUT;
	unsigned long stat_reg_val;

	while (timeout_tries != 0) {

		if (sys_inb(KBD_STAT_REG, &stat_reg_val) != OK) {
			printf("Error in read_kbd_scancode(): sys_inb() failed 0x%x\n",
					stat_reg_val);
			return 1;
		}

		if (!(stat_reg_val & BIT(0))) //bit 0 == 1 output buffer full - data available for reading
		{
			printf(
					"Error in kbd_interrupt_handler(): stat_reg_val invalid 0x%x\n",
					stat_reg_val);
			tickdelay(micros_to_ticks(DELAY_US));
		}

		if (stat_reg_val & PARITY_ERR) {
			printf("Error in kbd_int_handler: PARITY bit(7) active\n");
			return 1;
		}

		if (stat_reg_val & TIMEOUT_ERR) {
			printf("Error in kbd_int_handler: TIMEOUT bit(6) active\n");
			return 1;
		}

		else {
			if (sys_inb(KBD_BUF, scan_code) != OK) {
				printf("Error in read_kbd_scancode(): sys_inb() failed\n");
				return 1;
			}
			//return 0;
			break;//successfully read byte
		}

		timeout_tries--;

	}

	return 0;
}

int scan_code_print(unsigned long *scan_code, int flag) {

	if (flag) {
		if (BREAKCODE(*scan_code)) {
			printf("Breakcode: 0xe0%x\n", LSBMASK(*scan_code));
		} else
			//if(MAKECODE(*scan_code)){
			printf("Makecode: 0xe0%x\n", LSBMASK(*scan_code));
	} else {
		if (BREAKCODE(*scan_code)) {
			printf("Breakcode: 0x%x\n", LSBMASK(*scan_code));
		} else
			//if(MAKECODE(*scan_code)){
			printf("Makecode: 0x%x\n", LSBMASK(*scan_code));
	}

	return 0;
}

int kbd_subscribe_int(void) {

	//returns the bit number that will be set in msg.NOTIFY_ARG upon a TIMER_0 interrupt

	int hookCopy = kbd_hook_id;

	if (sys_irqsetpolicy(IRQ_KBD, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&kbd_hook_id) != OK) {
		printf("Error: timer_subscribe_int: sys_irqsetpolicy failed\n");
		return -1;	//Asked in documentation
	}

	if (sys_irqenable(&kbd_hook_id) != OK) {
		printf("Error: timer_subscribe_int: sys_irqsetpolicy failed\n");
		return -1;	//Asked in documentation
	}
	return hookCopy;
}

int kbd_unsubscribe_int() {

	if (sys_irqdisable(&kbd_hook_id) != OK) {
		printf("Error: timer_unsubscribe_int: sys_irqdisable failed\n");
		return 1;
	}

	if (sys_irqrmpolicy(&kbd_hook_id) != OK) {
		printf("Error: timer_unsubscribe_int: sys_irqrmpolicy failed\n");
		return 1;
	}
	return 0;
}

//FUNCTION FOR POSSIBLE FUTURE USE
//int write_command(unsigned long instruction,unsigned short led)
//{
//	unsigned long stat_val, kbd_response;
//
//	do if(sys_inb(KBD_STAT_REG, &stat_val) != OK){
//		printf("Error in write_command: error in sys_inb(): checking if stat_reg is empty\n");
//		return 1;
//	}
//	while(stat_val & IN_BUF_FULL);
//
//	if(sys_outb(IN_BUF, instruction) != OK){
//		printf("Error in write_command: error in sys_outb(): sending command to input buffer\n");
//		return 1;
//	}
//
//	if(sys_inb(OUT_BUF, &kbd_response) != OK){
//		printf("Error in write_command: error in sys_inb: getting response from output buffer\n");
//		return 1;
//	}
//
//	if(kbd_response == ACK)
//	{
//		if(sys_outb(IN_BUF, (unsigned long) BIT(led)) != OK){
//			printf("Error in write_command: error in sys_outb: processing ACK response\n");
//			return 1;
//		}
//
//		if(sys_inb(OUT_BUF,&kbd_response) != OK){
//			printf("Error in write_command: error in sys_inb: processing ACK response\n");
//			return 1;
//		}
//
//		switch(kbd_response)
//		{
//		case ACK:
//			break;
//		case RESEND:
//			if(sys_outb(IN_BUF, (unsigned long) BIT(led)) != OK){
//				printf("Error in write_command: error in sys_outb: processing RESEND response\n");
//				return 1;
//			}
//			break;
//		case ERROR:
//			printf("write_command() terminated with an error\n");
//			break;
//
//		}
//
//
//	}
//
//	return 0;
//
//
//}

int write_command_kbc(unsigned long port, unsigned long cmd) {

	unsigned long stat_val, kbd_response;

	do
		if (sys_inb(KBD_STAT_REG, &stat_val) != OK) {
			printf(
					"Error in write_command_kbc: error in sys_inb(): checking if stat_reg is empty\n");
			return 1;
		} while (stat_val & IN_BUF_FULL);

	sys_outb(port, cmd);

	if (sys_inb(OUT_BUF, &kbd_response) != OK) {
		printf(
				"Error in write_command_kbc: error in sys_inb: processing ACK response\n");
		return 1;
	}

	switch (kbd_response) {
	case ACK:
		break;
	case RESEND:
		if (sys_outb(port, cmd) != OK) {
			printf(
					"Error in write_command_kbc: error in sys_outb: processing RESEND response\n");
			return 1;
		}
		break;
	case ERROR:
		printf("write_command_kbc() terminated with an error\n");
		break;

	}
	return 0;
}

int write_command_leds(unsigned long leds) {

	if(write_command_kbc(IN_BUF, SWITCH_LEDS) != OK){
		printf("Error in write_command_leds()\n");
		return 1;
	}

		if (write_command_kbc(IN_BUF, leds) != OK) {
			printf(	"Error in write_command_leds: error in sys_outb: processing ACK response\n");
			return 1;

	}

	return 0;

}

int activate_leds(unsigned int n_leds, unsigned short *leds) {

	int driver_receive(int num, message* numnum, int* numnumnum);

	int ipc_status;
	message msg;
	int r;
	int irq_set;
	irq_set = BIT(timer_subscribe_int());
	int i;
	unsigned int leds_bits = 0;

	//Reset leds
	write_command_leds(0);

	for (i = 0; i < n_leds;) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with: %d", r);
			continue;

		}

		//printf("cenas1 number: %d\n", i);

		if (is_ipc_notify(ipc_status)) { //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) { //subscribed interrupt

					timer_int_handler();
					//printf("cenas2 number: %d\n", i);
					//printf("timer: %d\n", getInterruptCounter());

					if (getInterruptCounter() == 60) {
						setInterruptCounter(0);

						//printf("cenas3 number: %d\n", i);

						switch (leds[i]) {
						case 0:
							printf("\nScroll Lock toggled\n");
							break;
						case 1:
							printf("\nNumLock toggled\n");
							break;
						case 2:
							printf("\nCaps Lock toggled\n");
							break;
						default:
							printf("activate_led: invalid parameter %d.\n",
									leds[i]);
							return -1;
						}

						leds_bits ^= BIT(leds[i]);

						if (write_command_leds(leds_bits) != OK) {
							printf("Error in activate_led");
							return 1;
						}

						i++;

					}
				}
				break;

			default:
				break; //no other notifications expected: do nothing
			}
		}
	}
	timer_unsubscribe_int();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------
