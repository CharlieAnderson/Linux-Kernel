#ifndef TTY_H_
#define TTY_H_

#include "keyboard.h"
#include "lib.h"
#include "syscalls.h"
#include "types.h"
#include "paging.h"

#define VIDEO 0xB8000
//current terminal displayed
uint32_t tty_num;
//processes launched
uint32_t total_proc;
//flag for pcb_init to know if it's a terminal
volatile uint32_t new_term; 

//terminal struct
typedef struct 
{
	uint8_t* vidmem;
	uint32_t tty_id;
	//1 if current terminal
	uint32_t flag;
	//cursor position to switch back to
	uint32_t x_pos;
	uint32_t y_pos;
	//keep buffers separate between terminals
	unsigned char key_buf[BUFFER_SIZE];
	int32_t buf_idx;
	volatile int32_t ent_flag;
	volatile int32_t up_flag;
	int32_t up_args;
	uint32_t up_args_index;
	unsigned char args[UP_MEM_SIZE][BUFFER_SIZE];
	uint32_t cursor_index;
	uint32_t left_flag;
} tty_t;
//default values for all terminals
uint32_t tty_init();
//launch new terminal or call switch
uint32_t tty_launch(uint32_t id);
//switch to previously launched terminal
uint32_t tty_switch(tty_t* curr_term, tty_t* next_term);

#endif /* TTY_H_ */
