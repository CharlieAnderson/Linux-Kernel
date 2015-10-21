#include "sched.h"
#include "pcb.h"

/*
 * schedule
 *   DESCRIPTION: at each quantum this switches tasks in a round robin fashion
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: the current process's info is stored and the next process
 *					is staged. An iret sets it in motion.
 */  
void schedule (){
	cli();

	pcb_t *current_pcb;
    current_pcb = get_pcb_ptr();

    uint32_t ebp, esp;

    cur_proc++;
    //round robin
    cur_proc = cur_proc % total_proc;

    pcb_t* next_process = run_queue[cur_proc];
   	fd_array = next_process->pcb_fd_array;

    uint32_t pid = next_process -> PID;
   	uint32_t kernel_stack = KERNEL_START - KERNEL_STACK_SIZE*pid - STACK_OFFSET; 
	tss.esp0 = kernel_stack;

	ebp = next_process->registers.cur_ebp;
	esp = next_process->registers.cur_esp;

	uint32_t* cr3 = next_process->cr3;

	asm volatile("					\n\
				movl %%ebx, %%cr3	\n\
				"
				:
				:"b"(cr3)
				:"cc"
				);
	//esp saved in linkage (irq)
	current_pcb->registers.cur_esp = temp_esp;

	asm volatile("				\n\
			movl %%edx, %%esp 	\n\
			movl %%ecx, %%ebp	\n\
			"
			::"c"(next_process->registers.cur_ebp), "d"(next_process->registers.cur_esp));
	//direction video memory
	re_map_vid();

	sti();
	asm volatile(" popf 	\n\
				popal 	\n\
				iret");
	return;
}
