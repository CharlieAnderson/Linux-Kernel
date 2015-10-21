#ifndef _LINKAGE_H
#define _LINKAGE_H

#include "syscalls.h"
#include "i8259.h"
//#include "sched.h"

//linkage for system call
extern void START_SYS_CALLS();
//execute uses for an artificial iret to jump to user level
extern void USER_LEVEL(uint32_t* process_stack, uint32_t* eip);

#endif
