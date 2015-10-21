#include "filesys.h"
#include "lib.h"
#include "syscalls.h"

//addresses for quick reference
//boot block
static uint32_t* start;
//start of data blocks
static uint8_t* data_blocks;
//pointers to start of dentries and inodes
static dentry_t* dirs;
static inode_t* inode_ptr;
//quantities for locating things
static uint32_t num_entries;
static uint32_t num_inodes; 
static uint32_t num_data;

/*
 * filesys_init
 *   DESCRIPTION: Initialize all components of the file system.
 *   INPUTS: mod_start - the address of the boot block
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: addresses are ready for easy reference
 */  
void filesys_init(uint32_t mod_start){
	clear();
	start = (uint32_t*)mod_start;
	//first item in boot block is the number of files
	num_entries = *start;

	num_inodes = *(start + NUM_INODES_OFFSET);

	num_data = *(start + NUM_DATA_OFFSET);

	dirs = (dentry_t*)(start + DIR_ENTRIES_OFFSET);

	inode_ptr = (inode_t*)(start + BLOCK_OFFSET);

	//data blocks are at the end of the inodes
	data_blocks = (uint8_t*)(start + BLOCK_OFFSET + num_inodes*BLOCK_OFFSET);
}

/*
 * read_dentry_by_name
 *   DESCRIPTION: Locate the data for a file in the system based on its name
 *   INPUTS: name to search for and a dentry to fill in
 *   OUTPUTS: a directory entry consisting of the file's name, type, and inode pointer
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: callee now has access to this file's type and inode
 */ 
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	uint32_t i;
	int32_t flag = -1;


	if(dentry == NULL)
		return flag;

	/* find the file with name fname */
	for(i = 0; i < num_entries; i++)
	{
		//if they aren't the same length go to the next one
		if(strlen((int8_t*)fname) != strlen((int8_t*)(dirs[i].name)))
			continue;

		//if the names match set the flag for success
		if(strncmp((int8_t*)fname, (int8_t*)(dirs[i].name), strlen((int8_t*)fname)) == 0)
		{
			flag = 0;
			break;
		}
	}
	/* if successfully found, fill dentry */
	if(flag == 0)
	{
		strcpy((int8_t*)dentry->name, (int8_t*)fname);
		dentry->type = dirs[i].type ;
		dentry->inode = dirs[i].inode;

	}

	return flag;
}

int8_t lookup_name(const uint8_t* fname, dentry_t* dentry){
	int32_t entries = 0;
	int32_t entry = 0;
	int i;

	if(dentry == NULL)
		return -1;

	for (i = 0; i < num_entries; ++i)
	{
		if(strncmp((int8_t*)fname, (int8_t*)(dirs[i].name), strlen((int8_t*)fname)) == 0)
		{
			entries++;
			entry = i;
		}
	}
	/* if successfully found, fill dentry */
	if(entries == 1)
	{
		strcpy((int8_t*)dentry->name, (int8_t*)dirs[entry].name);
		dentry->type = dirs[entry].type ;
		dentry->inode = dirs[entry].inode;
		return 0;
	}

	return -1;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Locate the data for a file by finding its inode
 *   INPUTS: index in inode list and dentry to fill in
 *   OUTPUTS: a directory entry consisting of the file's name, type, and inode pointer
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: callee now has access to this file's type and name
 */ 
int32_t read_dentry_by_index (const uint8_t index, dentry_t* dentry)
{
	if(dentry == NULL)
		return -1;

	if(index>num_entries)
		return -1;
		
	//if the inode exists, fill out the dentry
	else
	{
		strcpy((int8_t*)dentry->name, (int8_t*)dirs[index].name);
		dentry->type = dirs[index].type ;
		dentry->inode = dirs[index].inode;
	}

	return 0;
}

/*
 * read_data
 *   DESCRIPTION: Reads a specified number of bytes from a file into callee provided buffer
 *   INPUTS: inode - the index into inode list
 *			 offset - how many bytes into the file to start reading from
 *			 buf - char array to read into
 *			 length - the number of bytes to read	
 *   OUTPUTS: buf - read data in a buffer
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: callee has desired data from file
 */ 
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{	
	uint32_t i, count_offset;
	inode_t curr_inode = inode_ptr[inode];
	uint32_t data_length;
	uint32_t inode_idx = offset/BLOCK_SIZE;
	uint32_t data_block_index = curr_inode.inode_data_blocks[inode_idx];
	//offset into inode to start at right data block
	uint8_t* start_addr = (uint8_t*)(data_blocks + BLOCK_SIZE*data_block_index + offset % BLOCK_SIZE);

	data_length = curr_inode.length;
	count_offset = offset % BLOCK_SIZE;

	if(start_addr == NULL)
		return -1;

	if(inode > num_inodes)
		return -1;

	if(*((int8_t*)start_addr) == EOF)
		return 0;

	for(i = 0; i < length; i++)
	{
		/*check if we need a new block*/
		if (count_offset >= BLOCK_SIZE){
			count_offset = 0;
			inode_idx++;
			data_block_index = curr_inode.inode_data_blocks[inode_idx];

			if(data_block_index >= num_data)
				return -1;

			start_addr = (uint8_t*)(data_blocks+ BLOCK_SIZE*data_block_index);

		}

		/*read data*/
		buf[i] = *start_addr;
		count_offset++;
		start_addr++;
		if (i+offset >= data_length)
			break;

	}
	return i; 
}

/*
 * file_open
 *   DESCRIPTION: fills out an entry in the file descriptor table for the file
 *   INPUTS: filename - used to get info to store in fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: the file can now be read form
 */ 
int32_t file_open (const uint8_t* filename)
{
	dentry_t new_dentry;
	dentry_t* dentry = &new_dentry;
	//get file info
	if (-1 == read_dentry_by_name(filename, dentry))
		return -1;
	int i;
	//opens file in first open spot
	for (i=2; i<8; i++){
		if (fd_array[i].flags == 0){					
			fd_array[i].inode_ptr = dentry->inode;
			fd_array[i].flags = 1;
			fd_array[i].file_pos = 0;
			return i;
		}
	}
	return -1;
}

/*
 * file_close
 *   DESCRIPTION: Frees a spot in the FD table and this file
 *					is no longer readable
 *   INPUTS: fd - index into descriptor table to close
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: the file is no longer readable
 *					a new file can take its place
 */ 
int32_t file_close (int32_t fd)
{
	//make sure it's a valid fd
	if (fd > MAX_FD || fd < MIN_FD)
		return -1;
	//make sure it is open
	if(fd_array[fd].flags == 0)
		return -1;
	fd_array[fd].flags = 0;
	return 0;
}

/*
 * file_read
 *   DESCRIPTION: Uses read_data to read from a file and 
 *					tracks the position in the file
 *   INPUTS: fd - index of file to read
 *			 buf - buffer to read into
 *			 nbytes - bytes to read into buf
 *   OUTPUTS: buf with read data
 *   RETURN VALUE: ret - #bytes read on success or -1 on failure
 *   SIDE EFFECTS: data from file given to callee
 *					current position in file stored in file_pos
 */ 
int32_t file_read (int32_t fd, uint8_t* buf, int32_t nbytes)
{
	if(fd_array[fd].flags == 0)
		return -1;
	int32_t ret = read_data(fd_array[fd].inode_ptr, fd_array[fd].file_pos, buf, nbytes);
	if (ret != 0)
		fd_array[fd].file_pos += ret;
	else if (ret == -1)
		return -1;
	return ret;
}

/*
 * file_write
 *   DESCRIPTION: place holder function
 *   INPUTS: inputs to match other write operations
 *   OUTPUTS: none
 *   RETURN VALUE: -1 because our filesystem is noe writeable 
 *   SIDE EFFECTS: none
 */
int32_t file_write(uint8_t* buf, int32_t nbytes)
{

	return -1;
}

/*
 * dir_open
 *   DESCRIPTION: fills out an entry in the file descriptor just like for a file
 *   INPUTS: filename - placeholder
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: new directory
 */ 
int32_t dir_open (const uint8_t* filename)
{

	int32_t i;
	for (i=2; i<8; i++){
		if (fd_array[i].flags == 0){					//1 is in use
			fd_array[i].inode_ptr = NULL;
			fd_array[i].flags = 1;
			fd_array[i].file_pos = 0;
			return i;
		}
	}
	return -1;
}

/*
 * dir_close
 *   DESCRIPTION: closes directory like a file
 *   INPUTS: fd - index into descriptor table to close
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success or -1 on failure
 *   SIDE EFFECTS: directory is not usable
 */ 
int32_t dir_close (int32_t fd)
{
	if (fd > MAX_FD || fd < MIN_FD)
		return -1;
	fd_array[fd].flags = 0;
	return 0;
}

/*
 * dir_read
 *   DESCRIPTION: prints through the list of file descriptors on each 
 *					call. When none are left it prints 0s.
 *   INPUTS: fd - index of file to read
 *			 buf - buffer to read into
 *			 nbytes - bytes to read into buf
 *   OUTPUTS: buf with a file name or 0
 *   RETURN VALUE: ret - #bytes read on success or -1 on failure
 *   SIDE EFFECTS: list of files in system. Useful for ls
 */ 
int32_t dir_read (int32_t fd, uint8_t* buf, int32_t nbytes)
{
	if(fd_array[fd].flags == 0)
		return -1;
	//return 0s once all names are read
	if(fd_array[fd].file_pos >= num_entries)
	{	
		return 0;
	}	
	dentry_t dentry;
	dentry_t* dentry_ptr = &dentry;
	uint32_t ret = read_dentry_by_index(fd_array[fd].file_pos, dentry_ptr);
	if(ret == -1)
		return -1;
	uint32_t x;
	for(x=0; x < strlen((int8_t*)dentry.name); x++)
	{
			buf[x] = dentry_ptr->name[x];
	}
	buf[strlen((int8_t*)dentry.name)] = '\n';
	//update global index of file descriptors
	fd_array[fd].file_pos++;
	return strlen((int8_t*)dentry_ptr->name);
}

/*
 * dir_write
 *   DESCRIPTION: place holder function
 *   INPUTS: inputs to match other write operations
 *   OUTPUTS: none
 *   RETURN VALUE: -1 because our filesystem is noe writeable 
 *   SIDE EFFECTS: none
 */
int32_t dir_write(uint8_t* buf, int32_t nbytes)
{
	return -1;
}

/*
 * loader
 *   DESCRIPTION: copies user program from data blocks into 
 *					physical user space
 *   INPUTS: dest - where to load program
 *		     filename - program to load
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executable can now be ran
 */
void loader(uint32_t* dest, const uint8_t* filename){
	dentry_t dentry;
	dentry_t* dentry_ptr = &dentry;
	read_dentry_by_name(filename, dentry_ptr);
	inode_t curr_inode = inode_ptr[dentry_ptr->inode];
	uint32_t size = curr_inode.length/BLOCK_SIZE + 1;
	int i;
	for(i=0; i<size; i++){
		memcpy(dest, data_blocks + BLOCK_SIZE*curr_inode.inode_data_blocks[i], BLOCK_SIZE);
		dest += BLOCK_OFFSET;
	}
}
