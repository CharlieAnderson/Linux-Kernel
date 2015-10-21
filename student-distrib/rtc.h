#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

/* Ports that each PIC sits on */
#define INDEX_RTC_PORT 0x70
#define DATA_RTC_PORT  0x71
#define REG_A 0x0A
#define REG_B 0x0B
#define REG_C 0x0C

#define RATE_CONST  32768
#define MAX_FREQ 1024
#define MIN_FREQ 2
#define DATA_MASK 0x40
#define RTC_IRQ 8

/* Initialize the rtc */
void rtc_init();
//open and close communication with rtc
extern void rtc_handler();
extern int rtc_change_freq(int32_t freq);
//waits for rtc interrupt to return
extern int rtc_read(int32_t fd, uint8_t* buf, int32_t nbytes);
//change the rate
extern int rtc_write(uint8_t* buf, int32_t nbytes);
//starts rtc at default frequency
extern int rtc_open(const uint8_t* filename);
//placeholder for fops
extern int rtc_close(int32_t fd);

#endif /* _RTC_H */
