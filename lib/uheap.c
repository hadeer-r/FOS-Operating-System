#include <inc/lib.h>
#define total ((USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE) + 3


#define total ((USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE) + 100
//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment) {
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================

bool marked[total], is_start[total];

void* malloc(uint32 size) {
	if (size == 0)
		return NULL;
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		return alloc_block_FF(size);

	if (sys_isUHeapPlacementStrategyFIRSTFIT()) {
		size = ROUNDUP(size, PAGE_SIZE);
		uint32 needed_pages = size / PAGE_SIZE;

		uint32 count = 0, va;
		uint32 start_page = myEnv->u_limit + PAGE_SIZE;
		for (uint32 i = start_page; i < USER_HEAP_MAX; i += PAGE_SIZE) {
			uint32 x = (i - start_page) / PAGE_SIZE;
			if (marked[x]) {
				count = 0;
			} else {
				if (!count)
					va = i;
				count++;
			}
			if (count >= needed_pages)
				break;
		}

		if (count >= needed_pages) {
			uint32 y = (va - start_page) / PAGE_SIZE;
			is_start[y] = 1;

			for (uint32 i = y; i < y + needed_pages; i++) {
				marked[i] = 1;
			}
			sys_allocate_user_mem(va, size);
			return (void *) va;

		}
	}

	// No suitable block found
	return NULL;
}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address) {
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	uint32 va = (uint32) virtual_address;
	uint32 start_page = myEnv->u_limit + PAGE_SIZE;
	if (va >= USER_HEAP_START && va < myEnv->u_limit)
		free_block(virtual_address);
	else if (va >= start_page && va < USER_HEAP_MAX) {

		uint32 y = (va - start_page) / PAGE_SIZE,numofpages=0;
		if (is_start[y]) {
			is_start[y]=0;
			while(marked[y]&&!is_start[y])
			{
				numofpages++;
				marked[y]=0;
				y++;
			}

			sys_free_user_mem(va,numofpages*PAGE_SIZE);

		}


	} else
		panic("Invalid virtual address");

	return;

}

//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable) {
//==============================================================
//DON'T CHANGE THIS CODE========================================
	if (size == 0)
		return NULL;
//==============================================================
//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
// Write your code here, remove the panic and write your code
//	panic("smalloc() is not implemented yet...!!");

	uint32 upsize = ROUNDUP(size, PAGE_SIZE);
	uint32 numpages = upsize / PAGE_SIZE;
	int count = 0;

	uint32 start = USER_HEAP_START;
	uint32 end = USER_HEAP_MAX;

	uint32 s_space = 0;
	while (start < end) {
		if (myEnv->env_page_directory == NULL) {
			count++;
			if (count == 1) {
				s_space = start;
			}
		} else {
			count = 0;
			continue;
		}

		if (count == numpages) {

			//(char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
			return (void*) sys_createSharedObject(sharedVarName, size,
					isWritable, (void*) s_space);
		}
		start += PAGE_SIZE;

	}

	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName) {
//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
// Write your code here, remove the panic and write your code
//	panic("sget() is not implemented yet...!!");
	int size_shared_object = sys_getSizeOfSharedObject(ownerEnvID,
			sharedVarName);
	if (size_shared_object <= 0)
		return NULL;
	void* allocated_VA = smalloc(sharedVarName, size_shared_object, 1);
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

void sfree(void* virtual_address) {
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
void *realloc(void *virtual_address, uint32 new_size) {
//[PROJECT]
// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}

//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize) {
	panic("Not Implemented");

}
void shrink(uint32 newSize) {
	panic("Not Implemented");

}
void freeHeap(void* virtual_address) {
	panic("Not Implemented");

}
