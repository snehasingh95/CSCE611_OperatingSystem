/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include "blocking_disk.H"
#include "assert.H"
#include "utils.H"
#include "console.H"
#include "scheduler.H"

extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
//Queue* BlockingDisk::disk_queue;

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
  disk_queue = new Queue();
  Console::puts("BlockingDisk Initiated.\n");
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

bool BlockingDisk::ready() {
	return SimpleDisk::is_ready();
}

void BlockingDisk::wait_until_ready() {
	if(!BlockingDisk::ready()){
		Thread* curr_thread = Thread::CurrentThread();
		disk_queue -> enqueue(curr_thread);
		Console::puts("Disk not Available. Thread Added to Blocked Disk Queue.\n");
		SYSTEM_SCHEDULER -> yield();
	}
}

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::read(_block_no, _buf);
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  SimpleDisk::write(_block_no, _buf);
}
