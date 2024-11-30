/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va) {
	uint32 *curBlkMetaData = ((uint32 *) va - 1);
	return (*curBlkMetaData) & ~(0x1);
}
//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va) {
	uint32 *curBlkMetaData = ((uint32 *) va - 1);
	return (~(*curBlkMetaData) & 0x1);
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY) {
	void *va = NULL;
	switch (ALLOC_STRATEGY) {
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list) {
	cprintf("=========================================\n");
	struct BlockElement* blk;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk),
				is_free_block(blk));
		cprintf("blk add :%x\n", blk, "\n");
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart,
		uint32 initSizeOfAllocatedSpace) {
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0)
			initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...
	// Set up BEG Block
	uint32* BEG_Block = (uint32*) daStart;
	*BEG_Block = 0x1;
	uint32* END_Block = (uint32*) (daStart + initSizeOfAllocatedSpace
			- sizeof(uint32));
	*END_Block = 0x1;

	uint32 Blockaddress = daStart + 2 * sizeof(int);
	struct BlockElement* firstBlock = (struct BlockElement*) Blockaddress;

	uint32* header = (uint32*) ((uint32) firstBlock - 4);
	uint32* footer = (uint32*) ((uint32) firstBlock + initSizeOfAllocatedSpace
			- 4 * sizeof(int));
	*header = (initSizeOfAllocatedSpace - 8) | 0;
	*footer = (initSizeOfAllocatedSpace - 8) | 0;

	LIST_INIT(&freeBlocksList);
	LIST_INSERT_TAIL(&freeBlocksList, firstBlock);
}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated) {
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("set_block_data is not implemented yet");
	//Your Code is Here...
	// Header: stores the block size and allocation status
	uint32 *header = (uint32 *) ((char *) va - sizeof(uint32));

	*header = totalSize | isAllocated;

	uint32 *footer = (uint32 *) (((char *) va + totalSize) - 2 * sizeof(uint32));

	*footer = totalSize | isAllocated;

}

//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
int cow = 0;
void *alloc_block_FF(uint32 size) {
	if (size == 0)
		return NULL;

//	print_blocks_list((struct MemBlock_LIST) (freeBlocksList));
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0)
			size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if (!is_initialized) {
			uint32 required_size = size + 2 * sizeof(int) /*header & footer*/
			+ 2 * sizeof(int) /*da begin & end*/;
			uint32 da_start = (uint32) sbrk(
			ROUNDUP(required_size, PAGE_SIZE) / PAGE_SIZE);
			uint32 da_break = (uint32) sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet")
	// write your code
	struct BlockElement *blk;
	bool case1 = 0, case2 = 0;
	size += 2 * sizeof(uint32); // head w footer
//	cprintf("sz of blk :%d\n", size, "\n");
//	print_blocks_list(freeBlocksList);
	LIST_FOREACH (blk,&(freeBlocksList))
	{
		if ((get_block_size((void*) blk) >= size)) {

			//
			if (get_block_size((void*) blk) >= size + (uint32) 16) // what is minimum block size?
					{
				//
				uint32 rem = get_block_size((void*) blk) - size;
				set_block_data(blk, size, 1);
				struct BlockElement* new_address =
						(struct BlockElement*) ((char *) blk + size);
				set_block_data(new_address, rem, 0);
				LIST_INSERT_AFTER(&freeBlocksList, blk, new_address);

				//
				case1 = 1;

			} else {
				set_block_data(blk, get_block_size((void*) blk), 1);
				case2 = 1;
			}
			LIST_REMOVE(&freeBlocksList, blk);
			break;
		}

	}

	if (!case1 && !case2) {

//		cprintf("ammmmmmmmmm in with sbrk");
		uint32 new_size = size;
		new_size = ROUNDUP(new_size, PAGE_SIZE);
		int needed_pages = new_size / PAGE_SIZE;

		uint32 address = (uint32) sbrk(needed_pages);
		//cprintf("add in ff :%u\n", address, "\n");
		if ((void*) address == (void*) -1)
			return NULL;

		uint32* END_Block = (uint32*) ((uint32) address + new_size
				- sizeof(uint32));
		*END_Block = 0x1;

		struct BlockElement*p = NULL;
		if (LIST_SIZE(&(freeBlocksList)))
			p = LIST_LAST(&freeBlocksList);
		struct BlockElement * newBlock_add = (void*) address;
		struct BlockElement*prev = (struct BlockElement*) ((char *) newBlock_add
				- sizeof(uint32));

		LIST_INSERT_TAIL(&(freeBlocksList), newBlock_add); //////important
		set_block_data(newBlock_add, new_size, 0);

		if (p && is_free_block(prev)) {

			new_size = get_block_size(prev) + new_size;

			LIST_REMOVE(&freeBlocksList, (struct BlockElement* )newBlock_add);

			set_block_data(p, new_size, 0);

		}

		// print_blocks_list((struct MemBlock_LIST)(freeBlocksList));
		blk = alloc_block_FF(size - 2 * sizeof(uint32));

	}
	uint32 x = (uint32) blk;

	return (void *) blk;
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size) {
	if (size == 0)
		return NULL;
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...

	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0)
			size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if (!is_initialized) {
			uint32 required_size = size + 2 * sizeof(int) /*header & footer*/
			+ 2 * sizeof(int) /*da begin & end*/;
			uint32 da_start = (uint32) sbrk(
			ROUNDUP(required_size, PAGE_SIZE) / PAGE_SIZE);
			uint32 da_break = (uint32) sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================
	size += 2 * sizeof(uint32);
	struct BlockElement *blk, *address;
	uint32 best_size = -1;
	bool found = 0;
	LIST_FOREACH (blk,&(freeBlocksList))
	{

		uint32 sz = get_block_size(blk);
		if (sz >= size) {
			if (sz < best_size || best_size == -1) {
				address = blk;
				best_size = sz;
			}
			found = 1;

		}

	}

	if (!found) {
		if ((uint32) sbrk(size) == -1)
			return NULL;
		return sbrk(size);

	}

	if (best_size >= size + (uint32) 16) {
		uint32 rem = best_size - size;
		struct BlockElement* new_address =
				(struct BlockElement*) ((char *) address + size);
		set_block_data(address, size, 1);
		set_block_data(new_address, rem, 0);
		LIST_INSERT_AFTER(&freeBlocksList, address, new_address);
	} else
		set_block_data(address, best_size, 1);

	LIST_REMOVE(&freeBlocksList, address);
	return address;

}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va) {
	if (va == NULL)
		return;

	if (!LIST_SIZE(&freeBlocksList)) {
		LIST_INSERT_HEAD(&freeBlocksList, (struct BlockElement * ) va);
		uint32 sz = get_block_size(va);
		set_block_data(va, sz, 0);
		return;
	}
	bool found = 0;
	struct BlockElement *current_blk;
	LIST_FOREACH(current_blk,&(freeBlocksList))
	{

		if (current_blk > (struct BlockElement*) (va)) {
			LIST_INSERT_BEFORE(&freeBlocksList, current_blk,
					(struct BlockElement * )va);
			found = 1;
			break;
		}
	}

	if (!found)
		LIST_INSERT_TAIL(&freeBlocksList, (struct BlockElement * ) va);

	struct BlockElement*prev = NULL, *nxt = NULL; // prev foot ,nxt head
	struct BlockElement*p, *n;
	p = LIST_PREV((struct BlockElement * ) va);
	n = LIST_NEXT((struct BlockElement * ) va);
	nxt = (struct BlockElement*) ((char *) va + get_block_size(va));
	prev = (struct BlockElement*) ((char *) va - sizeof(uint32));

	uint32 new_sz = 0;

	if (p && n && is_free_block(prev) && is_free_block(nxt)) {
		new_sz = get_block_size(nxt) + get_block_size(prev)
				+ get_block_size(va);
		LIST_REMOVE(&freeBlocksList, n);
		LIST_REMOVE(&freeBlocksList, (struct BlockElement* )va);
		set_block_data(p, new_sz, 0);
	} else if (p && is_free_block(prev)) {
		new_sz = get_block_size(prev) + get_block_size(va);
		LIST_REMOVE(&freeBlocksList, (struct BlockElement * )va);
		set_block_data(p, new_sz, 0);
	} else if (n && is_free_block(nxt)) {
		new_sz = get_block_size(nxt) + get_block_size(va);
		LIST_REMOVE(&freeBlocksList, n);
		set_block_data(va, new_sz, 0);
	} else {
		new_sz = get_block_size(va);
		set_block_data(va, new_sz, 0);
	}

	return;

}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size) {
//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("realloc_block_FF is not implemented yet");
//Your Code is Here...

	if (va == NULL) {
		if (new_size > 0) {
			return alloc_block_FF(new_size);
		} else
			return NULL;
	}
	if (new_size == 0) {
		free_block(va);
		return NULL;
	}
	if (get_block_size(va) == new_size)
		return va;

	uint32 old_size = get_block_size(va);

	uint32 diff_size = new_size - old_size;

	if (diff_size > 0) {

		uint32* next_block = va + old_size;

		if (is_free_block(next_block)) {

			if (get_block_size(next_block) == diff_size) {
				// 1. remove next_blk from list
				LIST_REMOVE(&(freeBlocksList),
						(struct BlockElement* )next_block);
				set_block_data(va, new_size + 8, 1);
				cprintf("1\n");
				return va;

			}

			else if (get_block_size(next_block) > diff_size) {
				cprintf("2p\n");
				if (get_block_size(next_block) - diff_size < 16) {

					LIST_REMOVE(&(freeBlocksList),
							(struct BlockElement* )next_block);
					set_block_data(va,
							get_block_size(va) + get_block_size(next_block), 1);
					cprintf("2\n");
				} else {

					set_block_data(va, new_size + 8, 1);

					set_block_data(next_block - diff_size + 4,
							get_block_size(next_block) - diff_size, 0);
					cprintf("3\n");
				}
				return va;
			} else {
				uint32* new_add = alloc_block_FF(new_size + 8);
				if (!new_add) {
					free_block(va);
					return NULL;
				}
				free_block(va);
				cprintf("4\n");

				return new_add;

			}

		}

		else {
			uint32* new_add = alloc_block_FF(new_size + 8);
			if (!new_add) {
				free_block(va);
				return NULL;
			}

			free_block(va);
			cprintf("5\n");
			return new_add;
		}

	}

// last condition is that new_size < old_size

	if (old_size - new_size < 16) {
		cprintf("6\n");
		return va;
	}
	diff_size = old_size - new_size;
	set_block_data(va, new_size + 8, 1);
	set_block_data(va + new_size, old_size - new_size + 8, 0);

	struct BlockElement *block;
	bool found = 0;
	LIST_FOREACH(block,&freeBlocksList)
	{
		if (block > (struct BlockElement*) va + new_size) {
			LIST_INSERT_BEFORE(&freeBlocksList, block,
					(struct BlockElement* )va + new_size);
			found = 1;
		}
	}
	if (!found) {
		LIST_INSERT_TAIL(&freeBlocksList, (struct BlockElement* )va);
	}
	cprintf("7\n");
	return va;
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size) {
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size) {
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
