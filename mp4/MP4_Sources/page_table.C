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
#define USER_BIT 4 //bit 2 -> 1=user, 0=kernel
#define MAKE_INVALID 0xFFFFFFFE
#define MSB_MASK 0x80000000
#define PTE_INDX_MASK 0x3ff
#define PD_ADDR_MASK 0xfffff000
#define PT_ADDR_MASK 0xffc00000


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
	page_directory = (unsigned long *)(process_mem_pool-> get_frames(1)* PAGE_SIZE);
	unsigned long * page_table = (unsigned long *)(process_mem_pool->get_frames(1) * PAGE_SIZE);

	//shared memory setup (page table)
	unsigned long n_shared_frames = (PageTable::shared_size)/PAGE_SIZE;

	unsigned long i, frame_address =0, n_entries=PAGE_SIZE/4; //each entry is 4 bytes long.
	//shared memory space -> 4MB/4KB = 1024
	for(i=0;i<n_shared_frames;i++) 
	{
		page_table[i] = frame_address | WRITE_BIT | VALID_BIT;
		frame_address += PAGE_SIZE;
	}

	//remaining memory space = 1024-1024 = 0
	for(i;i<n_entries;i++) 
	{
		page_table[i] = frame_address | WRITE_BIT;
		frame_address += PAGE_SIZE;
	}

	//page directory setup
	page_directory[0] = (unsigned long) page_table | WRITE_BIT | VALID_BIT;

	for(i=1;i<n_entries-1;i++) 
	{
		page_directory[i] = 0 | WRITE_BIT;
	}
	//last entry to point back to page directory for recursive page table loookup
	page_directory[n_entries-1] = (unsigned long) page_directory | WRITE_BIT | VALID_BIT;

	//setting the vmpools to NULL
	n_registered_vmpools = 0;
	for(unsigned int i=0;i<VM_POOLS_MAX;i++) 
	{
		registered_vmpools[i]=NULL;
	}

	Console::puts("Constructed Page Table object\n");
}

void PageTable::load()
{
	current_page_table = this;
	write_cr3((unsigned long)(current_page_table-> page_directory));
	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{	
	paging_enabled = 1;
	write_cr0(read_cr0() | MSB_MASK); //setting the MSB of cr3 to enable paging.
	Console::puts("Enabled paging\n");
}

unsigned long* PageTable::PDE_address(unsigned long address)
{
	//returns logical address of PDE
	return (unsigned long *)(((address>>(10+12))<<2) | PD_ADDR_MASK);
}

unsigned long* PageTable::PTE_address(unsigned long address)
{
	//returns logical address of PTE
	return (unsigned long *)(((address>>(12))<<2) | PT_ADDR_MASK);
}    

void PageTable::handle_fault(REGS * _r)
{
	unsigned long faulty_logical_address = read_cr2();
	unsigned long error_word = _r->err_code;
	
	if ((error_word & VALID_BIT) == 1) 
	{
		Console::puts("Protection fault\n");
		assert(false);
		return;
	}
	
	unsigned int vm_pool_index = 0;
	unsigned int curr_vm_pool_count = current_page_table -> n_registered_vmpools;
	
	for(;vm_pool_index<curr_vm_pool_count;vm_pool_index++)
	{
		if(current_page_table->registered_vmpools[vm_pool_index]!=NULL 
		    && current_page_table->registered_vmpools[vm_pool_index]->is_legitimate(faulty_logical_address))
		{
			break;
		}
	}
	
	if(vm_pool_index==curr_vm_pool_count)
	{
		Console::puts("Illegitimate Page\n");
		assert(false);
		return;
	}
	
	VMPool* curr_vm_pool = current_page_table->registered_vmpools[vm_pool_index];
	
	unsigned long pde_indx = faulty_logical_address >> (12+10) ;
	unsigned long pte_indx = (faulty_logical_address >> 12) & PTE_INDX_MASK;
	unsigned long* pde = PageTable::PDE_address(faulty_logical_address);
	unsigned long* pte_base_index = (unsigned long*)((pde_indx << 12) | PT_ADDR_MASK);  // address of the first entry in the pd.am

	if((*pde & VALID_BIT) == 0) //*curr_pd_address = pde
	{
		// pde invalid
		*pde = ((curr_vm_pool->_frame_pool->get_frames(1)) << 12 ) | WRITE_BIT | VALID_BIT;
		// get_frames returns a 20 bit value, which is the index of the start frame. Hence, << 12 to make it 32 bit.
		
		
		// setting up new page table, and all its entries
		for(unsigned int pd_offset=0;pd_offset<PAGE_SIZE/4;pd_offset++)
		{
			*(pte_base_index+pd_offset) = WRITE_BIT; //pd_offset*4 not needed as int is 4 bytes long.
		}
	}
	
	// setting up new frame for the given faulty_logical_address
	unsigned long new_frame_address = (curr_vm_pool->_frame_pool->get_frames(1)) << 12 ;
	*(pte_base_index+pte_indx) = new_frame_address | WRITE_BIT | VALID_BIT;
	
	Console::puts("handled page fault\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{
	if(current_page_table -> n_registered_vmpools == VM_POOLS_MAX)
	{
		Console::puts("Virtual Memory full\n");
		assert(false);
		return;
	}
	
	current_page_table -> registered_vmpools[current_page_table -> n_registered_vmpools] = _vm_pool;
	current_page_table -> n_registered_vmpools++;
	
	Console::puts("Registered VM pool\n");
}

void PageTable::free_page(unsigned long _page_no) 
{
	unsigned long pde_indx = _page_no >> (12+10) ;
	unsigned long pte_indx = (_page_no >> 12) & PTE_INDX_MASK;
	unsigned long* pte = PageTable::PTE_address(_page_no);
	
	if(*pte & VALID_BIT)
	{
		ContFramePool::release_frames(*pte>>12);
		*pte = *pte & MAKE_INVALID;
		//Flushing the TLB
		write_cr3((unsigned long)(current_page_table-> page_directory));
	}
	Console::puts("freed page\n");
}
