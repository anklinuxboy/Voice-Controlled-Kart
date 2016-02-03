#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <asm/hardware.h>
#include <asm/arch/pxa-regs.h>
#include <asm/uaccess.h>
#include <asm-arm/arch/hardware.h>
#include <asm/gpio.h>

/**********************************************************************************
Prepared by Team BlueGum
Juan Jhong Chung, Laura Kamfonik, Ankit Sharma

EC535 Fall 2014 Final Project

Kernel module for controlling kart movement with a SN754410 motor driver

**********************************************************************************/

//GPIO pins
#define L_FORW_PIN 28		// Connect to pin 2 (1A) on the SN754410
#define L_BACK_PIN 29		// Connect to pin 7 (2A) on the SN754410
#define R_FORW_PIN 30		// Connect to pin 15 (4A) on the SN754410
#define R_BACK_PIN 31 		// Connect to pin 10 (3A) on the SN754410

//PWM pin
#define SPEED_PIN 16		// Connect to pin 1 (1,2 EN) and pin 9 (3,4) EN on the SN754410

//Useful constants
#define LOW 0
#define HIGH 1
#define MSG_BUF_SIZE 128
#define CONVERSION_BASE 10

#define TURN_TIME 250 		//how long each call to turnKart() turns on the kart for, in milliseconds

#define PROC_SIZE sizeof(int)	//size of the proc buffer, currently it only needs to hold one integer

int robokart_init(void);
void robokart_exit(void);

//kart movement functions
int moveKart(int forward, int back);
void stopKart (unsigned long data);  	//kart_timer expiration function
int turnKart (int forward, int back);
//void busyforgum (void);
void changeSpeed (int speed);
void kartDance (void);
void manualStop (void);			//duplicate of stopKart() for manually stopping the kart
								//because calling timer function stopKart() manually can cause kernel panic

//proc functions	
static int proc_read( char *page, char **start, off_t off, int count, int *eof, void *data );
static ssize_t proc_write( struct file *filp, const char __user *buff, unsigned long len, void *data );

//kart variables
struct proc_dir_entry *proc_entry;
char * proc_buffer;
int move_time = 5000;			//how long each call to moveKart() turns on the kart for, in milliseconds
char msg_buf[MSG_BUF_SIZE]; 	//buffer for sending kart messages to userspace
struct timer_list * kart_timer;

module_init(robokart_init);
module_exit(robokart_exit);
MODULE_LICENSE("GPL"); 

	
int robokart_init(void)
{
	//setup GPIO & PWM output pins
	pxa_gpio_mode(GPIO16_PWM0_MD);
	gpio_direction_output(L_FORW_PIN, LOW);
	gpio_direction_output(L_BACK_PIN, LOW);
	gpio_direction_output(R_FORW_PIN, LOW);
	gpio_direction_output(R_BACK_PIN, LOW);

	/*** DUTY CYCLE SETUP ***/
	
	//enable PWM0 clock in CKEN register
	pxa_set_cken(CKEN0_PWM0,1);
	CKEN |= CKEN0_PWM0;

	PWM_CTRL0 = 1; // + 1 = pulse number each counter
	
	PWM_PERVAL0 = 99; // + 1 = total counter
	
	// clear the duty cycle bits 0-9 so they can be set
	PWM_PWDUTY0 &= ~(0x3FF);
	
	// Duty cycle must be >=80% because the max voltage is only 3.3V
	PWM_PWDUTY0 = 90;
	
	// set bit 10 to 1 to disable duty cycle so default speed is high
	PWM_PWDUTY0 |= (1 << 10);
	
	
	/*** SETUP PROC FILE FOR USERSPACE INTERACTION ***/
	
	/* Allocate buffer for the proc entry */
	proc_buffer = kmalloc(PROC_SIZE, GFP_KERNEL);
	if (!proc_buffer)
	{
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		return -ENOMEM; 
	}
	memset(proc_buffer, 0, PROC_SIZE);

	//create the proc entry
	proc_entry = create_proc_entry( "robokart", 0644, NULL );

	if (proc_entry == NULL)
	{
		printk(KERN_INFO "Couldn't create proc entry\n");
		kfree(proc_buffer);
		return -ENOMEM;
	}

	//set the proc file operations
	proc_entry->read_proc = proc_read;
	proc_entry->write_proc = proc_write;
	proc_entry->owner = THIS_MODULE;
	
	
	/*** KART MOVEMENT TIMER SETUP ***/
	// mod_timer() will be called by the kart movement functions
	
	kart_timer = (struct timer_list *) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	setup_timer(kart_timer, stopKart, 0);
	
	printk("Kart module successfully loaded.\n");
	return 0;
	
}

void robokart_exit(void)
{
	if (proc_buffer)
		kfree(proc_buffer);
	
	if (kart_timer)
		kfree(kart_timer);

	printk("Kart module successfully unloaded.\n");
}

void changeSpeed(int spd)
{
	if (spd)
		PWM_PWDUTY0 |= (1 << 10);	// set bit 10 of the duty cycle register to 1, disabling PWM and setting speed to high
	else
		PWM_PWDUTY0 &= ~(1 << 10);	// set bit 10 of the duty cycle register to 0, enabling PWM and setting speed to low
						// duty cycle for low speed is established in robokart_init()
}


void stopKart (unsigned long data)
{	
	/*	
	This function should only be used as the kart_timer expiration function.
	Calling it manually interferes in some way with the built-in timer code and can cause kernel panic.
	If a manual stop call is needed, use the function manualStop() defined below.
	*/

	// stop kart by setting set all motor pins to 0
	gpio_set_value(L_FORW_PIN, LOW);
	gpio_set_value(L_BACK_PIN, LOW);
	gpio_set_value(R_FORW_PIN, LOW);
	gpio_set_value(R_BACK_PIN, LOW);
}

void kartDance (void)
{
	// make the kart dance!
	
	int i = 0;
	for (i = 0; i < 2; i++)
	{
	
	//go forward
	gpio_set_value(L_FORW_PIN, HIGH);
	gpio_set_value(L_BACK_PIN, LOW);
	gpio_set_value(R_FORW_PIN, HIGH);
	gpio_set_value(R_BACK_PIN, LOW);

	msleep(500);

	//turn right
	gpio_set_value(L_FORW_PIN, LOW);
	gpio_set_value(L_BACK_PIN, HIGH);
	gpio_set_value(R_FORW_PIN, HIGH);
	gpio_set_value(R_BACK_PIN, LOW);

	msleep(1000);

	//turn left
	gpio_set_value(L_FORW_PIN, HIGH);
	gpio_set_value(L_BACK_PIN, LOW);
	gpio_set_value(R_FORW_PIN, LOW);
	gpio_set_value(R_BACK_PIN, HIGH);

	msleep(500);	
	
	//go backward
	gpio_set_value(L_FORW_PIN, LOW);
	gpio_set_value(L_BACK_PIN, HIGH);
	gpio_set_value(R_FORW_PIN, LOW);
	gpio_set_value(R_BACK_PIN, HIGH);

	msleep(250);	
	}
	
	mod_timer(kart_timer, jiffies + msecs_to_jiffies(500));
}

void manualStop (void)
{
	/*	
	This function is a duplicate of stopKart() to be used for stopping the kart manually.
	Calling stopKart() manually interferes in some way with the built-in timer code and can cause kernel panic.
	Do not use stopKart() for anything but the kart_timer expiration; use this function instead.
	*/

	// set all output pins to 0
	gpio_set_value(L_FORW_PIN, LOW);
	gpio_set_value(L_BACK_PIN, LOW);
	gpio_set_value(R_FORW_PIN, LOW);
	gpio_set_value(R_BACK_PIN, LOW);

}


int moveKart(int forward, int back)
{
	// forward = HIGH, back = LOW moves the kart forward
	// reversing the values moves the kart backward
	gpio_set_value(L_FORW_PIN, forward);
	gpio_set_value(L_BACK_PIN, back);
	gpio_set_value(R_FORW_PIN, forward);
	gpio_set_value(R_BACK_PIN, back);
	
	// stop the kart motion after move_time milliseconds
	mod_timer(kart_timer, jiffies + msecs_to_jiffies(move_time));
	
	return 0;
}

int turnKart (int forward, int back)
{
	// to turn right, set left wheel to go forward, and right wheel to go backward
	// to turn left, do the opposite

	gpio_set_value(L_FORW_PIN, forward);
	gpio_set_value(L_BACK_PIN, back);
	gpio_set_value(R_FORW_PIN, back);
	gpio_set_value(R_BACK_PIN, forward);

	// stop turning after TURN_TIME has elapsed
	mod_timer(kart_timer, jiffies + msecs_to_jiffies(TURN_TIME));
	
	return 0;
}


static ssize_t proc_write( struct file *filp, const char __user *buff, unsigned long len, void *data )
{
	int proc_val;

	// some pointers for simple_strtoul
	char *endp, *pbptr = proc_buffer;
	
	memset(proc_buffer, 0, PROC_SIZE);
	
	//a number corresponding to an instruction will be written to /proc/robokart	
	if (copy_from_user(proc_buffer, buff, len ))
		return -EFAULT;
	
	//convert the number to an int
	proc_val = simple_strtoul(pbptr, &endp, CONVERSION_BASE);
	
	
	/* List of /proc/robokart command values:
		0 - manual stop
		1 - go forward
	 	2 - go backward
		3 - turn right
		4 - turn left
		5 - set speed to fast (default, disable PWM)
		6 - set speed to slow (enable PWM)
		7 - dance
		8 - change move time to 5 seconds (default)
		9 - change move time to 10 seconds
	*/

    switch (proc_val)
	{
		case 0:	//manual stop 
		strncpy(msg_buf, "Stopping cart manually on command 0", MSG_BUF_SIZE);
		manualStop();
		break;
		
		case 1:  //forward
		strncpy(msg_buf, "Moving forward", MSG_BUF_SIZE);
		moveKart(HIGH, LOW);
		break;
	
		case 2:  //backward
		strncpy(msg_buf, "Moving backward", MSG_BUF_SIZE);
		moveKart(LOW, HIGH);
		break;
		
		case 3: //turn right
		strncpy(msg_buf, "Turning right", MSG_BUF_SIZE);
		turnKart(HIGH,LOW);
		break;
		
		case 4: //turn left
		strncpy(msg_buf, "Turning left", MSG_BUF_SIZE);
		turnKart(LOW,HIGH);
		break;
		
		case 5: // go fast
		strncpy(msg_buf, "Changing speed to HIGH", MSG_BUF_SIZE);
		changeSpeed(HIGH);
		break;
		
		case 6: // go slow
		strncpy(msg_buf, "Changing speed to LOW", MSG_BUF_SIZE);
		changeSpeed(LOW);
		break;

		case 7: //dance
		strncpy(msg_buf, "Yippeeee!", MSG_BUF_SIZE);
		kartDance();
		break;

		case 8: //short
		strncpy(msg_buf, "Changing move time to 5 seconds", MSG_BUF_SIZE);
		move_time = 5000;
		break;

		case 9: //long
		strncpy(msg_buf, "Changing move time to 10 seconds", MSG_BUF_SIZE);
		move_time = 10000;
		break;		
	
		default: //error case
		strncpy(msg_buf, "Sorry, your input was not understood.", MSG_BUF_SIZE);
	}
	
	return len;
}

static int proc_read( char *page, char **start, off_t off, int count, int *eof, void *data )
{
	// Called when the bluetooth server application reads from /proc/robokart
	// Sends the movement message in msg_buf to userspace
	
	int len = 0;

	// tell userspace when it is done reading
	if (off > 0) 
	{
		*eof = 1;
		return 0;
	}
	
	// Alert userspace if it is reading an empty buffer
	if (msg_buf[0] == 0)
		strncpy(msg_buf, "No message in buffer...\0", MSG_BUF_SIZE);

	// write message to userspace
	len = sprintf(page, "%s", msg_buf);

	// clear the buffer for the next command
	memset(msg_buf,0,MSG_BUF_SIZE);
	
	return len;
}
