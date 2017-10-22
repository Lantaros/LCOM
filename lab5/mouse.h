#ifndef __MOUSE_H
#define __MOUSE_H


//typedef enum {INIT, COUNT, EXIT} state_t;
//typedef enum type {RDOWN, RUP, PSLOPE, NPSLOPE, SUCCESS} ev_type_t;


int mouse_subscribe_int(void);

int mouse_unsubscribe_int(void);

int mouse_packet_ignore(int irq_set_mouse);

int mouse_print_packet(unsigned char *packet); //prints in a user friendly way 1 packet of 3 bytes

int mouse_int_handler(unsigned char *packet_byte);

int mouse_read_packet(unsigned char *packet, int irq_set_mouse);

int write_command_mouse(unsigned long port, unsigned long cmd);

int check_readable(void);

int get_config(unsigned char *packet);

int print_config(unsigned char *packet);

////State Machine
//void process_event(ev_type_t *evt, unsigned char* packet, char signal);
//
//void state_machine(ev_type_t* evt, unsigned char *packet, int distance, unsigned int *distance_to_go, char signal);
//
//int mouse_positive_slope(unsigned char *packet,char signal);

#endif /* __TEST_4.H */
