#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

//fops functions for terminal
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t terminal_write(uint8_t* buf, int32_t nbytes);
int32_t terminal_close(int32_t fd);

#endif /* _TERMINAL_H */
