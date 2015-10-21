#ifndef _PCB_H
#define _PCB_H

#include "types.h"
#include "filesys.h"
#include "terminal.h"
#include "keyboard.h"
#include "paging.h"

//laid out like this to see correspondance
#define PCB_SIZE 4+48+4+4+12+28*8+4+4+128+12

#define KERNEL_STACK_SIZE 0x2000
#define KERNEL_START 0x800000
//used for scheduling
//set in linkage
extern uint32_t temp_esp;

typedef enum {TASK_RUNNING, TASK_WAITING, TASK_READY} process_state_t;
//keep track of processes started
uint32_t pid_array[6];
//store everything per process for switching
typedef struct 
{
	//used for scheduling
	uint32_t cur_ebp;
	uint32_t cur_esp;
	//used for halting
	uint32_t esp;
	uint32_t ebp;
} reg_t;
//everything needed to switch processes
typedef struct pcb
{
	//process id
	int32_t PID;//4
	//terminal running this process
	int32_t tty;
	reg_t registers;//48
	//address of stack in kernel
	uint32_t proc_stack;//4
	process_state_t state;//12
	//file descriptor array for this process
	fdentry_t pcb_fd_array[8];//28*8
	struct pcb* parent;//4 tops?
	struct pcb* child;
	uint32_t* cr3;//4
	uint8_t arg_buf[BUFFER_SIZE];//128
} pcb_t;
pcb_t* run_queue[3];

//setup EVERYTHING
int32_t pcb_init(pcb_t* parent, uint32_t ebp, uint32_t esp, uint8_t* args);
//placeholder function for fops table
int32_t stdout_read(int32_t fd, uint8_t* buf, int32_t nbytes);
//initialize pid array and run queue
void pid_init(void);
//finds open pid
int32_t get_pid(void);
//sets pid in array to 0
void kfree(uint32_t pid);

#endif /* _PCB_H */
