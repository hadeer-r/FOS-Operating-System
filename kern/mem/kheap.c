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
	panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
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
	return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code
	panic("sbrk() is not implemented yet...!!");
}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc (unsigned int size ){
if(isKHeapPlacementStrategyFIRSTFIT()==0){
cprintf("not following the first fit");
return NULL;}
else{
     if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
        void* blockallocated=alloc_block_FF(size);
        if(blockallocated==NULL){
            cprintf("failed to allocate");
            return NULL;
        }
        return blockallocated;
     }
else{
    bool x=0;
    unsigned int upsize=ROUNDUP(size,PAGE_SIZE);
    unsigned int numofpages=upsize/PAGE_SIZE;
    unsigned int VA=KERNEL_HEAP_START;
    while(VA<KERNEL_HEAP_MAX){
        x=1;
        for(unsigned int i=0;i<numofpages;i++){
            unsigned int VA2 = VA+(i*PAGE_SIZE);
            struct FrameInfo* infoframe=get_frame_info(ptr_page_directory,VA2,NULL);
            if(infoframe !=NULL){
                x=0;
                break;
            }
        }
        if(x==1){
            for(unsigned int i=0;i<numofpages;i++){
                unsigned int mapVA=VA+(i*PAGE_SIZE);
                struct FrameInfo* infoframe;
                if(allocate_frame(&infoframe) !=0){
                    cprintf("failed allocation");
                    return NULL;
                }
                uint32*page_table;
                if(get_page_table(ptr_page_directory,mapVA,&page_table)==0){
                    if(create_page_table(ptr_page_directory, mapVA) == NULL){
                        cprintf("failed to create page table");
                        free_frame(infoframe);
                        return NULL;
                    }
                }
                 if (map_frame(ptr_page_directory, infoframe, mapVA, PERM_WRITEABLE) != 0) {
                            cprintf("map frame failed\n");
                            free_frame(infoframe);
                            return NULL;}

                            tlb_invalidate(ptr_page_directory,(void*)mapVA);
            }
            return(void*)VA;
        }
        VA+=PAGE_SIZE;
    }
    return NULL;
}
}
return NULL;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
         uint32 va = (uint32)virtual_address;

	 if (va >= start && va < seg_break) {
	     return free_block((void*)va);
	 }


	 else if (va >= (limit + PAGE_SIZE) && va < KERNEL_HEAP_MAX) {
	     uint32* page;

	     struct FrameInfo* frame_info = get_frame_info(ptr_page_directory, va, &page);


	     uint32 frames = (frame_info->references + PAGE_SIZE - 1) / PAGE_SIZE; 

	     for (uint32 i = 0; i < frames ; i++) {
	        uint32 frame_address = va + (i * PAGE_SIZE);

		 unmap_frame(ptr_page_directory, frame_address);

		 }
	 }

	 else {
	     panic("Invalid virtual address passed to kfree");
     }
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
	//panic("kheap_virtual_address() is not implemented yet...!!");
    struct FrameInfo* ff = to_frame_info(physical_address);

    if (ff == NULL || ff->references == 0)
    {
        return 0;
    }

    uint32 virtual_address = (ff->va & 0xFFFFF000) + (physical_address & 0x00000FFF);

    return virtual_address;
	
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
