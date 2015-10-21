#include "pit.h"
//counter for pit
int ticks = 0;

/*
 * init_pit
 *   DESCRIPTION: ready the pit for use
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: the pit is now counting ticks
 */ 
void init_pit() {
	 ticks = 0;
	 enable_irq(0);
	 setPIT(1);

	//http://www.osdever.net/bkerndev/Docs/pit.htm

}
/*
 * init_pit
 *   DESCRIPTION: set the frequency of ticks
 *   INPUTS: freq - scale factor
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: the pit ticks at designated frequency
 */ 
void setPIT(int freq) {		//NOT sure if working properly

	int divisor = 1193180 / freq;       /* Calculate our divisor */   
	outb(0x43, 0x36);             /* Set our command byte 0x36 */
	outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outb(0x40, divisor >> 8);     /* Set high byte of divisor */

}

/*
 * timeHandler
 *   DESCRIPTION: interupt handler for pit
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: schedule() is called every quantum
 */ 
void timeHandler() {
	send_eoi(0);

	schedule();
}
