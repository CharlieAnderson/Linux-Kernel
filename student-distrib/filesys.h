#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"
#include "multiboot.h"

#define DIR_ENTRIES_OFFSET 16
#define TYPE_OFFSET 8
#define INODE_OFFSET 9
#define NUM_INODES_OFFSET 1
#define NUM_DATA_OFFSET 2
#define BLOCK_OFFSET 1024
#define BLOCK_SIZE 4096 
#define EOF -1
//for checking validity of file descriptors
#define MAX_FD 7
#define MIN_FD 2

//represent the dentries in the boot block
typedef struct 
{
	uint8_t name[32];
	uint32_t type;
	uint32_t inode;
	uint8_t reserved[24];

} dentry_t;

//represent the inodes
typedef struct 
{
	uint32_t length;
	uint32_t inode_data_blocks[1023];
} inode_t;

//generic struct of function pointers so we 
//can store one in each pcb, no matter what file type
typedef struct 
{
	int32_t (*open)(const uint8_t* filename);
	int32_t (*close)(int32_t fd);
	int32_t (*read)(int32_t fd, uint8_t* buf, int32_t nbytes);
	int32_t (*write)(uint8_t* buf, int32_t nbytes);
} fcn_table_t;

//file descriptors
typedef struct 
{
	fcn_table_t file_op_ptr;
	uint32_t inode_ptr;
	uint32_t file_pos;
	uint32_t flags;
} fdentry_t;

//our file descriptor table
fdentry_t* fd_array;			//global fdentry_t pointer

//setup values 
void filesys_init(uint32_t mod_start);
//find file by name and fill out dentry
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
//find file by inode index and fill out dentry
int32_t read_dentry_by_index (const uint8_t index, dentry_t* dentry);
//fill buffer with length bytes from file at inode index
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
//add a file to file descriptor table
int32_t file_open (const uint8_t* filename);
//remove file from file descriptor table
int32_t file_close (int32_t fd);
//uses read_data and also keeps track of position in the file
int32_t file_read (int32_t fd, uint8_t* buf, int32_t nbytes);
//place holder for fcn_table_t
int32_t file_write(uint8_t* buf, int32_t nbytes);
//same as file_open
int32_t dir_open (const uint8_t* filename);
//same as file_close
int32_t dir_close (int32_t fd);
//returns file names one at a time. returns 0s once all are read
int32_t dir_read (int32_t fd, uint8_t* buf, int32_t nbytes);
//place holder for fcn_table_t
int32_t dir_write(uint8_t* buf, int32_t nbytes);
//loads executable user program into memort to be ran
void loader(uint32_t* dest, const uint8_t* filename);

int8_t lookup_name(const uint8_t* fname, dentry_t* dentry);

#endif /* _FILESYS_H */
