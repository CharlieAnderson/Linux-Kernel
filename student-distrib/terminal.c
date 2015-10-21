#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "syscalls.h"
#include "tty.h"
//available terminals
extern tty_t tty1;
extern tty_t tty2;
extern tty_t tty3;

extern tty_t ttys[3];

extern unsigned char* key_buffer;
/*
 * int32_t terminal_open(const uint8_t* filename)
 * Description: Helper function for pcb (open)
 * Inputs: A file's name
 * Outputs: Always 0
 */
int32_t terminal_open(const uint8_t* filename) {
	return 0;
}

/*
 * int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes)
 * Description: Helper function for pcb (read), copies key buffer to buf
 * Inputs: File descriptor, buffer, and length to read
 * Outputs: Number of bytes read
 */
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes) {
	sti();
	int32_t i = 0;
	while(!ttys[get_pcb_ptr()->tty].ent_flag);
	cli();
	//for up arrow press
	if(key_buffer[0] != '\n')
	{	//count the number of total past arguments
		if(ttys[tty_num].up_args < UP_MEM_SIZE-1)
			ttys[tty_num].up_args++;
	}	
	
	for (i = 0; i < nbytes; i++) {
		buf[i] = key_buffer[i];
		//for up arrow to store past arguments
			ttys[tty_num].args[ttys[tty_num].up_args][i] = key_buffer[i];

		if (buf[i] == '\n') {
			i++;
			break;
		}
	}
	//set index for up arrow so it gets the latest arg
	ttys[tty_num].up_args_index = ttys[tty_num].up_args+1;	
	//ttys[tty_num].up_press_count = 0;
	ttys[tty_num].ent_flag = 0;

	enter();
	clear_buffer();
	sti();
	return i;
}

/*
 * int32_t terminal_write(uint8_t* buf, int32_t nbytes)
 * Description: Helper function for pcb (write), copies key buffer to buf
 * Inputs: Buffer, length to read
 * Outputs: Length of the buffer
 */
int32_t terminal_write(uint8_t* buf, int32_t nbytes) {
	cli();
	/* This is for printing to the visible terminal that is being processed*/
	if(get_pcb_ptr()->tty == tty_num){
		uint32_t i = 0;
		clear_buffer();
		for (i = 0; i < nbytes; i++) {
			printf("%c", buf[i]);
		}
		clear_buffer();
		return i;
	}
	/* This is for writing to the back page when the terminal is not visible but is being processed*/
	else{
		uint32_t i = 0;
		for (i = 0; i < nbytes; i++) {
			putsudo(buf[i], (char*)ttys[get_pcb_ptr()->tty].vidmem, (uint32_t*)&ttys[get_pcb_ptr()->tty].x_pos, (uint32_t*)&ttys[get_pcb_ptr()->tty].y_pos);
		}
	}
	sti();
	return 0;
}

/*
 * int32_t terminal_close(int32_t fd)
 * Description: Helper function for pcb (close)
 * Inputs: File descriptor
 * Outputs: Always 0
 */
int32_t terminal_close(int32_t fd) {
	return 0;
}
