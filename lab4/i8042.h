#ifndef __T8042_H
#define __I8042_H

#define OK 0
#define TIMEOUT 4 //number of times kbd waits to receive message
#define DELAY_US 20000


#define IRQ_KBD 1
#define IRQ_MOUSE 12

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
//MOUSE
#define NACK  0xFE

//Keyboard Commands
#define RESET_KBD 0xFF
#define DEFAULT_ENABLE_KBD 0xF6
#define DISABLE_KBD 0xF5
#define CLEAR_BUFFER 0xF4
#define CHANGE_KBD_REPETITION 0xF3
#define SWITCH_LEDS 0xED

//Mouse Commands
#define MOUSE_IRQ 12
#define MOUSE_ENABLE 0xA8
#define MOUSE_BAT 0xAA
#define MOUSE_DEVICE_ID 0x00
#define MOUSE_ERROR 0xFC
#define MOUSE_SET_RESOLUTION  0xE8
#define MOUSE_SET_SCALLING 0xE7
#define MOUSE_SET_SAMPLE_RATE 0xF3
#define MOUSE_SET_STREAM_MODE 0xEA
#define MOUSE_SET_REMOTE_MODE 0xF0
#define MOUSE_RESET_WRAP_MODE 0xEC
#define MOUSE_RESET 0xFF
#define MOUSE_NON_RESEND 0xFE
#define MOUSE_ENABLE_READ_DATA 0xEB
#define MOUSE_ENABLE_DATA_REPORTING 0xF4
#define MOUSE_DISABLE_DATA_REPORTING 0xF5
#define MOUSE_STATUS_REQUEST 0xE9
#define MOUSE_CMD 0xD4




















#endif /* __I8042_H */
