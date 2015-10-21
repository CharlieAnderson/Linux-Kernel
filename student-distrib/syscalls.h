

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "lib.h"
#include "terminal.h"
#include "rtc.h"
#include "types.h"
#include "filesys.h"
#include "pcb.h"
#include "idt_init.h"
#include "x86_desc.h"
#include "pit.h"

#define PCB_MASK 0xffffe000
#define VIRT_START 0x8000000
#define START_OFFSET 0x48000
#define USER_START_ADDR 0x8400000
#define STACK_OFFSET 0x4
#define MAX_FD 7
#define MIN_FD 2
#define MAX_DENTRIES 8
#define ARG_SIZE BUFFER_SIZE/2
#define NAME_SIZE 32
#define EXE_BUF_SIZE 4
#define USER_START_INDEX 6
#define RUN_QUEUE_SIZE 3

//end a process
extern int32_t halt(uint8_t status);
//start a process
extern int32_t execute(const uint8_t* command);
//call read from fops table for fd
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
//call write from fops table for fd
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);
//add filename to descriptor table so fops can be done on it
extern int32_t open(const uint8_t* filename);
//remove a file from descriptor table
extern int32_t close(int32_t fd);
//return argument from pcb to user program
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
//get a pointer to user program so it can write to screen
extern int32_t vidmap(uint8_t** screen_start);
//not implemented
extern int32_t set_handler(int32_t signum, void* handler_address);
//not implemented
extern int32_t sigreturn(void);
//get pointer to current process pcb
extern pcb_t* get_pcb_ptr(void);
//flag for halt by exception
volatile int32_t halt_exception;

#endif /* _SYSCALLS_H */


