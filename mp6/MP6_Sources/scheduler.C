/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "blocking_disk.H"


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

/*struct Node {
	Thread* _thread;
	struct Node* _next_node;
};*/

Queue* Scheduler::ready_queue;
BlockingDisk* Scheduler::blocking_disk;
	
Scheduler::Scheduler() {
	//adding dummy node to the ready queue
	Scheduler::ready_queue = new Queue();
	Scheduler::blocking_disk = NULL;
	Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
	//yield to a blocked thread if ready before yielding to next ready thread
	
	if(Scheduler::blocking_disk!=NULL && Scheduler::blocking_disk->ready() && !Scheduler::blocking_disk->disk_queue->empty()) {
		Thread* next_disk_thread = Scheduler::blocking_disk-> disk_queue -> dequeue();
		Console::puts("Yielding To Disk Thread: "); Console::puti(next_disk_thread->ThreadId()); Console::puts(" in Blocked Queue.\n");
		Thread::dispatch_to(next_disk_thread);
	}else{
	
		if(Scheduler::ready_queue->empty()){
		  	Console::puts("Ready Queue Is Empty.\n");
		  	assert(false);
	  	}
	  	//adding current thread to end of ready queue not needed, as current thread is premempted by kernal before yielding.
  	
	  	//dispatch to next node
	  	Thread* next_ready_thread = Scheduler::ready_queue -> dequeue();
	  	
	  	//Scheduler::ready_queue->printQueue();
	  	Console::puts("Yielding To Next Thread: "); Console::puti(next_ready_thread->ThreadId()); Console::puts(" in Ready Queue.\n");
	  	Thread::dispatch_to(next_ready_thread);
	}
}

void Scheduler::resume(Thread * _thread) {
  	Scheduler::add(_thread);
}

void Scheduler::add(Thread * _thread) {
	Scheduler::ready_queue -> enqueue(_thread);
	//Scheduler::ready_queue->printQueue();
	Console::puts("Added Thread To Ready Queue.\n");
}

void Scheduler::terminate(Thread * _thread) {
	//remove the _thread from the ready queue, in case the thread sucides, before thread shutdown.
	Scheduler::ready_queue -> removeNode(_thread);
	//Scheduler::ready_queue->printQueue();
	Console::puts("Thread Terminated.\n");
	Scheduler::yield();
}

void Scheduler::add_blocking_disk(BlockingDisk* blocking_disk){
	Scheduler::blocking_disk = blocking_disk;
}
