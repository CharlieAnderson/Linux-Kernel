#include "rtc.h"
#include "lib.h"

volatile int int_flag = 0;
static int rate_min = 6;
static int rate_max = 15;

/*
 * void rtc_init()
 * Description: Enables the rtc interrupt
 * Inputs: None
 * Outputs: None
 */
void rtc_init() {
	//printf("Initializing RTC.\n");
	cli();
	outb(REG_B, INDEX_RTC_PORT);
	uint8_t prev = inb(DATA_RTC_PORT);
	outb(REG_B, INDEX_RTC_PORT);
	outb(prev | DATA_MASK, DATA_RTC_PORT);
	sti();
	enable_irq(RTC_IRQ);

	//printf("RTC is initialized. \n");
}

/*
 * void rtc_handler()
 * Description: Handler for RTC interrupts
 * Inputs: None
 * Outputs: None
 */
void rtc_handler() {
	cli();
	outb(REG_C,INDEX_RTC_PORT);
	inb(DATA_RTC_PORT);
	send_eoi(RTC_IRQ);
	int_flag = 1;
	sti();
}


/*
You will need to write the open, read, write, and close functions for the real-time clock (RTC) and demonstrate
that you can change the clock frequency. You will need to do some research on how the RTC works and what the
device driver needs to do to communicate with it. Virtualizing the RTC is not required, but does make testing easier
when you run multiple programs with the RTC open. Again, see Appendix B for how each function should work.
*/


/*
Note that some system calls need to synchronize with interrupt handlers. 
For example, the read system call made on the RTC device should wait 
until the next RTC interrupt has occurred before it returns. 
Use simple volatile flag vari- ables to do this synchronization 
(e.g., something like int rtc interrupt occurred;) when possible
 (try some- thing more complicated only after everything works!), 
and small critical sections with cli/sti. 
For example, writing to the RTC should block interrupts to interact with the device. 
Writing to the terminal also probably needs to block interrupts, if only briefly, 
to update screen data when printing (keyboard input is also printed to 
the screen from the interrupt handler).
*/



/*
3. int32 t read (int32 t fd, void* buf, int32 t nbytes);
4. int32 t write (int32 t fd, const void* buf, int32 t nbytes); 
5. int32 t open (const uint8 t* filename);
6. int32 t close (int32 t fd);
*/

/*
 * int rtc_change_freq(int32_t freq)
 * Description: Function to change the frequency of the RTC
 * Inputs: Frequency to change RTC to
 * Outputs: Always 0
 */
int rtc_change_freq(int32_t freq)
{
	if (!((freq != 0) && !(freq & (freq - 1)))) {
		printf("Error: The desired frequency %dhz is not a power of 2.\n", freq);
		return -1; 
	}

	if(freq < MIN_FREQ) {
		printf("Warning: The desired frequency %dhz is too low, clipping to 2 Hz.\n", freq);
		freq = MIN_FREQ; // Min frequency is 2 Hz
	}

	if(freq > MAX_FREQ) {
		printf("Warning: The desired frequency %dhz is too high, capping at 1024 Hz.\n", freq);
		freq = MAX_FREQ; // Max frequency is 1024 Hz
	}

	int rate; 
	switch(freq) { //default rate setting is 15 (2 Hz)
		case MIN_FREQ:
			rate = rate_max;
			break;
		case 4:
			rate = 14;
			break;
		case 8:
			rate = 13;
			break;
		case 16:
			rate = 12;
			break;
		case 32:
			rate = 11;
			break;
		case 64:
			rate = 10;
			break;
		case 128:
			rate = 9;
			break;
		case 256:
			rate = 8;
			break;
		case 512:
			rate = 7;
			break;
		case MAX_FREQ:
			rate = rate_min;
			break;
		default: 
			rate = rate_min;
			break;
	}

	cli();
	outb(REG_A, INDEX_RTC_PORT);	//set index to register A, disable NMI
	char prev = inb(DATA_RTC_PORT);	//get initial value of register A
	outb(REG_A, DATA_RTC_PORT);		//reset index to A
	outb(((prev & 0xF0) | rate), DATA_RTC_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.
	sti();

	return 0;
}


/*
 * int rtc_read(int32_t fd, uint8_t* buf, int32_t nbytes)
 * Description: Waits for a RTC interrupt flag to occur.
 * Inputs: File descriptor, buffer, nbytes 
 * Outputs: Always 0
 */
int rtc_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
	sti();
	while(int_flag == 0){}
	int_flag = 0;

	return 0;
}

/*
 * int rtc_write(uint8_t* buf, int32_t nbytes)
 * Description: Changes the RTC to the given rate
 * Inputs: Buffer containing the frequency, nbytes (should be 4)
 * Outputs: -1 if failure, 0 if success
 */
int rtc_write(uint8_t* buf, int32_t nbytes)
{
	if (*buf == NULL) {
		return -1;
	}
	if (nbytes != 4) {
		return -1;
	}
	rtc_change_freq(*buf);
	
	return 0;
}

/*
 * int rtc_open(const uint8_t* filename)
 * Description: Opens the RTC
 * Inputs: Filename (unused)
 * Outputs: 0 if success
 */
int rtc_open(const uint8_t* filename)
{	printf("RTC open called.\n");
	printf("Setting default rate of 2hz to RTC.\n");
	rtc_change_freq(2); //default rtc rate is 2hz 
	
	return 0;
}

/*
 * int rtc_close(int32_t fd)
 * Description: Closes the RTC
 * Inputs: File descriptor (unused)
 * Outputs: Always 0
 */
int rtc_close(int32_t fd)
{
	printf("RTC close called.\n");
	
	return 0;
}
