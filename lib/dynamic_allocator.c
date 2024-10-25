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
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
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

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
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
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return ;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...
	 LIST_INIT(&freeBlocksList);
	    struct BlockElement *firstBlock;
	    firstBlock=(struct BlockElement *)daStart;
	    firstBlock->is_free = 1;
	    firstBlock->prev_next_info.le_next = NULL;
	    firstBlock->prev_next_info.le_prev = NULL;
	    firstBlock->size = initSizeOfAllocatedSpace;
	    LIST_INSERT_TAIL(&freeBlocksList, firstBlock);

}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
	//Your Code is Here...
	// Header: stores the block size and allocation status
	    struct BlockElement* header = (struct BlockElement*) va;
	    header->size = totalSize;
	    if (isAllocated)
	    {
	        header->is_free = 0;
	    }
	    else
	    {
	        header->is_free = 1;
	    }
	    void* footer_va = (uint8*)va + totalSize - sizeof(struct BlockElement);

	    struct BlockElement* footer = (struct BlockElement*) footer_va;
	    footer->size = totalSize;
	    if (header->is_free == 0)
	    {
	        footer->is_free = 0;
	    }
	    else
	    {
	        footer->is_free = 1;
	    }
}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet");

	struct BlockElement *blk;
        bool found=0;
		LIST_FOREACH (blk,&(freeBlocksList)){
			if(!is_free_block(blk)) continue;

			if((get_block_size(blk)>size)){

			if(get_block_size(blk)>=size+4*sizeof(int))
			{
				set_block_data(blk,size,1);
				int rem=get_block_size(blk)-size;
				struct BlockElement* new_address=blk+size;
				set_block_data(new_address,rem,0);

			}
			else  set_block_data(blk,get_block_size(blk),1);

				found=1;
				break;


			}


		}

		if(!found) sbrk(0);


	//Your Code is Here...
		return NULL;


}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...


	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
	if (size % 2 != 0) size++;//ensure that the size is even (to use LSB as allocation flag)
	if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
	size = DYN_ALLOC_MIN_BLOCK_SIZE ;
	if (!is_initialized)
	{
	uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
	uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
	uint32 da_break = (uint32)sbrk(0);
	initialize_dynamic_allocator(da_start, da_break - da_start);
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet");

	struct BlockElement *blk,*address;
	        int best_size=-1;
	                bool found=0;
	LIST_FOREACH (blk,&(freeBlocksList)){
	if(!is_free_block(blk)) continue;
	found=1;
	int sz=get_block_size(blk);
	if(sz>=size){
	      if(sz<best_size||best_size==-1)
	         {
	               address=blk;
	                best_size=sz;
	          }

	}


	}

	if(!found) sbrk(0);
	else
	{
	        if(best_size>=size+4*sizeof(int))
	        {
	           set_block_data(address,size,1);
	           int rem=best_size-size;
	            struct BlockElement* new_address=address+size;
	           set_block_data(new_address,rem,0);

	          }
	        else  set_block_data(address,best_size,1);
	}

	//Your Code is Here...

	}


}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("free_block is not implemented yet");
	//Your Code is Here...
}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...

	if(va == NULL){
		if(new_size > 0)
		{
			return alloc_block_BF(new_size);
		}
		else return NULL;
	}
	if(new_size==0){
		free_block(va);
		return NULL;
	}

	uint32 old_size = get_block_size(va);
	if(old_size==new_size)
		return va;

	void*new_va=NULL;

	uint32 diff_size = new_size-old_size;


	if(new_size>old_size){

		uint32* next_block = va+old_size +1;

		// struct BlockElement *next_blk;
		// LIST_FOREACH(next_blk,&(freeBlocksList)){
		// 	if(next_blk==next_block+1){
		// 		break;
		// 	}
		// }
		struct BlockElement *current_blk;
		LIST_FOREACH(current_blk,&(freeBlocksList)){
			if(current_blk==(struct BlockElement*)(next_block+1)){
				break;
			}
		}

		struct BlockElement *next_blk = LIST_NEXT(current_blk);

		

		if(next_blk->is_free){
			if(next_blk->size==diff_size ){
				// 1. remove next_blk from list
				LIST_REMOVE(&(freeBlocksList), next_blk);

				set_block_data(va,new_size,1);
				return va;
				

			}
			else if(next_blk->size>diff_size){

				if(next_blk->size-diff_size<16){
					set_block_data(va,current_blk->size+next_blk->size,1);
				}
				else {
					set_block_data(va+new_size+1,next_blk->size-diff_size,0);
					set_block_data(va,new_size,1);
				}
				return va;
			}
			else {
				uint32* new_add= alloc_block_FF(new_size);
				free_block(va);
				return new_add;

			}
			

		}

		else{
			uint32* new_add= alloc_block_FF(new_size);
				free_block(va);
				return new_add;
		}


	}

	// last condition is that new_size < old_size

	if(old_size-new_size<16)
	return va;
	
	set_block_data(va,new_size,1);
	set_block_data(va+new_size+1,old_size-new_size,0);
	
	return va;
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
