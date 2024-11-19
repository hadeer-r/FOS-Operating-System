#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code

	//initialize_paging()??!
//	cprintf("initcurdastart:%x\n",daStart ,"\n");
	start=daStart;
//	cprintf("initcurstart:%x\n",start ,"\n");
	initSizeToAllocate=ROUNDUP(initSizeToAllocate,PAGE_SIZE);
	uint32 needed_frames=initSizeToAllocate/PAGE_SIZE;
	seg_break= start+initSizeToAllocate;
	limit= daLimit;

	 if(seg_break>daLimit)
			return   E_NO_MEM;

	 if(needed_frames>LIST_SIZE(&MemFrameLists.free_frame_list))
	 		return   E_NO_MEM;



	 for(uint32 i=0;i<needed_frames;i++){
	 		struct FrameInfo* ptr_frame;
	 		allocate_frame(&ptr_frame);
	 		map_frame(ptr_page_directory,ptr_frame, (start+i*PAGE_SIZE),PERM_WRITEABLE|PERM_PRESENT|PERM_MODIFIED);

	 	}
	 initialize_dynamic_allocator(start,initSizeToAllocate);

        return 0;
}

void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
	//return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code

	if(numOfPages==0) return (void*)seg_break;
	else
	{
		/*uint32 available_size=ROUNDDOWN(limit-seg_break,PAGE_SIZE);
		int available_pages=available_size/PAGE_SIZE; // way 1*/
		/*if(available_pages<numOfPages)
					return (void*)-1 ;*/
//		cprintf("curbreak:%x\n",seg_break ,"\n");
		uint32 needed_break=seg_break+(( uint32)numOfPages)*PAGE_SIZE;
		if(needed_break>limit) return (void*)-1 ;

		if(numOfPages>LIST_SIZE(&MemFrameLists.free_frame_list))
			 		return  (void*)-1 ;
		 for(uint32 i=0;i<numOfPages;i++){
			 		struct FrameInfo* ptr_frame;
			 		allocate_frame(&ptr_frame);
			 		map_frame(ptr_page_directory,ptr_frame, (seg_break+i*PAGE_SIZE),PERM_WRITEABLE|PERM_PRESENT|PERM_MODIFIED);

			 	}
		 uint32 prev_break=seg_break;
		 seg_break=seg_break+(( uint32)numOfPages)*PAGE_SIZE; //needed_break;
//		 cprintf("modbreak :%x\n",seg_break ,"\n");
		 return (void*)prev_break;

	}

}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
		return alloc_block_FF( size);

	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
