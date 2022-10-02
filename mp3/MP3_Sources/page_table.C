#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;

#define VALID_BIT 1 //bit 0 -> 1=valid, 0=absent
#define WRITE_BIT 2 //bit 1 -> 1=read/write, 0=read-only
#define USER_BIT 14 //bit 2 -> 1=user, 0=kernel
#define MSB_MASK 0x80000000
#define PTE_INDX_MASK 0x3ff
#define PT_ADDR_MASK 0xfffff000

void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   PageTable::kernel_mem_pool = _kernel_mem_pool;
   PageTable::process_mem_pool = _process_mem_pool;
   PageTable::shared_size = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   page_directory = (unsigned long *)(kernel_mem_pool-> get_frames(1)* PAGE_SIZE);
   unsigned long * page_table = (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
   
   //shared memory setup (page table)
   unsigned long n_shared_frames = (PageTable::shared_size)/PAGE_SIZE;
   
   unsigned long i, frame_address =0, n_entries=PAGE_SIZE/4; //each entry is 4 bytes long.
   //shared memory space -> 4MB/4KB = 1024
   for(i=0;i<n_shared_frames;i++) {
   	page_table[i] = frame_address | WRITE_BIT | VALID_BIT;
   	frame_address += PAGE_SIZE;
   }
   
   //remaining memory space = 1024-1024 = 0
   for(i;i<n_entries;i++) {
   	page_table[i] = frame_address | WRITE_BIT;
   	frame_address += PAGE_SIZE;
   }
   
   //page directory setup
   page_directory[0] = (unsigned long) page_table | WRITE_BIT | VALID_BIT;
   
   for(i=1;i<n_entries-1;i++) {
   	page_directory[i] = 0 | WRITE_BIT;
   }
   //last entry to point back to page directory for recursive page table loookup
   page_directory[n_entries-1] = (unsigned long) page_directory | WRITE_BIT | VALID_BIT;
   
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   current_page_table = this;
   write_cr3((unsigned long) current_page_table-> page_directory);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   paging_enabled = 1;
   write_cr0(read_cr0() | MSB_MASK); //setting the MSB of cr3 to enable paging.
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  unsigned long logical_address = read_cr2();
  unsigned long pt_indx = (logical_address >> 12) & PTE_INDX_MASK;
  unsigned long pd_indx = logical_address >> (12+10) ;
  
  unsigned long *curr_pd_address = (unsigned long *) read_cr3(); //ptbr_value
  unsigned long error_word = _r->err_code;
  
  if ((error_word & VALID_BIT) == 0) { //invalid entry in pde/pte
  	unsigned long pde = curr_pd_address[pd_indx];
  	if((pde & VALID_BIT) ==0) { //invalide entry in pde
  		curr_pd_address[pd_indx] = (unsigned long)(kernel_mem_pool->get_frames(1) * PAGE_SIZE) | WRITE_BIT | VALID_BIT;
  		unsigned long *curr_pt_address = (unsigned long *)(pde & PT_ADDR_MASK); 
  		
  		unsigned long i,n_entries=PAGE_SIZE/4;
  		for(i=0;i<n_entries;i++){
  			curr_pt_address[i] = 0 | USER_BIT;
  		}
  		curr_pt_address[pt_indx] = (process_mem_pool->get_frames(1) * PAGE_SIZE) | USER_BIT | WRITE_BIT | VALID_BIT;
  	}
  	else { //invalid entry in pte
  		unsigned long *curr_pt_address = (unsigned long *)(pde & PT_ADDR_MASK); 
  		curr_pt_address[pt_indx] = (process_mem_pool->get_frames(1) * PAGE_SIZE) | USER_BIT | WRITE_BIT | VALID_BIT;
  	}
  }else {
  	Console::puts("Some error has occurred\n");
  }
  Console::puts("handled page fault\n");
}

