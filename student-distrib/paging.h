#ifndef PAGING_H_
#define PAGING_H_

#include "lib.h"
#include "paging.h"
#include "types.h"

//addresses
#define VIDEO 0xB8000
#define KERNEL 0x00400000
#define KERNEL_END 0x800000
#define VIRT_START 0x8000000
#define USER_START_ADDR 0x8400000
#define TTY1 0xD0000
#define TTY2 0xD1000
#define TTY3 0xD2000 
//frequently used values
#define PAGE_SIZE 0x400000
#define BLANK 0x00000002
#define PAGE_DIR_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define VIDEO_INDEX 184
#define DATA_SIZE 4096
#define PAGE_DIR_INDEX 22
//masks
#define USER_VIDEO 0x7
#define MASK 0x3
#define DIR_FLAGS 0x83
#define USER_FLAG 0x87

//initialize directories and tables
void page();
//map to physical memory and load the cr3
void enablePaging();
//setup page directory for new process
void createPage(uint32_t pid);
//map a page to the screen
uint8_t* map_vid(uint32_t pid);
//remapping memory to screen for terminal switches
void re_map_vid();
//array of page directories
uint32_t* pg_array[6];
	
#endif /* PAGING_H_ */
