/* Error codes for IDT initialization */

#include "lib.h"
#include "x86_desc.h"
#include "idt_init.h"
#include "irq.h"
#include "linkage.h"
#include "syscalls.h"


/*
 * divide_error
 *   DESCRIPTION: when you divide by zero
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void divide_error() {
	clear();
    cli();
	printf("Exception in: Divide Error (idt[0])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * debug_exception
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void debug_exception() {
	clear();
	cli();
	printf("Exception in: Debug Exception (idt[1])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * nmi_interrupt
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void nmi_interrupt() {
	clear();
	cli();
	printf("Exception in: NMI Interrupt (idt[2])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * breakpoint
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void breakpoint() {
	clear();
	cli();
	printf("Exception in: Breakpoint (idt[3])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * overflow
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void overflow() {
	clear();
	cli();
	printf("Exception in: Overflow (idt[4])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * bound_range_exceeded
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void bound_range_exceeded() {
	clear();
	cli();
	printf("Exception in: BOUND Range Exceeded (idt[5])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * invalid_opcode
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void invalid_opcode() {
	clear();
	cli();
	printf("Exception in: Invalid Opcode (idt[6])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * device_not_available
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void device_not_available() {
	clear();
	cli();
	printf("Exception in: Device Not Available (idt[7])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * double_fault
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void double_fault() {
	clear();
	cli();
	printf("Exception in: Double Fault (idt[8])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * coprocessor_segment_overrun
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void coprocessor_segment_overrun() {
	clear();
	cli();
	printf("Exception in: Coprocessor Segment Overrun (idt[9])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * invalid_tss
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void invalid_tss() {
	clear();
	cli();
	printf("Exception in: Invalid TSS (idt[10])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * segment_not_present
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void segment_not_present() {
	clear();
	cli();
	printf("Exception in: Segment Not Present (idt[11])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * stack_fault
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void stack_fault() {
	clear();
	cli();
	printf("Exception in: Stack Fault (idt[12])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * general_protection
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void general_protection() {
	clear();
	cli();
	printf("Exception in: General Protection (idt[13])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * page_fault
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void page_fault() {
	clear();
	cli();
	printf("Exception in: Page Fault (idt[14])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * floating_point_error
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void floating_point_error() {
	clear();
	cli();
	printf("Exception in: Floating-Point Error (idt[16])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * alignment_check
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void alignment_check() {
	clear();
	cli();
	printf("Exception in: Alignment Check (idt[17])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * machine_check
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void machine_check() {
	clear();
	cli();
	printf("Exception in: Machine Check (idt[18])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * simd_floating_point_exception
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void simd_floating_point_exception() {
	clear();
	cli();
	printf("Exception in: SIMD Floating-Point Exception (idt[19])\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * common_exception
 *   DESCRIPTION: built into processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: this squashes the running program
 */ 
void common_exception() {
	clear();
	cli();
	printf("Exception in: Common\n");
	//flag to halt differently than a process ending
	halt_exception = 1;
	halt(0);
	while(1) {}
}

/*
 * idt_init
 *   DESCRIPTION: set IDT entries for interrupts
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupts are assigned to proper INTs
 */ 
void idt_init() {
	idt_desc_t idt_interrupt;
    idt_interrupt.seg_selector = KERNEL_CS;
    idt_interrupt.reserved4 = 0x0;
    idt_interrupt.reserved3 = 0x0;
    idt_interrupt.reserved2 = 0x1;
    idt_interrupt.reserved1 = 0x1;
    idt_interrupt.reserved0 = 0x0;
	idt_interrupt.size = 0x1;
    idt_interrupt.dpl = 0x0;
    idt_interrupt.present = 0x1;
	
	idt_desc_t idt_exception = idt_interrupt;
	idt_exception.reserved3 = 1;
	
	idt_desc_t idt_syscall = idt_interrupt;
	idt_syscall.dpl = 3;
	idt_syscall.reserved3 = 1;

	int i;
	for (i = 0; i < 32; i++) {
		idt[i]= idt_exception;
	}
	for (i = 32; i < 256; i++) {
		idt[i] = idt_interrupt;
	}
	idt[0x80] = idt_syscall;

	SET_IDT_ENTRY(idt[0], &divide_error);
	SET_IDT_ENTRY(idt[1], &debug_exception);
	SET_IDT_ENTRY(idt[2], &nmi_interrupt);
	SET_IDT_ENTRY(idt[3], &breakpoint);
	SET_IDT_ENTRY(idt[4], &overflow);
	SET_IDT_ENTRY(idt[5], &bound_range_exceeded);
	SET_IDT_ENTRY(idt[6], &invalid_opcode);
	SET_IDT_ENTRY(idt[7], &device_not_available);
	SET_IDT_ENTRY(idt[8], &double_fault);
	SET_IDT_ENTRY(idt[9], &coprocessor_segment_overrun);
	SET_IDT_ENTRY(idt[10], &invalid_tss);
	SET_IDT_ENTRY(idt[11], &segment_not_present);
	SET_IDT_ENTRY(idt[12], &stack_fault);
	SET_IDT_ENTRY(idt[13], &general_protection);
	SET_IDT_ENTRY(idt[14], &page_fault);
	SET_IDT_ENTRY(idt[15], &common_exception);
	SET_IDT_ENTRY(idt[16], &floating_point_error);
	SET_IDT_ENTRY(idt[17], &alignment_check);
	SET_IDT_ENTRY(idt[18], &machine_check);
	SET_IDT_ENTRY(idt[19], &simd_floating_point_exception);

	SET_IDT_ENTRY(idt[0x20], irq_pit);

	SET_IDT_ENTRY(idt[0x21], irq_keyboard);
	SET_IDT_ENTRY(idt[0x28], irq_rtc);
	SET_IDT_ENTRY(idt[0x80], START_SYS_CALLS);			//call sys calls
}
