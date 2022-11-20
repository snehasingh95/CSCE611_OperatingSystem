/*
File: queue.H
*/

#ifndef QUEUE_h
#define QUEUE_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* QUEUE */
/*--------------------------------------------------------------------------*/

struct Node {
	Thread* _thread;
	struct Node* _next_node;
};

class Queue {
	private:
	struct Node* front;
	struct Node* tail;
	int queue_size;
	
	public:
	Queue();
	
	virtual bool empty();
	
	virtual Thread* peek();
	
	virtual void enqueue(Thread* _thread);
	
	virtual Thread* dequeue();
	
	virtual void removeNode(Thread* _thread);
	
	virtual void printQueue();
};

#endif
