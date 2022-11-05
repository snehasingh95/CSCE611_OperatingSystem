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

Node* Scheduler::ready_queue_front;
Node* Scheduler::ready_queue_tail;
int Scheduler::thread_count;
	
Scheduler::Scheduler() {
	//adding dummy node to the ready queue
	Scheduler::thread_count = 0;
	Scheduler::ready_queue_front = Scheduler::ready_queue_tail = NULL;
	Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  	if(Scheduler::ready_queue_front==NULL){
	  	Console::puts("Ready Queue Is Empty.\n");
	  	assert(false);
  	}
  	
  	//adding current thread to end of ready queue not needed, as current thread is premempted by kernal before yielding.
  	
  	//dispatch to next node
  	Node* next_ready_thread = Scheduler::ready_queue_front;
  	if(Scheduler::ready_queue_front == Scheduler::ready_queue_tail){
  		Scheduler::ready_queue_tail = NULL;
  	}
  	Scheduler::ready_queue_front = Scheduler::ready_queue_front -> _next_node;
  	
  	Thread::dispatch_to(next_ready_thread -> _thread);
}

void Scheduler::resume(Thread * _thread) {
  	Scheduler::add(_thread);
}

void Scheduler::add(Thread * _thread) {
	Node* new_ready_thread_node = new Node;
	new_ready_thread_node -> _thread = _thread;
	new_ready_thread_node -> _next_node = NULL;
	
	//Machine::disable_interrupts();
	
	if(Scheduler::ready_queue_front==NULL){
		//only dummy node in ready queue
		Scheduler::ready_queue_front = new_ready_thread_node;
		Scheduler::ready_queue_tail = new_ready_thread_node;
	}else{
		Scheduler::ready_queue_tail -> _next_node = new_ready_thread_node;
		Scheduler::ready_queue_tail = new_ready_thread_node;
	}
	Scheduler::thread_count++;
	
	//Machine::enable_interrupts();
	
	Console::puts("Added Thread To Ready Queue.\n");
}

void Scheduler::terminate(Thread * _thread) {
	//remove the _thread from the ready queue, in case the thread sucides, before thread shutdown.
	
	//Machine::disable_interrupts();
	if(Scheduler::ready_queue_front==NULL){
	  	return;
  	}

  	Node* node = Scheduler::ready_queue_front;

  	if(Scheduler::ready_queue_front -> _thread -> ThreadId() == _thread -> ThreadId()){
  		Scheduler::ready_queue_front = Scheduler::ready_queue_front -> _next_node;
  		delete node;
  		Scheduler::thread_count--;
  		Scheduler::yield();
	  	return;
  	}
  	
  	while(node->_next_node!=NULL){
  		Node* nextNode = node -> _next_node;
  		if(nextNode -> _thread -> ThreadId() == _thread -> ThreadId()){
  			node -> _next_node = nextNode -> _next_node;
  			delete nextNode;
  			break;
  		}
  		node = nextNode;
  	}
	
	//Machine::enable_interrupts();
	Scheduler::thread_count--;
	Scheduler::yield();
}

/*void Scheduler::printReadyQueue(){
	Console::puts("Ready Queue is: ");
	Node* node = Scheduler::ready_queue_front;
	while(node!=NULL){
		Console::puti(node->_thread->ThreadId());
		Console::puts("->");
		node = node -> _next_node;
	}
	Console::puts("\n");
}*/
