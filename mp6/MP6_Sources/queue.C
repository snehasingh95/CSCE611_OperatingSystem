/*
 File: queue.C
 
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

#include "queue.H"
#include "thread.H"
#include "machine.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   Q u e u e  */
/*--------------------------------------------------------------------------*/

/*struct Node {
	Thread* _thread;
	struct Node* _next_node;
};*/

Queue::Queue(){
	front = tail = NULL;
	queue_size = 0;
}

bool Queue::empty(){
	return front == NULL;
}

Thread* Queue::peek(){
	if(empty()) return NULL;
	return front -> _thread;
}

void Queue::enqueue(Thread* _thread){
	Node* new_node = new Node;
	new_node -> _thread = _thread;
	new_node -> _next_node = NULL;
	
	bool interrupts_enabled = Machine::interrupts_enabled();
	Console::puts("Initial Interrupt State: ");Console::puti(interrupts_enabled);Console::puts("\n");
	
	if(interrupts_enabled){
		Machine::disable_interrupts();
		Console::puts("Disabled Interrupts Before Critical Section.\n");
	}
	
	if(empty()){
		front = tail = new_node;
	}else{
		tail -> _next_node = new_node;
		tail = new_node;
	}
	queue_size++;
	
	if(interrupts_enabled){
		Machine::enable_interrupts();
		Console::puts("Enabled Interrupts After critical section.\n");
	}

	//printQueue();
	Console::puts("Added Thread To Queue.\n");
}
	
Thread* Queue::dequeue(){
	if(empty()){
	  	return NULL;
  	}
  	
  	bool interrupts_enabled = Machine::interrupts_enabled();
  	//Console::puts("Initial Interrupt State: ");Console::puti(interrupts_enabled);Console::puts("\n");
	if(interrupts_enabled){
		Console::puts("Disabled Interrupts Before Critical Section.\n");
		Machine::disable_interrupts();
	}

  	Node* node = front;
  	
  	if(front == tail){
  		tail = NULL;
  	}
  	front = front -> _next_node;
  	
  	queue_size--;
	
	if(interrupts_enabled){
		Machine::enable_interrupts();
		Console::puts("Enabled Interrupts After Critical Section.\n");
	}

	return node->_thread;
}

void Queue::removeNode(Thread* _thread){
	if(empty()){
	  	return;
  	}
  	
  	bool interrupts_enabled = Machine::interrupts_enabled();
  	//Console::puts("Initial Interrupt State: ");Console::puti(interrupts_enabled);Console::puts("\n");
	if(interrupts_enabled){
		Console::puts("Disabled Interrupts Before Critical Section.\n");
		Machine::disable_interrupts();
	}

  	Node* node = front;

  	if(node -> _thread -> ThreadId() == _thread -> ThreadId()){
  		front = front -> _next_node;
  		delete node;
  		queue_size--;
  	}
  	else {
	  	while(node->_next_node!=NULL){
	  		Node* nextNode = node -> _next_node;
	  		if(nextNode -> _thread -> ThreadId() == _thread -> ThreadId()){
	  			node -> _next_node = nextNode -> _next_node;
	  			delete nextNode;
	  			queue_size--;
	  			break;
	  		}
	  		node = nextNode;
	  	}
  	}
	
	if(interrupts_enabled){
		Machine::enable_interrupts();
		Console::puts("Enabled Interrupts After Critical Section.\n");
	}
}

void Queue::printQueue() {
	Console::puts("Queue is: ");
	Node* node = front;
	while(node!=NULL){
		Console::puti(node->_thread->ThreadId());
		Console::puts("->");
		node = node -> _next_node;
	}
	Console::puts("\n");
}
