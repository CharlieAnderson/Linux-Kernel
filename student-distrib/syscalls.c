#include "syscalls.h"
#include "lib.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "keyboard.h"
#include "idt_init.h"
#include "rtc.h"
#include "filesys.h"
#include "paging.h"
#include "terminal.h"
#include "linkage.h"
#include "tty.h"

static dentry_t dentry;
static fcn_table_t rtc_ops = {rtc_open, rtc_close, rtc_read, rtc_write};
static fcn_table_t file_ops = {file_open, file_close, file_read, file_write};
static fcn_table_t dir_ops = {dir_open, dir_close, dir_read, dir_write};

/*

Kernel code executes at privilege level 0, while user-level code must execute at privilege level 3. The x86 processor
does not allow a simple function call from privilege level 0 code to privilege level 3, so you must use an x86-specific
convention to accomplish this privilege switch.
The convention to use is the IRET instruction. Read the ISA reference manual for the details of this instruction. 
You
must set up the correct values for the user-level EIP, CS, EFLAGS, ESP, and SS registers on the kernel-mode stack, and
then execute an IRET instruction. 
The processor will pop the values off the stack into those registers, and by doing
this, will perform a privilege switch into the privilege level specified by the low 2 bites of the CS register. 
The values for the CS and SS registers must point to the correct entries in the Global Descriptor Table that correspond to the
user-mode code and stack segments, respectively. 
The EIP you need to jump to is the entry point from bytes 24-27 of
the executable that you have just loaded. Finally, you need to set up a user-level stack for the process. 
For simplicity,
you may simply set the stack pointer to the bottom of the 4 MB page already holding the executable image.
Two final bits: the DS register must be set to point to the correct entry in the GDT for the user mode data segment (USER DS)
before you execute the IRET instruction (conversely, when an entry into the kernel happens, 
for example, through a system call, exception, or interrupt, you should set DS to point to the KERNEL DS segment). 
Finally, you will need to modify the TSS; this is explained in Appendix E.

*/

/*
 * halt
 *   DESCRIPTION: end program and return to parent, if no parent relaunch shell
 *   INPUTS: status - return value to shell
 *   OUTPUTS: none
 *   RETURN VALUE: status or 256 on exception
 *   SIDE EFFECTS: process restored to parent
 */  
int32_t halt(uint8_t status) {

	//close all files
	int i;
	for(i = 0; i <= MAX_FD; i++)
	{
		close(i);
	}	

 	//terminate process
 	pcb_t *current_pcb = 0;
    cli();
    current_pcb = get_pcb_ptr();
   
   	//case of last shell
    if (current_pcb->parent == NULL) {
    		int i;
		   	for(i = 0; i<RUN_QUEUE_SIZE; i++){
		   		if(run_queue[i] == current_pcb){
		   			run_queue[i] = NULL;
		   		}
		   	} 
            printf("Last shell, nice try.\n");
            kfree(current_pcb->PID);
            cli();
            new_term = 1;
            execute((uint8_t*)"shell");                    
            return 0;
    }

   	pcb_t* parent_pcb = current_pcb->parent;
   	fd_array = parent_pcb->pcb_fd_array;
   	//put parent back on run queue
   	for(i = 0; i<RUN_QUEUE_SIZE; i++){
   		if(run_queue[i] == current_pcb){
   			run_queue[i] = parent_pcb;
   			break;
   		}
   	} 
   	//restore parent values
    uint32_t pid = parent_pcb -> PID;
   	uint32_t kernel_stack = KERNEL_START - KERNEL_STACK_SIZE*pid - STACK_OFFSET; 
	tss.esp0 = kernel_stack;
	uint32_t ebp = current_pcb->registers.ebp;
	uint32_t esp = current_pcb->registers.esp;
	uint32_t* cr3 = parent_pcb->cr3;

	kfree(current_pcb->PID);

	asm volatile("					\n\
				movl %%ebx, %%cr3	\n\
				"
				:
				:"b"(cr3)
				:"cc"
				);
	asm volatile("					\n\
				xorl %%eax, %%eax 	\n\
				movb %%bl, %%al 	\n\
				"
				::"b"(status));
	asm volatile("			\n\
			movl %%ecx, %%ebp	\n\
			movl %%ebx, %%esp 	\n\
			"
			:
			:"c"(ebp), "b"(esp));

	sti();
	//special case of exception we need return value of 256
	//too big to pass in as status
	if (halt_exception == 1){
		halt_exception = 0;
		asm volatile("			\n\
			movl $256, %%eax	\n\
			":::"eax");
	}
	asm volatile("jmp ret_label");
	return -1;
}

/*
 * execute
 *   DESCRIPTION: begin to execute a user level program
 *   INPUTS: command - program name and maybe an argument
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure
 *   SIDE EFFECTS: process is now being scheduled
 */  
int32_t execute(const uint8_t* command) {
	uint32_t ret_val;
 	uint8_t file_name[BUFFER_SIZE];
 	uint8_t args[2][ARG_SIZE];
 	int num_args = 0;
	int i = 0;
	int name_length = 0;
	int arg_index=0;
	int no_args = 0;
	halt_exception = 0;
	int x = 0;

	for(x = 0; x<ARG_SIZE; x++)
	{
		args[0][x] = '\0';
		args[1][x] = '\0';
	}
	//do some checks
	if(command == 0 || command ==NULL)
	{
	 	printf("Invalid command");
		return -1;
	}

	if(command[0] == '\0' || command[0]==' ' || command[0]=='\n'){
		return -1;
	}

	while(command[i]!=' ' && command[i]!='\0')
	{	
		if(i>BUFFER_SIZE){
	 		printf("File name is too long");
	 		return -1;
		}

		file_name[i] = command[i];
		i++;
	}

	name_length = i;
	file_name[i] = '\0';



	if(command[i]==' ') {
		i++;
		if(command[i]==' ') {
			int x = 0;
			for(x = 0; x<BUFFER_SIZE/2; x++)
			{
				args[0][x] = '\0';
				args[1][x] = '\0';
			}
			no_args=1;
		}
	}
	//get args to store in pcb for get_args
	while(command[i]!='\0' && no_args==0 && command[i]!='\n')
	{	
		if(num_args>1) {
	 		printf("too many args");
	 		return -1;
		}
		if(arg_index>=BUFFER_SIZE/2) {
	 		printf("arg is too long");
	 		return -1;
		}
		if(command[i]==' '){
			num_args++;
			i++;
			arg_index = 0;
			args[num_args][arg_index] = '\0';
		}

		args[num_args][arg_index] = command[i];
		i++;
		arg_index++;
	}

	args[1][arg_index] = '\0';

	//make sure it's executable
	if(!read_dentry_by_name(file_name, &dentry)){
 		int8_t elf_buf[EXE_BUF_SIZE];
 		if(read_data(dentry.inode, 0, (uint8_t*)elf_buf, EXE_BUF_SIZE)!=(-1)) {
 			if(strncmp(elf_buf, "ELF", EXE_BUF_SIZE)) {
 		 		return -1;
			}
 		}
	}
	else{
	 	return -1;
	}

	uint32_t* start, *user_stack;
	uint32_t kernel_stack;
	int32_t pid;

	//initialize pcb
	uint32_t ebp, esp;
	asm volatile("			\n\
				movl %%ebp, %%eax 	\n\
				movl %%esp, %%ebx 	\n\
			"
			:"=a"(ebp), "=b"(esp));

	//if there is already 6 processes running
	if(-1 == (pid = pcb_init(get_pcb_ptr(), ebp, esp, args[0]))){
		printf("No more memory\n");
		return -1;
	}

	//init fops to file by default (for error checking purposes)
	for(i = 2; i < 8; i++)
	{
		fd_array[i].file_op_ptr = file_ops;
	}

	//creating page
	createPage(pid);

	//assuming counter for writing to physical memory
	cli();
	start = (uint32_t*)(VIRT_START + START_OFFSET);
	loader(start, file_name);
	kernel_stack = KERNEL_START - KERNEL_STACK_SIZE*pid - STACK_OFFSET; 
	tss.esp0 = kernel_stack;
	user_stack = (uint32_t*)(USER_START_ADDR - STACK_OFFSET); 
	//get to 3rd ring
	USER_LEVEL(user_stack, (uint32_t*)start[USER_START_INDEX]);
	//jump here on halt
	asm volatile("ret_label:"
				:"=a"(ret_val));
	return ret_val;

}

/*
 * read
 *   DESCRIPTION: dispatches system call to correct read by file type
 *   INPUTS: fd - file descriptor to read
 *			 buf - buffer to read into
 *			 nbytes - bytes to read into buf
 *   OUTPUTS: buf holds bytes read
 *   RETURN VALUE: -1 on failure or bytes read on success
 *   SIDE EFFECTS: appropriate read performed
 */  
int32_t read(int32_t fd, void* buf, int32_t nbytes){

 	//if fd->filetype File types are 0 for a file giving user-level access to
	//the real-time clock (RTC), 1 for the directory, and 2 for a regular file. 
	if (buf == NULL)
		return -1;

	return get_pcb_ptr()->pcb_fd_array[fd].file_op_ptr.read(fd, (uint8_t*)buf, nbytes);
}

/*
 * write
 *   DESCRIPTION: dispatches system call to correct write by file type
 *   INPUTS: fd - file descriptor to write to
 *			 buf - buffer to write from
 *			 nbytes - bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure or 0 on succes
 *   SIDE EFFECTS: appropriate read performed
 */ 
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
 	//if fd->filetype File types are 0 for a file giving user-level access to
	//the real-time clock (RTC), 1 for the directory, and 2 for a regular file. 
	sti();
	
	if (buf == NULL)
		return -1;

	return get_pcb_ptr()->pcb_fd_array[fd].file_op_ptr.write((uint8_t*)buf, nbytes);
}

/*
 * open
 *   DESCRIPTION: sets up a file in the fd_array so fops can be done on it
 *   INPUTS: filename - file to open
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure or fd on succes
 *   SIDE EFFECTS: fd taken, fops can be performed on it
 */ 
int32_t open(const uint8_t* filename){

 	//if fd->filetype File types are 0 for a file giving user-level access to
	//the real-time clock (RTC), 1 for the directory, and 2 for a regular file. 

	dentry_t new_dentry;
	dentry_t* dentry = &new_dentry;
	if (-1 == read_dentry_by_name(filename, dentry))
		return -1;
	int i;
	for (i=2; i<9; i++){
		if(i == MAX_DENTRIES)
			return -1;
		if (fd_array[i].flags == 0){					//1 is in use
			fd_array[i].flags = 1;
			fd_array[i].file_pos = 0;
			break;
		}
	}

	switch(dentry->type){
		case 0:
			fd_array[i].inode_ptr = NULL;
			fd_array[i].file_op_ptr = rtc_ops;
			break;

		case 1:
			fd_array[i].inode_ptr = NULL;
			fd_array[i].file_op_ptr = dir_ops;
			break;

		case 2:
			fd_array[i].inode_ptr = dentry->inode;
			fd_array[i].file_op_ptr = file_ops;
			break;
	}

	return i;
}

/*
 * close
 *   DESCRIPTION: sets fd flag so fd can be used again
 *   INPUTS: filename - file to close
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure or 0 on succes
 *   SIDE EFFECTS: file can no longer be used
 */ 
int32_t close(int32_t fd){

 	//if fd->filetype File types are 0 for a file giving user-level access to
	//the real-time clock (RTC), 1 for the directory, and 2 for a regular file. 

 	if (fd > MAX_FD || fd < MIN_FD)
		return -1;
	if(fd_array[fd].flags == 0)
		return -1;
	fd_array[fd].flags = 0;
	return 0;
}

/*
 * getarfs
 *   DESCRIPTION: returns the argument entered at program's execution
 *   INPUTS: buf - write arg to 
 *			 nbytes - max bytes to write
 *   OUTPUTS: buf - returns the argument from pcb
 *   RETURN VALUE: -1 on failure or 0 on succes
 *   SIDE EFFECTS: user program can get arg entered by user
 */ 
int32_t getargs(uint8_t* buf, int32_t nbytes){
//create buffer with arg from command line
	//printf("getargs syscall\n");
	if (buf == NULL)
		return -1;
	pcb_t* cur_pcb = get_pcb_ptr();
	strcpy((int8_t*)buf, (int8_t*)cur_pcb->arg_buf);
 	return 0;

}

/*
 * vidmap
 *   DESCRIPTION: gives a pointer to video memory to the user program
 *   INPUTS: screen_start - pointer to store vidmem access
 *   OUTPUTS: screen_start
 *   RETURN VALUE: 0 success, -1 failure
 *   SIDE EFFECTS: user program write to the screen
 */
int32_t vidmap(uint8_t** screen_start){
//maps a page to lpoc of video memory in physical memory, then modify the arg with the start of this page address

	if (screen_start < (uint8_t**)VIRT_START || screen_start > (uint8_t**)USER_START_ADDR)
		return -1;

	*screen_start = map_vid(get_pcb_ptr()->PID);
	return 0;
}

//not implemented
int32_t set_handler(int32_t signum, void* handler_address){

	return 0;
} 

//not implemented
int32_t sigreturn(void){

	return 0;
}

/*
 * get_pcb_ptr
 *   DESCRIPTION: gets a pointer to the current process's pcb by masking esp
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to pcb
 *   SIDE EFFECTS: easily access anything stored in the pcb
 */
pcb_t* get_pcb_ptr(void){
	pcb_t* ptr;
	asm volatile("				   \n\
				andl %%esp, %%eax  \n\
				"
				:"=a"(ptr)
				:"a"(PCB_MASK)
				:"cc"
				);
	return ptr;
};

