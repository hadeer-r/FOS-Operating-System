#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
// Create the frames_storage and initialize it by 0
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_frames_storage is not implemented yet");
	//Your Code is Here...
	static struct FrameInfo** storage_array[NUM_OF_UHEAP_PAGES];

	if(numOfFrames > NUM_OF_UHEAP_PAGES || numOfFrames == 0){
		return NULL;
	}

	for(int i = 0 ; i < numOfFrames ; i++){
		storage_array[i] = 0;
	}

    return (struct FrameInfo**) storage_array;
}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_share is not implemented yet");
	//Your Code is Here...
	struct Share* sharedObj = kmalloc(size);

	if (sharedObj == NULL) {
		return NULL;
	}

	struct FrameInfo** framesStorage = create_frames_storage((size / PAGE_SIZE));
	if (framesStorage == NULL) {
		return NULL;
	}

	sharedObj->ownerID = ownerID;
	sharedObj->size = size;
	sharedObj->isWritable = isWritable;
	sharedObj->references = 1;
	sharedObj->ID = (int32)((int32)sharedObj & 0x7FFFFFFF);
	sharedObj->framesStorage = framesStorage;
	strcpy(sharedObj->name, shareName);

	return sharedObj;
}
//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_share is not implemented yet");
	//Your Code is Here...
	struct Share* current_share ;
	acquire_spinlock(&AllShares.shareslock);
	LIST_FOREACH(current_share, &AllShares.shares_list)
	    {

	        if (current_share->ownerID == ownerID && strcmp(current_share->name, name) == 0)
	        {

	            return current_share;
	        }
	    }
	 release_spinlock(&AllShares.shareslock);

	    return NULL;

}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("createSharedObject is not implemented yet");
	//Your Code is Here...

	struct Env* myenv = get_cpu_proc(); //The calling environment
	if (myenv == NULL)
	    {
	        return E_NO_SHARE;
	    }


	    if (get_share(ownerID, shareName) != NULL)
	    {
	        return E_SHARED_MEM_EXISTS;
	    }


	    struct Share* newShare = (struct Share*)kmalloc(sizeof(struct Share));
	    if (newShare == NULL)
	    {
	        return E_NO_SHARE; // Memory allocation failed
	    }

	    // Initialize the Share object
	    newShare->ownerID = ownerID;
	    strncpy(newShare->name, shareName, sizeof(newShare->name) - 1);
	    newShare->name[sizeof(newShare->name) - 1] = '\0';
	    newShare->size = size;
	    newShare->isWritable = isWritable;
	    newShare->references = 0;


	    unsigned int upsize = ROUNDUP(size, PAGE_SIZE);
	        unsigned int num_of_pages = upsize / PAGE_SIZE;


	    newShare->framesStorage = (struct FrameInfo**)kmalloc(num_of_pages * sizeof(struct FrameInfo*));
	    if (newShare->framesStorage == NULL)
	    {
	        kfree(newShare);
	        return E_NO_SHARE;
	    }


	    for (uint32 i = 0; i < num_of_pages; ++i)
	    {
	        struct FrameInfo* frame ;


	        if (allocate_frame(&frame) != 0)
	        {

	            for (uint32 j = 1; j < i; ++j)
	            {
	                free_frame(newShare->framesStorage[j]);
	            }
	            kfree(newShare->framesStorage);
	            kfree(newShare);
	            return E_NO_SHARE;
	        }


	        newShare->framesStorage[i] = frame;


	        uint32 * va = (virtual_address + i * PAGE_SIZE);
	        if (map_frame(ptr_page_directory, frame, (uint32)va, PERM_WRITEABLE) != 0)
	        {

	            free_frame(frame);
	            for (uint32 j = 1; j < i; ++j)
	            {
	                free_frame(newShare->framesStorage[j]);
	            }
	            kfree(newShare->framesStorage);
	            kfree(newShare);
	            return E_NO_SHARE;
	        }
	    }


	    newShare->ID = (int)newShare & 0x7FFFFFFF;


	    acquire_spinlock(&AllShares.shareslock);
	    LIST_INSERT_HEAD(&AllShares.shares_list, newShare);
	    release_spinlock(&AllShares.shareslock);

	    return newShare->ID;
	}


//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("getSharedObject is not implemented yet");
	//Your Code is Here...

	struct Env* myenv = get_cpu_proc(); //The calling environment
	    if (myenv == NULL)
	    {
	        return E_SHARED_MEM_NOT_EXISTS;
	    }
	    struct Share* share = get_share(ownerID, shareName);
	       if (share == NULL)
	       {
	           return E_SHARED_MEM_NOT_EXISTS;
	       }
	       unsigned int upsize = ROUNDUP(share->size, PAGE_SIZE);
	           unsigned int num_of_pages = upsize / PAGE_SIZE;
	       for (uint32 i = 1; i < num_of_pages; ++i)
	          {
	              struct FrameInfo* frame = share->framesStorage[i];


	              uint32* va = (virtual_address + i * PAGE_SIZE);


	              if (map_frame(ptr_page_directory, frame, (uint32)va, share->isWritable) != 0)
	              {
	                  return E_SHARED_MEM_NOT_EXISTS;
	              }
	          }


	          share->references++;


	          return share->ID;
	      }

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
void free_share(struct Share* ptrShare)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("free_share is not implemented yet");
	//Your Code is Here...

}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("freeSharedObject is not implemented yet");
	//Your Code is Here...

}
