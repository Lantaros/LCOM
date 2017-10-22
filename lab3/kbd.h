#ifndef __KBD_H
#define __KBD_H



//Macros

//Prototypes
	//Read codes
	int kbd_int_handler(unsigned long *scan_code);

	int scan_code_print(unsigned long *scan_code, int flag);



	//Subscribes

	int kbd_subscribe_int(void);

	int kbd_unsubscribe_int(void);


	//Aux
	//int write_command(unsigned long instruction, unsigned short led);

	int activate_leds(unsigned int n_leds, unsigned short *leds);

	int write_command_leds(unsigned long leds);

	int write_command_kbc(unsigned long port, unsigned long cmd);


























#endif /*__KBD_H */
