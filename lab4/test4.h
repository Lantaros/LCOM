#ifndef __TEST4_H
#define __TEST4_H

/** @defgroup test4 test4
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 * @brief To test packet reception via interrupts
 *
 * Displays the packets received from the mouse
 * Exits after receiving the number of packets received in argument
 *
 * @param cnt Number of packets to receive and display before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int test_packet(unsigned short cnt);

/*
Reads the packets from the PS/2 mouse using an interrupt handler
Prints them in a user friendly way

B1=0x8	B2=0x12	B3=0x14	LB=0	MB=0	RB=0	XOV=0	YOV=0	X=18	Y=20
B1=0x8	B2=0x12	B3=0x12	LB=0	MB=0	RB=0	XOV=0	YOV=0	X=18	Y=18
*/

/**
 * @brief To test handling of more than one interrupt
 *
 *  Similar test_packet() except that it
 *  should terminate if no packets are received for idle_time seconds
 *
 * @param idle_time Number of seconds without packets before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int test_async(unsigned short idle_time);

/*
Does the same as test_packets() but terminates if it receives no packets
		from the mouse for "idle_time" seconds
Must use Timer_0 interrupts
Timer_0 configuration cannot be changed
*/


/**
 * @brief To test reading and parsing of configuration
 *
 *  Reads mouse configuration and displays it in a human-friendly way
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int test_config(void);

/*
Test the code that displays the configuration of the mouse
Program exits after mouse configuration is displayed
*/


/**
 * @brief To test state machine implementation
 *
 *  Similar test_packet() except that it
 *  should terminate if user moves the mouse continuously
 *    with a positive slope
 *
 * @param length minimum length of movement (mouse units)
 *  in the y-direction. If positive, upwards movement, otherwise r
 *  downwards.
 *
 * @return Return 0 upon success and non-zero otherwise
 */


int test_gesture(short length);

/*
Does the same as test_packets(), but terminates if the user makes a continuous movement with a positive slope
		in the same direction with the mouse WHILE PRESSING THE RIGHT BUTTON.
		The minimum length of the movement in the y-direction is specified in the "length" argument
length > 0 = upwards movement
length < 0 = downwards movement
*/

#endif /* __TEST_4.H */

