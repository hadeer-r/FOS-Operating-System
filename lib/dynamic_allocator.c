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
		 // Set up BEG Block
			uint32* BEG_Block = (uint32*) daStart;
		    *BEG_Block = 0x1;
		    uint32* END_Block = (uint32*) (daStart + initSizeOfAllocatedSpace - sizeof(uint32));
		    *END_Block = 0x1;

		    cprintf("BEG Block = %x\n", *BEG_Block);
		    cprintf("END Block = %x\n", *END_Block);

		    uint32 Blockaddress = daStart + 2*sizeof(int);
			struct BlockElement* firstBlock = (struct BlockElement*)Blockaddress;

			uint32* header = (uint32*) ((uint32)firstBlock - 4);
			uint32* footer = (uint32*) ((uint32)firstBlock + initSizeOfAllocatedSpace - 4*sizeof(int));
			*header = (initSizeOfAllocatedSpace-8) | 0;
			*footer = (initSizeOfAllocatedSpace-8) | 0;
			cprintf("header = %x \n",*header);
			cprintf("footer = %x \n",*footer);

			LIST_INIT(&freeBlocksList);
			LIST_INSERT_TAIL(&freeBlocksList,firstBlock);
}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("set_block_data is not implemented yet");
	//Your Code is Here...
	// Header: stores the block size and allocation status
	   uint32 *header = (uint32 *)((char *)va - sizeof(uint32));
		    *header = totalSize | isAllocated;

		    uint32 *footer = (uint32 *)(((char *)va + totalSize) - 2 * sizeof(uint32));
		    *footer = totalSize | isAllocated;

}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	if(size==0) return NULL;
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
	//panic("alloc_block_FF is not implemented yet")
	// write your code
	struct BlockElement *blk;
	        bool case1=0,case2=0;
	        size+=2*sizeof(uint32); // head w footer
			LIST_FOREACH (blk,&(freeBlocksList)){
				if((get_block_size((void*)blk)>=size)){

					//
				if(get_block_size((void*)blk) >size+(uint32)8 ) // what is minimum block size?
				{
					//
					uint32 rem=get_block_size((void*)blk)-size;
					set_block_data(blk,size,1);
					struct BlockElement* new_address= (struct BlockElement*)((char *)blk+size);
					set_block_data(new_address,rem,0);
					LIST_INSERT_AFTER(&freeBlocksList, blk, new_address);

					//
					case1=1;

				}
				else
				{
					set_block_data(blk,get_block_size((void*)blk),1);
					case2=1;
				}

				      break;
				}


			}

			if(!case1&&!case2)
			{
				if((uint32)sbrk(size)==-1) return NULL;
				return sbrk(size);
				//print_blocks_list(freeBlocksList);

				}
			else
			{
				LIST_REMOVE(&freeBlocksList,blk);

				return blk;
			}




return NULL;
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	if(size==0) return NULL;
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");
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
	}
	//==================================================================================
	//==================================================================================
	  size+=2*sizeof(uint32);
	struct BlockElement *blk,*address;
	uint32 best_size=-1;
	bool found=0;
	LIST_FOREACH (blk,&(freeBlocksList)){

    uint32 sz=get_block_size(blk);
	if(sz>=size){
	      if(sz<best_size||best_size==-1)
	         {
	               address=blk;
	                best_size=sz;
	         }
	      found=1;

	}

	}

	if(!found)
	{
		if((uint32)sbrk(size)==-1) return NULL;
		return sbrk(size);

		}

    if(best_size>=size+(uint32)16)
    {
       uint32 rem=best_size-size;
       struct BlockElement* new_address=(struct BlockElement*)((char *)address+size);
       set_block_data(address,size,1);
       set_block_data(new_address,rem,0);
       LIST_INSERT_AFTER(&freeBlocksList,address, new_address);
      }
    else  set_block_data(address,best_size,1);

    LIST_REMOVE(&freeBlocksList,address);
    return address;


}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	if(va==NULL) return ;
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_block is not implemented yet");
	//Your Code is Here...

	// is the address may be not here , not belong to the heap?!
//	set_block_data( va, get_block_size(va), 0);
	// to put it sorted , you have three cases

	//struct BlockElement* new_address= (struct BlockElement*)((char *)blk+size);
//		cprintf("1\n");
		uint32 sz=LIST_SIZE(&freeBlocksList);
		struct BlockElement * it= LIST_FIRST(&freeBlocksList);
		bool found=0;
		for(int i=0;i<sz;i++)
		{
			if(it>(struct BlockElement *)va)
			{
				LIST_INSERT_BEFORE(&freeBlocksList, it,(struct BlockElement *)va);
				found=1;
				break;
			}

			it++;
		}
		if(!found) LIST_INSERT_TAIL(&freeBlocksList,(struct BlockElement *) va);
//		cprintf("2\n");
		struct BlockElement*prev=NULL,*nxt=NULL; // prev foot ,nxt head
		struct BlockElement*p,*n;
		p=LIST_PREV((struct BlockElement *) va);
        n=LIST_NEXT((struct BlockElement *) va);
		nxt=(struct BlockElement*)((char *)va+get_block_size(va));

	uint32 new_sz=0;
//	cprintf("a1\n");
//	cprintf("%p\n",p);
     if(n>=(struct BlockElement*)KERNEL_HEAP_START&&p>=(struct BlockElement*)KERNEL_HEAP_START&&(struct BlockElement*)va==(struct BlockElement*)((char *)p+get_block_size(p)) &&nxt==n)
     {
//    	 cprintf("3\n");
    	 new_sz= get_block_size(nxt)+get_block_size(p)+get_block_size(va);
    	 LIST_REMOVE(&freeBlocksList,(struct BlockElement *)va);
    	 LIST_REMOVE(&freeBlocksList,nxt);
    	 set_block_data(p,new_sz,0);
    	 return;
     }
//     cprintf("a2\n");
     if(p>=(struct BlockElement*)KERNEL_HEAP_START&&(struct BlockElement*)va==(struct BlockElement*)((char *)p+get_block_size(p)))
     {
//    	 cprintf("4\n");
    	 new_sz= get_block_size(p)+get_block_size(va);
    	 LIST_REMOVE(&freeBlocksList,(struct BlockElement *)va);
    	 set_block_data(p,new_sz,0);
    	 return;
     }
//     cprintf("a3\n");
     if(n>=(struct BlockElement*)KERNEL_HEAP_START&&nxt==n)
     {
//    	 cprintf("5\n");
    	 new_sz= get_block_size(nxt)+get_block_size(va);

    	 LIST_REMOVE(&freeBlocksList,(struct BlockElement *)va);
    	 LIST_INSERT_BEFORE(&freeBlocksList, nxt,(struct BlockElement *)va);
    	 set_block_data(va,new_sz,0);
    	 LIST_INSERT_BEFORE(&freeBlocksList, nxt,(struct BlockElement *)va);
    	 LIST_REMOVE(&freeBlocksList,nxt);
    	 return;
     }
//     cprintf("a4\n");
//    	 cprintf("6\n");
    	 new_sz=get_block_size(va);
    	 set_block_data(va,new_sz,0);


     return;

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
			return alloc_block_FF(new_size);
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

		uint32* next_block = va+old_size +2;

		struct BlockElement *current_blk;
		LIST_FOREACH(current_blk,&(freeBlocksList)){
			if(current_blk==(struct BlockElement*)(next_block+1)){
				break;
			}
		}

		struct BlockElement *next_blk = LIST_NEXT(current_blk);



		if(!((*next_block) & (uint32)1)){
			if(get_block_size(va+old_size +2)==diff_size ){
				// 1. remove next_blk from list
				LIST_REMOVE(&(freeBlocksList), next_blk);
				set_block_data(va,new_size,1);
				return va;


			}
			else if(get_block_size(va+old_size +2)>diff_size){

				if(get_block_size(va+old_size +2)-diff_size<16){
					set_block_data(va,get_block_size(va)+get_block_size(va+old_size +2),1);
					LIST_REMOVE(&(freeBlocksList), next_blk);
				}
				else {
					set_block_data(va+new_size+1,get_block_size(va+old_size +2)-diff_size,0);
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
