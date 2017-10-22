#include "timer.h"
#include "i8254.h"
#include "i8042.h"
#include "kbd.h"
#include "test3.h"


unsigned int ass_handler();
int driver_receive(int num, message* numnum, int* numnumnum);


int kbd_test_scan(unsigned short asmm) {

	//	1st - subscribes the KBC interrupts.
	//				2nd - IH receives scancodes from the keyboard, prints them on the console
	//					  indicating whether or not the scancode is make or break code.
	//			Should output this:
	//				Makecode: 0x01
	//				Breakcode: 0x81

	//function exits when user presses Esc key

	int r;
	int ipc_status;
	message msg;
	int irq_set_kbc;
	unsigned long scan_code = 0;
	int two_byte_flag = 0; // 1 equals twoByte ScanCode

	if (asmm != 0 && asmm != 1) {
		printf("kbd_test_scan() failed\n");
		printf("invalid argument: %lu\n", asmm);
		return 1;
	}

	irq_set_kbc = BIT(kbd_subscribe_int());


	while (scan_code != ESC_BRK) {

		//printf("\nScanCode: 0x%x\n", LSBMASK(scan_code));

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) //received notification
				{
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & irq_set_kbc) {

					if (asmm == 0) {
						if (kbd_int_handler(&scan_code) != OK) {
							printf(
									"Error in test_kbd_scan(): int_handler() failed\n");
							break;
						}
					} else {
						if (asmm == 1)
							scan_code = ass_handler();
						if(!scan_code)
						{
							printf("Error in kbd_int_handler: assembly: PARITY/TIMEOUT error\n");
							return 1;
						}
					}

					if (LSBMASK(scan_code) == TWO_BYTE_SCANCODE)
						two_byte_flag = 1;

					else {
						if (scan_code_print(&scan_code, two_byte_flag) != OK) {
							printf("\nInvalid ScanCode 0x%x\n", scan_code);
							break; //Servere Malfunction
						}
						two_byte_flag = 0;
					}
				}
			}
		}
	}
	kbd_unsubscribe_int();
	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {

	if (activate_leds(n, leds) != OK)
		printf("activate_leds() failed.\n");

	free(leds);
	return 0; //ou 1??? D:

}

int kbd_test_timed_scan(unsigned short n) {
	int r;
	int ipc_status;
	message msg;
	int irq_set_kbc, irq_set_timer;
	unsigned int elapsed_time = 0;
	unsigned long scan_code = 0;
	int two_byte_flag = 0; // 1 equals twoByte ScanCode

	irq_set_kbc = BIT(kbd_subscribe_int());
	irq_set_timer = BIT(timer_subscribe_int());

	while ((scan_code != ESC_BRK) && (elapsed_time < n)) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) //received notification
				{
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
				if (msg.NOTIFY_ARG & irq_set_timer) {
					timer_int_handler();
					if (getInterruptCounter() == 60) {
						setInterruptCounter(0);
						elapsed_time++;
						printf("Increased %d seconds timer\n", elapsed_time);
					}
				}
				if (msg.NOTIFY_ARG & irq_set_kbc) {

					if (kbd_int_handler(&scan_code) != OK) {
						printf(
								"Error in test_kbd_scan(): int_handler() failed\n");
						break;
					}

					if (LSBMASK(scan_code) == TWO_BYTE_SCANCODE)
						two_byte_flag = 1;

					else {
						if (scan_code_print(&scan_code, two_byte_flag) != OK) {
							printf("\nInvalid ScanCode 0x%x\n", scan_code);
							return 1; //Servere Malfunction
						}
						two_byte_flag = 0;
						setInterruptCounter(0);
						elapsed_time = 0;
					}
				}

			}
		}
	}

	timer_unsubscribe_int();
	kbd_unsubscribe_int();

	return 0;

}

