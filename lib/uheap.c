#include <inc/lib.h>
typedef struct PageMetadata {
    bool is_marked;
    uint32 size;
} PageMetadata;

#define TOTAL_PAGES ((USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE)
PageMetadata user_heap_metadata[TOTAL_PAGES];


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size) {
    // If the requested size is less than or equal to the block size, use the dynamic block allocator
    if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
        return alloc_block_FF(size);
    }

    // Align the requested size to the next page boundary
    uint32 aligned_size = ROUNDUP(size, PAGE_SIZE);
    uint32 num_pages = aligned_size / PAGE_SIZE;

    uint32 consecutive_free = 0;
    uint32 start_page = 0;

    // Start from the current limit of the user heap (myEnv->u_limit)
    //uint32 begin = myEnv->u_limit;
cprintf("===============1==========");
    // First-Fit Allocation Strategy
    if (sys_isUHeapPlacementStrategyFIRSTFIT()) {
    	//cprintf("===============2==========");
        for (uint32 i = 0; i < (USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE; i++) {
       // cprintf("===============3==========");
            if (!user_heap_metadata[i].is_marked) { // Check if the page is free
                if (consecutive_free == 0) {
                    start_page = i; // Mark the start of a free block
                }//cprintf("===============4==========");
                consecutive_free++;

                // If enough consecutive free pages are found
                if (consecutive_free == num_pages) {
                    uint32 start_va = myEnv->u_limit+PAGE_SIZE; // Correct calculation of the start address

                    // Reserve memory for the requested space
                  //  cprintf("==============5===========%x\n", start_va);
             sys_allocate_user_mem(start_va, aligned_size);
                    //cprintf("==============6===========%x\n", start_va);
                    // Update metadata for allocated pages
             for (uint32 j = start_page; j < start_page + num_pages*PAGE_SIZE; j++) {
                 user_heap_metadata[j].is_marked = 1;
                 user_heap_metadata[j].size = aligned_size;

                 // Print the updated metadata for debugging
                 //cprintf("Page %d: is_marked = %d, size = %u bytes\n",j, user_heap_metadata[j].is_marked, user_heap_metadata[j].size);
             }


                    // Update the environment's heap limit to reflect the new end of the heap
                    myEnv->u_limit = start_va + aligned_size;
                    cprintf("==============10===========%x\n", start_va);
                    return (void*)start_va; // Return the allocated virtual address
                    cprintf("==============11===========%x\n", start_va);
                }
            } else {
                consecutive_free = 0; // Reset count if a page is occupied
            }
        }
    }

    // No suitable block found
    return NULL;
}



//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	panic("free() is not implemented yet...!!");
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
//	panic("smalloc() is not implemented yet...!!");

	uint32 upsize = ROUNDUP(size, PAGE_SIZE);
	uint32 numpages = upsize / PAGE_SIZE;
	int count = 0;

	uint32 start =USER_HEAP_START;
	uint32 end = USER_HEAP_MAX;

	uint32 s_space=0;
	while(start<end){
		if(myEnv->env_page_directory==NULL) {
			count++;
			if(count==1){
				s_space=start;
			}
		}
		else {
			count=0;
			continue;
		}

		if(count==numpages){

			//(char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
			return (void*) sys_createSharedObject(sharedVarName,size,isWritable,(void*)s_space);
		}
		start+=PAGE_SIZE;

	}

	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
//	panic("sget() is not implemented yet...!!");
	int size_shared_object = sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
		if(size_shared_object <= 0)
			return NULL;
	void* allocated_VA = smalloc(sharedVarName,size_shared_object,1);
//	if(allocated_VA != NULL){
//		void* shared_object = sys_getSharedObject(ownerEnvID,sharedVarName,allocated_VA);
//		if(shared_object != NULL){
//			return shared_object;
//		}
//		else
//			return NULL;
//	}else
   return NULL;

}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
