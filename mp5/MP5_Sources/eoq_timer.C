/* 
    
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "interrupts.H"
#include "simple_timer.H"
#include "eoq_timer.H"
#include "thread.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

//int EOQTimer::quantum;

EOQTimer::EOQTimer(int _hz, int _quantum) :  SimpleTimer (_hz){
	set_quantum(_quantum);
	Console::puts("EOQTimer constructed/derived from  SimpleTimer\n");
}

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S i m p l e T i m e r */
/*--------------------------------------------------------------------------*/

void EOQTimer::set_quantum(int _quantum) {
    quantum = _quantum;
}

void EOQTimer::handle_interrupt(REGS *_r) {
/* What to do when timer interrupt occurs? In this case, we update "ticks",
   and maybe update "seconds".
   This must be installed as the interrupt handler for the timer in the 
   when the system gets initialized. (e.g. in "kernel.C") */

    /* Increment our "ticks" count */
    ticks++;

    /* Call an interrupt every  after _quantum quantums have passed. */
    if (ticks >= hz/20)//quantum )
    {
        //seconds++;
        ticks = 0;
        Console::puts("\nOne time quantum has passed, yielding to next thread\n");
        Thread::yield_thread();
    }
}
