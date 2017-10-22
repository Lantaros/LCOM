//NOTES
/*
Minix-3 device drivers execute at user level.
User level programs are not allowed to execute I/O sensitive operations such as:
	IN, OUT, STI, CLI, etc.
If interrupt handler issues any of these instructions, the kernel will kill it

IA-32 provides a mechanism that allows to execute those kinds of operations at user level.
	Use: IOPL (I/O Priviledged Level) field of the Extended Flags (EF) register.
	This field specifies the minimum current priviledge level of a process when it executes those
	operations.

Minix-3 offers a kernel call: sys_enable_iop() -> allows the IOPL of a process to be set to a value
	such that it is able to execute I/O sensitive operations, even though it executes at user level.
	Use the following code:

	set_startup();
	Enable IO-sennsitive operations for ourselves
	sys_enable_iop(SELF);




use this in order to wait for KBC response or time outs:
#include <minix/sysutil.h>

#define DELAY_US    20000

tickdelay(micros_to_ticks(DELAY_US);
*/



//FUNCTIONS TO IMPLEMENT



#ifndef __TEST3_H
#define __TEST3_H

#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>



/** @defgroup test3 test3
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 * @brief To test reading of scancode via KBD interrupts
 *
 * Displays the scancodes received from the keyboard
 * Exits upon release of the ESC key
 *
 * @param ass Which IH to use: if 0, in C, otherwise in assembly
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_scan(unsigned short asmm);

/**
 * @brief To test keyboard commands
 *
 * Toggles LED indicators, one per second, according to the
 *  arguments
 *
 * @param n number of elements of the second argument
 * @param leds array with the LEDs to toggle (Ranges from 0 t0 2)
 *
 * @return Return 0 upon success and non-zero otherwise
 */

/*
the purpose of this function is to test if the code is able to read scancodes from the
								   KBC using an interrupt handler (IH).
							if asm == 0: uses Interrupt Handler written in C
							if asm != 0: uses Interrupt Handler written in Assembly

			1st - subscribes the KBC interrupts.
			2nd - IH receives scancodes from the keyboard, prints them on the console
				  indicating whether or not the scancode is make or break code.
		Should output this:
			Makecode: 0x01
			Breakcode: 0x81

	The function exists when the user presses the Esc key (break code = 0x81).
	NOTE: it cancels the subscription of the KBC interrupt before terminating

	IMP: -variables used to pass information between the interrupt handler and the program should be
		 defined in the Assembly file
		 -if the Assembly file uses C preprocessor directives like #include's and #define's,
		 file must have the .S extension
*/
int kbd_test_leds(unsigned short n, unsigned short *leds);

/**
 * @brief To test handling of more than one interrupt
 *
 *  Similar to kbd_test_scan() except that it
 *  should terminate also if no scancodes are received for n seconds
 *
 * @param n Number of seconds without scancodes before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

/*
n = number of elements of toggle
														 toggle = sequence of instructions
		that specify which indicator LEDs should be toggled.
	Elements of toggle can take the following values:
				0 -> scroll lock
				1 -> numeric lock
				2 -> caps lock

		The function processes each element of toggle array once per second

		To measure time Timer_0 interrupts can be used
*/
int kbd_test_timed_scan(unsigned short n);
/*
handles interrupts form more than one device
									   similar to kbd_test_scan(), since it also prints on the
		console the scancodes received from the keyboard (use C version of the Interrupt Handler)
					However, this function exits both when the user presses the Esc key and if it
				doesnt receive a scancode for "n" seconds

				Use Timer_0 interrupts; dont change its configuration, only subscribe its interrupts like in timer_test_square()
*/

#endif /* __TEST3_H */
