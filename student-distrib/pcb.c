#include "pcb.h"
#include "tty.h"

//used for scheduling
//set in linkage
uint32_t temp_esp;
/*
 * pcb_init
 *   DESCRIPTION: sets up everything you could need to know about a process
 *   INPUTS: parent - pointer to parent pcb to be stored so it can return easily
 *			 ebp & esp - stored for halting
 *			 args - stored per process for get_args
 *   OUTPUTS: none
 *   RETURN VALUE: PID on success, -1 on failure
 *   SIDE EFFECTS: operations can be called, halting and scheduling possible
 */ 
int32_t pcb_init(pcb_t* parent, uint32_t ebp, uint32_t esp, uint8_t* args)
{
	int32_t pid;
	pcb_t new_pcb;
	//init buffer and copy args to it
	int i;
	for(i = 0; i< BUFFER_SIZE; i++)
	{
		new_pcb.arg_buf[i] = '\0';
	}
	strcpy((int8_t*)new_pcb.arg_buf, (int8_t*)args);

	//get an available pid 
	if(-1 == (pid = get_pid()))
		return -1; 
	
	new_pcb.state = TASK_RUNNING;
	new_pcb.PID = pid;
	//address to store pcb in kernel memory
	pcb_t* pcb_ptr = (pcb_t*)(KERNEL_START - KERNEL_STACK_SIZE*(new_pcb.PID + 1));

	//initialize fd_array
	fd_array = pcb_ptr->pcb_fd_array;

	//set new process to its parent's child
	if (new_term == 0){
		parent->child = pcb_ptr;
	}
	//if it's a new terminal
	if(new_term == 1){
		new_pcb.parent = NULL;
		new_term = 0;
		new_pcb.child = NULL;
	}
	//not a terminal
	else{
		new_pcb.parent = parent;
		new_pcb.child = NULL;
	}

	new_pcb.cr3 = pg_array[new_pcb.PID];
	//initialize stdin
	new_pcb.pcb_fd_array[0].flags = 1;
	new_pcb.pcb_fd_array[0].file_op_ptr.open = keyboard_open;
	new_pcb.pcb_fd_array[0].file_op_ptr.close = keyboard_close;
	new_pcb.pcb_fd_array[0].file_op_ptr.read = terminal_read;
	new_pcb.pcb_fd_array[0].file_op_ptr.write = file_write;
	new_pcb.pcb_fd_array[0].inode_ptr = NULL;
	new_pcb.pcb_fd_array[0].file_pos = -1;
	new_pcb.pcb_fd_array[1].flags = 1;
	//initialize stdout
	new_pcb.pcb_fd_array[1].file_op_ptr.open = terminal_open;
	new_pcb.pcb_fd_array[1].file_op_ptr.close = terminal_close;
	new_pcb.pcb_fd_array[1].file_op_ptr.read = stdout_read;
	new_pcb.pcb_fd_array[1].file_op_ptr.write = terminal_write;
	new_pcb.pcb_fd_array[1].inode_ptr = NULL;
	new_pcb.pcb_fd_array[1].file_pos = -1;
	//everything else is closed
	new_pcb.pcb_fd_array[2].flags = 0;
	new_pcb.pcb_fd_array[3].flags = 0;
	new_pcb.pcb_fd_array[4].flags = 0;
	new_pcb.pcb_fd_array[5].flags = 0;
	new_pcb.pcb_fd_array[6].flags = 0;
	new_pcb.pcb_fd_array[7].flags = 0;	
	new_pcb.registers.ebp = ebp;
	new_pcb.registers.esp = esp;
	new_pcb.registers.cur_ebp = ebp;
	new_pcb.registers.cur_esp = esp;
	//store terminal running on
	new_pcb.tty = tty_num; 
	keyboard_open((uint8_t*)"NULL");
	memcpy((void*)(KERNEL_START - KERNEL_STACK_SIZE*(new_pcb.PID + 1)), &new_pcb, PCB_SIZE);
	//replace the terminal in the run queue with its child process
	if (new_pcb.parent != NULL)
	{
		int i;
		for(i = 0; i<3; i++){
			if (run_queue[i] == new_pcb.parent){
				run_queue[i] = pcb_ptr;
				break;
			}
		}
	}
	//or just add the terminal to the next open spot
	else{
		int i;
		for(i = 0; i<3; i++){
			if(run_queue[i] == NULL){
				run_queue[i] = pcb_ptr;
				break;
			}
		}
	}
	return new_pcb.PID;
}

/*
 * pid_init
 *   DESCRIPTION: clears all pids
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: all pids are initialized to be used
 */ 
void pid_init(void){
	int i;
	for(i =0; i<6; i++){
		pid_array[i] = 0;
	}

	for(i = 0; i<3; i++){
		run_queue[i] = NULL;
	}
}

/*
 * get_pid
 *   DESCRIPTION: returns available pid
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: open pid
 *   SIDE EFFECTS: pid will be used for new process
 */ 
int32_t get_pid(void){
	int i;
	for(i=0; i<6; i++){
		if(pid_array[i] == 0){
			pid_array[i] = 1;
			return i;
		}
	}
	return -1;
}

/*
 * stdout_read
 *   DESCRIPTION: palceholder for fops table for stdout's read
 *   INPUTS: irrelevant
 *   OUTPUTS: none
 *   RETURN VALUE: -1, not readable
 */ 
int32_t stdout_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	return -1;
}

/*
 * kfree
 *   DESCRIPTION: makes pid available
 *   INPUTS: pid - pid to free
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: pid open to be used later
 */ 
void kfree(uint32_t pid){
	pid_array[pid] = 0;
}
