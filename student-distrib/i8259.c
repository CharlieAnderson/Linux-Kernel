/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

 /*
 * i8259_init
 *   DESCRIPTION: Initialize the 8259 PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupts can now be enabled
 */   
void
i8259_init(void)
{								//based off of wiki.osdev.org 
	uint32_t flags;
	cli_and_save(flags);
	
	outb( ICW1, MASTER_8259_PORT);  
	outb( ICW1, SLAVE_8259_PORT);

	outb( ICW2_MASTER, MASTER_8259_DATA);
	outb( ICW2_SLAVE, SLAVE_8259_DATA);

	outb( ICW3_MASTER, MASTER_8259_DATA);
	outb( ICW3_SLAVE, SLAVE_8259_DATA);
 
	outb( ICW4, MASTER_8259_DATA);
	outb( ICW4, SLAVE_8259_DATA);

	outb(0xff,MASTER_8259_DATA);
	outb(0xff,SLAVE_8259_DATA);
	
	restore_flags(flags);
}

/*
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ 
 *   INPUTS: irq_num
 *   OUTPUTS: signal to pic to enable interrupt
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupts are now active for the given device
 */ 
void
enable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < MASTER_IRQS) {
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= MASTER_IRQS;
		value = inb(MASTER_8259_DATA) & ~(1 << 2);
		outb(value, MASTER_8259_DATA);
    }
    value = inb(port) & ~(1 << irq_num);
    outb( value, port);
}

/*
 * disable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ
 *   INPUTS: irq_num
 *   OUTPUTS: signal to pic to disable interrupt
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupts are now inactive for the given device
 */ 
void
disable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < MASTER_IRQS) {
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= MASTER_IRQS;
    }
    value = inb(port) | (1 << irq_num);
    outb(value, port );
}

/*
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ 
 *   INPUTS: irq_num
 *   OUTPUTS: signal to pic that communication is over
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupt can return
 */ 
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= MASTER_IRQS)
		outb( 0x20, SLAVE_8259_PORT);
 												//works if i use 0x20 (nonspecific eoi) instead of specific eoi
	outb(0x20, MASTER_8259_PORT); 

}
