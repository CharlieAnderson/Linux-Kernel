#define ASM     1

#include "x86_desc.h"

.text

.globl irq_keyboard
.globl irq_rtc
.globl irq_pit

.align 4

 #:* irq_keyboard
 #:*   DESCRIPTION: storage for keyboard interrupt
 #:*   INPUTS: noen
 #:*   OUTPUTS: none
 #:*   SIDE EFFECTS: program continues executing normally
 #:*					after an interrupt
irq_keyboard:
	pushal
	pushf
	movl %esp, temp_esp
	call keyboard_handler 
	popf
	popal
	iret

 #:* irq_rtc
 #:*   DESCRIPTION: storage for rtc interrupt
 #:*   INPUTS: noen
 #:*   OUTPUTS: none
 #:*   SIDE EFFECTS: program continues executing normally
 #:*					after an interrupt
irq_rtc:
	pushal
	call rtc_handler
	popal
	iret

 #:* irq_pit
 #:*   DESCRIPTION: storage for pit interrupt
 #:*   INPUTS: noen
 #:*   OUTPUTS: none
 #:*   SIDE EFFECTS: program continues executing normally
 #:*					after an interrupt
irq_pit:
	pushal
	pushf
	movl %esp, temp_esp
	call timeHandler
	popf
	popal
	iret

.end
