#ifndef PIT_H_
#define PIT_H_

#include "i8259.h"
#include "lib.h"
#include "sched.h"

//sets up pit to tick
void init_pit(void);
//calls schedule() on quantums
void timeHandler(void);
//set tick frequency
void setPIT(int freq);

#endif /* PIT_H_ */
