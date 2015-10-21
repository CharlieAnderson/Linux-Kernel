#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "lib.h"
#include "i8259.h"

/* Ports that each PIC sits on */
#define INDEX_RTC_PORT 	0x70
#define SLAVE_RTC_PORT  0x71
#define KEYBOARD_DATA	0x60

/* Release adds 0x80 to the value */
//scan codes
#define	LCTRL_PRESS		0x1D
#define	LCTRL_RELEASE	0x9D
#define	LSHIFT_PRESS	0x2A
#define	LSHIFT_RELEASE	0xAA
#define RSHIFT_PRESS	0x36
#define RSHIFT_RELEASE	0xB6
#define CAPS_PRESS		0x3A
#define CAPS_RELEASE	0xBA
#define	ENTER_PRESS		0x1C
#define BACKSPACE_PRESS	0x0E
#define L_PRESS			0x26
#define C_PRESS			0x2E
#define LALT_PRESS		0x38
#define LALT_RELEASE	0xB8
#define F1_PRESS		0x3B
#define F2_PRESS		0x3C
#define F3_PRESS		0x3D
#define UP_ARROW		0x48
#define DOWN_ARROW		0x50
#define TAB				0x0F
#define LEFT_ARROW		0x4B
#define RIGHT_ARROW		0x4D

#define UP_MEM_SIZE		50
#define BUFFER_SIZE		128
#define KEY_SIZE		60

//current position in key_buffer
volatile int buffer_index;

/* Initialize the keyboard */
void keyboard_init(void);

/* handle keyboard interrupts */
extern void keyboard_handler(void);

//print keys as you type
void key_echo(unsigned char key, unsigned char capsshift);

//empty key_buffer
void clear_buffer();

//init and flip flag
int32_t keyboard_open(const uint8_t* filename);
//flip flag off
int32_t keyboard_close(int32_t fd);
//for history
void up_arrow();
void clear_arg();
//using left arrow and backspacing mid argument
void shift_buff_left();
void left_arrow();

#endif /* _KEYBOARD_H */
