
#include "tty.h"
#include "pcb.h"

tty_t tty1;
tty_t tty2;
tty_t tty3;

tty_t ttys[3];

extern unsigned char* key_buffer;

/*
 * tty_init
 *   DESCRIPTION: set all default values for all 3 terminals
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: terminals are ready to launch
 */  
uint32_t tty_init()
{
	new_term = 1;

	tty1.flag = 1;
	tty2.flag = 0;
	tty3.flag = 0;

	tty1.tty_id = 0;
	tty2.tty_id = 1;
	tty3.tty_id = 2;

	tty1.vidmem = (uint8_t*)0xD0000;
	tty2.vidmem = (uint8_t*)0xD1000;
	tty3.vidmem = (uint8_t*)0xD2000;

	ttys[0] = tty1;
	ttys[1] = tty2;
	ttys[2] = tty3;

	int i;
	for(i = 0; i < 3; i++)
	{
		ttys[i].x_pos = 0;
		ttys[i].y_pos = 0;
		ttys[i].buf_idx = 0;
		ttys[i].ent_flag = 0;
		ttys[i].up_flag = 0;
		ttys[i].up_args_index = 0;
		ttys[i].up_args = -1;
		ttys[i].cursor_index = 0;
		ttys[i].left_flag = 0;
	}	

	tty_num = 0;
	key_buffer = ttys[0].key_buf;

	return 0;
}

/*
 * tty_launch
 *   DESCRIPTION: set up new terminal or just do a switch
 *   INPUTS: id - terminal number that you are launching
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: terminal id's video memory is written to screen
 */  
uint32_t tty_launch(uint32_t id){
	int32_t old = tty_num;
	//if you don't need to switch
	if (id == tty_num)
		return 0;
	//if it's already running
	if (ttys[id].flag){
		tty_switch(&ttys[tty_num], &ttys[id]);
		tty_num = id;
	}
	//if launching for the first time
	else{
		ttys[tty_num].buf_idx = buffer_index;
		buffer_index = ttys[id].buf_idx;
		//map video memory to this terminal's
		memmove(ttys[tty_num].vidmem, (uint8_t*)VIDEO, 4096);	
		//store current cursor position
		get_cursor(&ttys[tty_num].x_pos, &ttys[tty_num].y_pos);
		clear();
		key_buffer = ttys[id].key_buf;
		tty_num = id;
		ttys[id].flag = 1;
		new_term = 1;
		total_proc++;
		cli();
		// save esp
		get_pcb_ptr()->registers.cur_esp = temp_esp;
		execute((uint8_t*)"shell");
		//when process halts it returns herer
		total_proc--;
		new_term = 0;
		tty_num = old;
		ttys[id].flag = 0;
		key_buffer = ttys[tty_num].key_buf;
		set_cursor(ttys[tty_num].x_pos, ttys[tty_num].y_pos);
		set_cursor_position();
		memmove((uint8_t*)VIDEO, ttys[tty_num].vidmem, 4096);
		buffer_index = ttys[tty_num].buf_idx;	
	}

	return 0;
}

/*
 * tty_switch
 *   DESCRIPTION: switch to a terminal that has already been launched
 *   INPUTS: curr_term - pointer to current terminal
 *			 next_term - pointer to terminal we are switching to
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: next_term is now displayed
 */  
uint32_t tty_switch(tty_t* curr_term, tty_t* next_term)
{
	//store current process stuff
	get_pcb_ptr()->registers.cur_esp = temp_esp;
	curr_term->buf_idx = buffer_index;
	buffer_index = next_term->buf_idx;
	get_cursor(&ttys[tty_num].x_pos, &ttys[tty_num].y_pos);
	memmove(curr_term->vidmem, (uint8_t*)VIDEO, 4096);
	clear();
	//restore process switching to
	memmove((uint8_t*)VIDEO, next_term->vidmem, 4096);
	set_cursor(next_term->x_pos, next_term->y_pos);
	set_cursor_position();
	key_buffer = next_term->key_buf;

	return 0;
}
