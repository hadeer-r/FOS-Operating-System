#include <inc/lib.h>
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

		uint32 y = (va - start_page) / PAGE_SIZE, numofpages = 0;
		if (is_start[y]) {
			is_start[y] = 0;
			while (marked[y] && !is_start[y]) {
				numofpages++;
				marked[y] = 0;
				y++;
			}

			sys_free_user_mem(va, numofpages * PAGE_SIZE);

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
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
//	panic("smalloc() is not implemented yet...!!");
	//cprintf("in smallocccccccccccccccccccccccccc");
	if (size == 0)
		return NULL;

	/*if(size<PAGE_SIZE){
	 size=PAGE_SIZE;
	 }*/

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
		//(char* shareName, uint32 size, uint8 isWritable,void* virtual_address)

		int x = sys_createSharedObject(sharedVarName, size, isWritable,
				(void*) va);

		if (x == E_NO_SHARE || x == E_SHARED_MEM_EXISTS) {
			return NULL;
		}
		return (void *) va;
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

	int size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);
	if (size <= 0 || size == E_SHARED_MEM_NOT_EXISTS)
		return NULL;

	size = ROUNDUP(size, PAGE_SIZE);
	uint32 needed_pages = size / PAGE_SIZE;

	uint32 seq = 0, count = 0, va;
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
		marked[y] = 1;

		for (uint32 i = y; i < y + needed_pages; i++) {
			marked[i] = 1;
		}

		int x = sys_getSharedObject(ownerEnvID, sharedVarName, (void*) va);

		if (x == E_SHARED_MEM_NOT_EXISTS) {
			return NULL;
		} else {
			return (void *) va;
		}

	}

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
