/* 

    This is a simple example of a hardware interrupt handler.
    As an example, this implements a timer, which in turn 
    triggers a function to be called after the end of the time quantum.
    The function is implemented in 'handle_interrupt'.

*/

#ifndef _EOQ_TIMER_H_
#define _EOQ_TIMER_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "interrupts.H"
#include "simple_timer.H"
#include "thread.H"

/*--------------------------------------------------------------------------*/
/* S I M P L E   T I M E R  */
/*--------------------------------------------------------------------------*/

class EOQTimer : public SimpleTimer {

private:

	/* time quantum to interrrupt a process */
	int quantum;
	
	void set_quantum(int _quantum);
	/* Set the interrupt frequency for the simple timer. */


public :
	EOQTimer(int _hz, int _quantum);
	//EOQTimer(int _hz, int _quantum);
	/* Initialize the simple timer, and set its frequency. */

	virtual void handle_interrupt(REGS *_r);
	/* This must be installed as the interrupt handler for the timer 
	when the system gets initialized. (e.g. in "kernel.C")  
	*/

};

#endif
