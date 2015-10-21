#include "keyboard.h"
#include "lib.h"
#include "syscalls.h"
#include "terminal.h"
#include "tty.h"
#include "pit.h"
#include "filesys.h"

extern tty_t tty1;
extern tty_t tty2;
extern tty_t tty3;

extern tty_t ttys[3];

//fill with key presses
unsigned char* key_buffer;

static char keys[4][KEY_SIZE] = {
	/* No caps/shift */
	{ '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	  '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	  '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	  '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*',
	  '\0', ' ', '\0'},
	/* Caps lock */
	{ '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	  '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
	  '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
	  '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '\0', '*',
	  '\0', ' ', '\0'},
	/* Shift */
	{ '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	  '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
	  '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	  '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '*',
	  '\0', ' ', '\0'},
	/* Caps and shift */
	{ '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	  '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}',
	  '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '"', '~',
	  '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '*',
	  '\0', ' ', '\0'}
};

static unsigned char capsshift = 0;		// 0 = regular, 1 = caps, 2 = shift, 3 = both
static unsigned char ctrl = 0;		// 0 = not pressed, 1 = pressed
static unsigned char alt = 0;	// 0 = not pressed, 1 = pressed
static int file_open_flag = 0;

/*
 * void keyboard_init()
 * Description: Enables the keyboard interrupt
 * Inputs: None
 * Outputs: None
 */
void keyboard_init()
{	
	enable_irq(1);
	buffer_index = 0;
}

/*
 * void keyboard_handler()
 * Description: Handler for the keyboard entries depending on the key
 * Inputs: None
 * Outputs: None
 */
void keyboard_handler()
{
	cli();
	unsigned char c = 0;
	dentry_t dentry;
	int32_t pos = buffer_index;
	// Gets the keyboard data and calls a switch case based on the data
	c = inb(KEYBOARD_DATA);
	tty_t* term = &ttys[tty_num];
	switch (c) {
		case LCTRL_PRESS: 
			ctrl = 1;
			break;

		case LCTRL_RELEASE:
			ctrl = 0;
			break;

		case ENTER_PRESS:
			if (buffer_index < BUFFER_SIZE) {
				key_buffer[buffer_index] = '\n';
			}
			ttys[tty_num].ent_flag = 1;
			break;
			
		case BACKSPACE_PRESS:
			if(buffer_index > term->cursor_index)
			{	backspace();
				shift_buff_left();
				left_arrow();
				buffer_index--;
			}	
			else if (buffer_index > 0 && term->cursor_index > 0) {
				backspace();
				buffer_index -= 1;
				term->cursor_index--;
			}
			break;

		case LSHIFT_PRESS:
			if (capsshift == 0) {
				capsshift = 2;
			} else if (capsshift == 1) {
				capsshift = 3;
			}
			break;

		case LSHIFT_RELEASE:
			if (capsshift == 2) {
				capsshift = 0;
			} else if (capsshift == 3) {
				capsshift = 1;
			}
			break;

		case RSHIFT_PRESS:
			if (capsshift == 0) {
				capsshift = 2;
			} else if (capsshift == 1) {
				capsshift = 3;
			}
			break;

		case RSHIFT_RELEASE:
			if (capsshift == 2) {
				capsshift = 0;
			} else if (capsshift == 3) {
				capsshift = 1;
			}
			break;

		case CAPS_PRESS:
			if (capsshift == 0) {
				capsshift = 1;
			} else if (capsshift == 1) {
				capsshift = 0;
			} else if (capsshift == 2) {
				capsshift = 3;
			} else if (capsshift == 3) {
				capsshift = 2;
			}
			break;

		case CAPS_RELEASE:
			break;
			
		case LALT_PRESS: 
			alt = 1;
			break;

		case LALT_RELEASE:
			alt = 0;
			break;

		case UP_ARROW:
			if(term->up_args < 0 || term->up_args + 1 < term->up_args_index || term->up_args_index < 1)
				break;
			term->up_args_index--;
			term->up_flag = 1;
			up_arrow();
			break;	

		case DOWN_ARROW:
			if(term->up_args > UP_MEM_SIZE-1 || term->up_args < term->up_args_index || term->up_args_index == UP_MEM_SIZE-1)
				break;
			term->up_flag = 1;
			term->up_args_index++;
			up_arrow();
			break;	

		case TAB:
			while(key_buffer[pos] != ' ' && pos >= 0){
				pos--;
			}
			unsigned char* temp[10];
			int i;
			for (i = 0; i < 10; ++i)
			{
				temp[i] = '\0';
			}
			strcpy((int8_t*)temp, (int8_t*)&key_buffer[++pos]);
			if(lookup_name((uint8_t*)temp,&dentry) == 0){
				strcpy((int8_t*)&key_buffer[pos], (int8_t*)dentry.name);
				for (i = 0; i < buffer_index; ++i)
				{		
					backspace();
				}
				buffer_index = strlen((int8_t*)key_buffer);
				
				for (i = 0; i < strlen((int8_t*)key_buffer); i++) {
					printf("%c", key_buffer[i]);
				}
			}
			break;
			
		case LEFT_ARROW:
			left_arrow();	
			break;

		case RIGHT_ARROW:
			if(term->cursor_index < buffer_index)
			{
				term->cursor_index++;
				uint32_t temp1 = 0;
				uint32_t temp2= 0;
				uint32_t* x_pos = &temp1;
				uint32_t* y_pos = &temp2;
				get_cursor(x_pos, y_pos);
				set_cursor(*x_pos + 1, *y_pos);
				set_cursor_position();
			}	
			break;

		default:
			key_echo(c, capsshift);
			break;
	}
	send_eoi(1);
	sti();
}

/*
 * void key_echo(unsigned char key, unsigned char capsshift)
 * Description: Prints the key from the keycode to the screen
 * Inputs: Key, value depending on caps and shift
 * Outputs: None
 */
void key_echo(unsigned char key, unsigned char capsshift)
{

	if (key == L_PRESS && ctrl == 1) {
		clear();
		clear_buffer();
		return;
	}

	if (key == F1_PRESS && alt == 1) {
		send_eoi(1);
		tty_launch(0);
		return;
	}

	if (key == F2_PRESS && alt == 1) {
		send_eoi(1);
		tty_launch(1);
		return;
	}

	if (key == F3_PRESS && alt == 1) {
		send_eoi(1);
		tty_launch(2);
		return;
	}

	//manually exit praogram by exception
	if(ctrl == 1 && key == C_PRESS)
	{
		send_eoi(1);
		clear();
	    cli();
		halt_exception = 1;
		halt(0);
	}

	if(ttys[tty_num].up_flag || ttys[tty_num].left_flag)
	{
		if(buffer_index >= ttys[tty_num].cursor_index && !ttys[tty_num].left_flag)
			buffer_index++;
		if(!ttys[tty_num].left_flag)
			ttys[tty_num].cursor_index++;
		printf("%c", key);
	}

	else if ((key < (KEY_SIZE - 1)) && (buffer_index < BUFFER_SIZE)) {
		char c = keys[capsshift][key];
		key_buffer[ttys[tty_num].cursor_index] = c;
		if(buffer_index == ttys[tty_num].cursor_index)
			buffer_index++;
		ttys[tty_num].cursor_index++;
		printf("%c", c);
	}
	return;
}

/*
 * void clear_buffer()
 * Description: Clears the keyboard, called with terminal actions
 * Inputs: None
 * Outputs: None
 */
void clear_buffer() {
	int i = 0;
	for (i = 0; i <= BUFFER_SIZE; i++) {
		key_buffer[i] = '\0';
	}
	buffer_index = 0;
	ttys[tty_num].cursor_index = 0;
}

/*
 * int32_t keyboard_open(const uint8_t* filename)
 * Description: Helper function for pcb (open)
 * Inputs: A file's name
 * Outputs: Always 0
 */
int32_t keyboard_open(const uint8_t* filename)
{	
	keyboard_init();
	file_open_flag = 1;

	return 0;
}

/*
 * int32_t keyboard_close(int32_t fd)
 * Description: Helper function for pcb (close)
 * Inputs: File descriptor
 * Outputs: Always 0
 */
int32_t keyboard_close(int32_t fd)
{
	file_open_flag = 0;

	return 0;
}

/*
 * up_arrow
 *   DESCRIPTION: prints the next argument from history in the terminal's arg buffer
 *   INPUTS: terminal's arg buffer
 *   OUTPUTS: prints argument from history to screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */  
void up_arrow()
{
	uint32_t i = 0;	
	clear_arg();
	//move the arg from history to key buffer
	strcpy((int8_t*)ttys[tty_num].key_buf, (int8_t*)ttys[tty_num].args[ttys[tty_num].up_args_index]);
	//print it
	 while(buffer_index < strlen((int8_t*)ttys[tty_num].key_buf) && ttys[tty_num].key_buf[i] != '\n')
	 {
	 	key_echo(ttys[tty_num].key_buf[i], 0);
	 	i++;
	 }
	 ttys[tty_num].up_flag = 0;
}

/*
 * clear_arg
 *   DESCRIPTION: clears anything already typed when you press up arrow
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: only the argument from history is shown and in the buffer
 */  
void clear_arg()
{
	uint32_t temp1 = 0;
	uint32_t temp2= 0;
	uint32_t move_cursor = buffer_index - ttys[tty_num].cursor_index;
	uint32_t* x_pos = &temp1;
	uint32_t* y_pos = &temp2;
	get_cursor(x_pos, y_pos);
	set_cursor(*x_pos + move_cursor, *y_pos);
	set_cursor_position();
	while(buffer_index > 0)
	{	
		backspace();
		buffer_index--;

	}
	ttys[tty_num].cursor_index = 0;
}

/*
 * shift_buff_left
 *   DESCRIPTION: if you backspace in the middle of the buffer this shifts it to fill the gap
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: backspacing mid word acts like a terminal would
 */ 
void shift_buff_left()
{
	uint32_t i = ttys[tty_num].cursor_index-1;
	ttys[tty_num].left_flag = 1;
	uint32_t temp1 = 0;
	uint32_t temp2= 0;
	uint32_t* x_pos = &temp1;
	uint32_t* y_pos = &temp2;
	//get the position because putc will move the cursor
	get_cursor(x_pos, y_pos);

	while(i < buffer_index-1)
	{
		key_buffer[i] = key_buffer[i + 1];
		key_echo(ttys[tty_num].key_buf[i], 0);
		i++;
	}	
	key_buffer[buffer_index] = '\0';
	key_echo('\0', 0);
	ttys[tty_num].left_flag = 0;
	//restore the cursor 
	set_cursor(*x_pos + 1, *y_pos);
	set_cursor_position();
}

/*
 * left_arrow
 *   DESCRIPTION: moves cursor to the left
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: see description
 */ 
void left_arrow()
{
	if(ttys[tty_num].cursor_index > 0)
	{
		ttys[tty_num].cursor_index--;
		uint32_t temp1 = 0;
		uint32_t temp2= 0;
		uint32_t* x_pos = &temp1;
		uint32_t* y_pos = &temp2;
		//move one space left of current position
		get_cursor(x_pos, y_pos);
		set_cursor(*x_pos - 1, *y_pos);
		set_cursor_position();
	}
}
