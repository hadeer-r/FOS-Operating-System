#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"
uint32 Allocation_count = 0;

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart,
		uint32 initSizeToAllocate, uint32 daLimit) {
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code

	//initialize_paging()??!

	start = daStart;

	initSizeToAllocate = ROUNDUP(initSizeToAllocate, PAGE_SIZE);
	uint32 needed_frames = initSizeToAllocate / PAGE_SIZE;
	seg_break = start + initSizeToAllocate;
	limit = daLimit;

	if (seg_break > daLimit)
		return E_NO_MEM;

	if (needed_frames > LIST_SIZE(&MemFrameLists.free_frame_list))
		return E_NO_MEM;
	for (uint32 i = 0; i < needed_frames; i++) {
		struct FrameInfo* ptr_frame;
		allocate_frame(&ptr_frame);
		map_frame(ptr_page_directory, ptr_frame, (start + i * PAGE_SIZE),
				PERM_WRITEABLE | PERM_PRESENT | PERM_MODIFIED);
		frame_array[to_frame_number(ptr_frame)].virtual_adress = (void*) (start
				+ i * PAGE_SIZE);
		frame_array[to_frame_number(ptr_frame)].num_of_frames = 0;
	}
	initialize_dynamic_allocator(start, initSizeToAllocate);

	return 0;
}

void* sbrk(int numOfPages) {
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
	if (numOfPages == 0)
		return (void*) seg_break;
	else {
		/*uint32 available_size=ROUNDDOWN(limit-seg_break,PAGE_SIZE);
		 int available_pages=available_size/PAGE_SIZE; // way 1*/
		/*if(available_pages<numOfPages)
		 return (void*)-1 ;*/

		uint32 needed_break = seg_break + ((uint32) numOfPages) * PAGE_SIZE;
		if (needed_break > limit) {
//			cprintf(" -1 test1 sbrk\n");
			return (void*) -1;


		}

		if (numOfPages > LIST_SIZE(&MemFrameLists.free_frame_list))
		{
//			cprintf(" -1 test2 sbrk\n");
			return (void*) -1;
		}

		for (uint32 i = 0; i < numOfPages; i++) {
			struct FrameInfo* ptr_frame;
			allocate_frame(&ptr_frame);
			map_frame(ptr_page_directory, ptr_frame,
					(seg_break + i * PAGE_SIZE),
					PERM_WRITEABLE | PERM_PRESENT | PERM_MODIFIED);
			frame_array[to_frame_number(ptr_frame)].virtual_adress =
					(void*) (seg_break + i * PAGE_SIZE);
			frame_array[to_frame_number(ptr_frame)].num_of_frames = 0;
		}

		uint32 prev_break = seg_break;
		seg_break = seg_break + ((uint32) numOfPages) * PAGE_SIZE; //needed_break;
		//cprintf("pre_break in sbrk kernalll :%u\n", prev_break, "\n");
		return (void*) prev_break;

	}

}
//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

#define totalpages ((KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE)
uint32 allocatesize[totalpages];
void* kmalloc(unsigned int size) {
	unsigned int upsize = ROUNDUP(size, PAGE_SIZE); //3shan page size
	unsigned int numpages = upsize / PAGE_SIZE;
	//cprintf("================== test block allocator range=================");
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
		return alloc_block_FF(size);
	}
//cprintf ("======================test page allocator ===========")
	uint32 staradd = limit + PAGE_SIZE;
	uint32 endadd = KERNEL_HEAP_MAX;
	uint32 curr = staradd;
	uint32 FS = 0; //awel el  free space
	unsigned int consecutivePages = 0;
	if (numpages > LIST_SIZE(&MemFrameLists.free_frame_list)) {
		return NULL;
	}
	while (curr < endadd) {
		uint32* pagetable;
		struct FrameInfo* FI = get_frame_info(ptr_page_directory, curr,
				&pagetable); //frameinfo
//            cprintf("=========page directory now : %p, page directory Index:%d\n",ptr_page_directory, PDX(ptr_page_directory));
		if (FI == NULL) {
//cprintf("ttttttttttttttttt freeframetttttttttttttttttt");
			if (consecutivePages == 0) {
				FS = curr; // Mark the start of the free space
			}
			consecutivePages++;
		} else {

			consecutivePages = 0;
		}
// cprintf("tttttttttttttttttt map frame ttttttttttttt");
		if (consecutivePages == numpages) {
			uint32 address = FS;
			frame_array[Allocation_count].num_of_frames = numpages;
			frame_array[Allocation_count].virtual_adress = (void*) FS;

			for (unsigned int i = 0; i < numpages; i++) {
				struct FrameInfo* frame;
				int result = allocate_frame(&frame);

				map_frame(ptr_page_directory, frame, address,
						PERM_WRITEABLE | PERM_PRESENT);
				if (i == 0) {
					frame_array[to_frame_number(frame)].num_of_frames =
							numpages;
				} else {
					frame_array[to_frame_number(frame)].num_of_frames = 0;
				}
				frame_array[to_frame_number(frame)].virtual_adress =
						(void*) address;
				address += PAGE_SIZE;
			}
			for (unsigned int i = 0; i < numpages; i++) {
				allocatesize[(FS + i * PAGE_SIZE - KERNEL_HEAP_START)
						/ PAGE_SIZE] = 1;
			}

			frame_array[Allocation_count].virtual_adress = (void*) FS;
			Allocation_count++;

			return (void*) FS;
		}

		curr += PAGE_SIZE;
	}

	return NULL;
}

void kfree(void* virtual_address) {
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	uint32 va = (uint32) virtual_address;

	if (va >= start && va < seg_break) {

		return free_block(virtual_address);

	}

	else if (va >= (limit + PAGE_SIZE) && va < KERNEL_HEAP_MAX) {

		uint32 n_pages;

		for (int i = 0; i < MaX_F; i++) {
//			cprintf("ttttttttttttttttttt---------found virtual address\n");

			if (frame_array[i].virtual_adress == virtual_address) {
//				cprintf("ttttttttttttttttttt---------found virtual address\n");

				n_pages = frame_array[i].num_of_frames;
				frame_array[i].num_of_frames = 0;
				frame_array[i].virtual_adress = NULL;
				break;

			}
		}

		uint32 current_va = va;
		for (uint32 i = 0; i < n_pages; i++) {
			uint32* ptr_page_table = NULL;

			struct FrameInfo* frame_info = get_frame_info(ptr_page_directory,
					current_va + (i * PAGE_SIZE), &ptr_page_table);
			if (frame_info != NULL) {
				unmap_frame(ptr_page_directory, current_va + (i * PAGE_SIZE));

			}

		}
	}

	else {
		panic("Invalid virtual address");
	}

}

uint32 kheap_physical_address(unsigned int virtual_address) {
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	uint32 *ptr_page_table;

	get_page_table(ptr_page_directory, virtual_address, &ptr_page_table);

	if (!(ptr_page_table[PTX(virtual_address)] & PERM_PRESENT)) {
		return 0;
	}

	uint32 physical_address = ptr_page_table[PTX(virtual_address)] & 0xFFFFF000;
	return physical_address + (virtual_address & 0x00000FFF);

}

uint32 kheap_virtual_address(uint32 physical_address) {
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	struct FrameInfo* ff = to_frame_info(physical_address);

	if (ff == NULL || ff->references == 0) {
		return 0;
	} else {
		if (frame_array[to_frame_number(ff)].virtual_adress != NULL) {
			return (((uint32) frame_array[to_frame_number(ff)].virtual_adress
					& 0xFFFFF000) + (physical_address & 0x00000FFF));
		} else {
			return 0;
		}
	}

}//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size) {
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
