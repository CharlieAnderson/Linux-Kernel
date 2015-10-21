#ifndef SCHED_H_
#define SCHED_H_

#include "types.h"
#include "pcb.h"
#include "syscalls.h"
#include "x86_desc.h"
#include "tty.h"
#include "paging.h"

//process to be scheduled
uint32_t cur_proc;

//round robin scheduling
void schedule(void);

#endif /* SCHED_H_ */
