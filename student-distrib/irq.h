#ifndef _IRQ_H
#define _IRQ_H

#include "pcb.h"

//functions to store registers at an interrupt
extern void irq_keyboard();
extern void irq_rtc();
extern void irq_pit();

#endif
