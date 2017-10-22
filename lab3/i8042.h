#ifndef __T8042_H
#define __I8042_H

#define OK 0
#define TIMEOUT 4 //number of times kbd waits to receive message
#define DELAY_US 20000


#define IRQ_KBD 1

//Scan Codes
#define ESC_MK 0x01
#define ESC_BRK 0x81
#define TWO_BYTE_SCANCODE 0xE0
#define OUT_BUF_FULL 0x1
#define IN_BUF_FULL 0x2
#define PARITY_ERR 0x80 //BIT 7
#define TIMEOUT_ERR 0x40 //BIT 6

#define LSBMASK(nr) (nr & 0x000000FF)
#define BREAKCODE(nr) (nr & BIT(7))
#define MAKECODE(nr) (!(nr & BIT(7)))


//Ports
#define KBD_STAT_REG 0x64
#define KBD_BUF 0x60
#define OUT_BUF 0x60
#define IN_BUF 0x60



//C@KBD possible answers
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC

//Keyboard Commands
#define RESET_KBD 0xFF
#define DEFAULT_ENABLE_KBD 0xF6
#define DISABLE_KBD 0xF5
#define CLEAR_BUFFER 0xF4
#define CHANGE_KBD_REPETITION 0xF3
#define SWITCH_LEDS 0xED





















#endif /* __I8042_H */
