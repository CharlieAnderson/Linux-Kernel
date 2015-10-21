#include "paging.h"
#include "tty.h"

//the 3 available terminals
extern tty_t tty1;
extern tty_t tty2;
extern tty_t tty3;

extern tty_t ttys[3];

//static initial paging
static uint32_t page_directory[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));
static uint32_t page_table[PAGE_TABLE_SIZE] __attribute__((aligned(DATA_SIZE)));

//paging for each individual process
static uint32_t page_directory0[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));
static uint32_t page_table0[PAGE_TABLE_SIZE] __attribute__((aligned(DATA_SIZE)));

static uint32_t page_directory1[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));

static uint32_t page_directory2[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));

static uint32_t page_directory3[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));

static uint32_t page_directory4[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));

static uint32_t page_directory5[PAGE_DIR_SIZE] __attribute__((aligned(DATA_SIZE)));

/*
 * void page()
 * Description: Initializes paging (called from in kernel.c)
 * Inputs: None
 * Outputs: None
 */
void page(){
	int i;

	pg_array[0] = page_directory0;
	pg_array[1] = page_directory1;
	pg_array[2] = page_directory2;
	pg_array[3] = page_directory3;
	pg_array[4] = page_directory4;
	pg_array[5] = page_directory5;

	/* Initializes each entry */
	for(i = 0; i < PAGE_DIR_SIZE; i++){
		page_directory[i] = BLANK;
	}
	//clear the pages
	for(i = 0; i<PAGE_TABLE_SIZE; i++){
		page_table[i] = BLANK;
		page_table0[i] = BLANK;
	}
	//direct to physical memory
	page_table[VIDEO_INDEX] = VIDEO | MASK;
	page_table[208] = TTY1 | MASK;
	page_table[209] = TTY2 | MASK;
	page_table[210] = TTY3 | MASK;
	page_table0[0] = VIDEO | USER_VIDEO;
	//initial paging is 4MB
	page_directory[0] = ((unsigned int)page_table) | MASK;
	page_directory[1] = KERNEL | DIR_FLAGS;

	enablePaging();
}

/*
 * void enablePaging()
 * Description: Sets the cr3 and cr4 registers and paging enable bit
 * Inputs: None
 * Outputs: None
 */
void enablePaging()
{
	asm volatile("                  \n\
			movl 	$page_directory, %%eax \n\
			movl 	%%eax, %%cr3 \n\
			movl	%%cr4, %%eax	\n\
			orl $0x00000010, %%eax	\n\
			movl 	%%eax, %%cr4	\n\
			movl    %%cr0, %%eax	     \n\
			orl $0x80000000, %%eax	\n\
			movl    %%eax, %%cr0      \n\
			"
			:
			:
			: "eax", "cc"
			);
}

/*
 * void createPage(uint32_t pid)
 * Description: Create a memory location for the computer to store/retrieve data with a specific PID
 * Inputs: A process ID (unique process identifier)
 * Outputs: None
 */
void createPage(uint32_t pid){
	uint32_t* pg_dir_ptr = pg_array[pid];
	int i;
	for(i = 0; i < 1024; i++){
		pg_dir_ptr[i] = BLANK;
	}
	pg_dir_ptr[0] = ((unsigned int)page_table) | MASK;
	pg_dir_ptr[1] = KERNEL | DIR_FLAGS;
	pg_dir_ptr[VIRT_START>>PAGE_DIR_INDEX] = ((KERNEL_END + PAGE_SIZE*pid) | USER_FLAG);
	asm volatile("					\n\
				movl %%eax, %%cr3	\n\
				"
				:
				:"a"(pg_dir_ptr)
				:"cc"
				);
}

/*
 * uint8_t* map_vid(uint32_t pid)
 * Description: Returns the address for video memory to start at in physical memory
 * Inputs: A process ID (unique process identifier)
 * Outputs: None
 */
uint8_t* map_vid(uint32_t pid){
	uint32_t* pg_dir_ptr = pg_array[pid];
	//map a page to the screen
	pg_dir_ptr[USER_START_ADDR>>PAGE_DIR_INDEX] = ((unsigned int)page_table0) | USER_VIDEO;
	return (uint8_t*)USER_START_ADDR;
}

/*
 * void re_map_vid()
 * Description: Maps the video memory if the terminal id has changed
 * Inputs: An id to check with a tty
 * Outputs: None
 */
void re_map_vid(){
	uint32_t* pg_dir_ptr = pg_array[get_pcb_ptr()->PID];

	//if the terminal displayed is running map its memory to the screen
	if (tty_num == get_pcb_ptr()->tty)
		pg_dir_ptr[USER_START_ADDR>>PAGE_DIR_INDEX] = ((unsigned int)page_table0) | USER_VIDEO;
	//if the terminal running is in the background map to memory elsewhere
	else{
		page_table[0] = ((unsigned int)ttys[get_pcb_ptr()->tty].vidmem) | USER_VIDEO;
		pg_dir_ptr[USER_START_ADDR>>PAGE_DIR_INDEX] = ((unsigned int)page_table) | USER_VIDEO;
	}
	//reset page directory
	asm volatile("movl %%cr3, %%eax 	\n\
				movl %% eax, %%cr3 		\n\
				"
				:::"eax");
}
