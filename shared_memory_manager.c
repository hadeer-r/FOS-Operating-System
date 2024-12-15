
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
	LIST_INIT(&AllShares.shares_list);
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

	if( numOfFrames <= 0){
		return NULL;
	}
	//cprintf("\n---->start calling kmalloc in create frame storage\n");
	acquire_spinlock(&MemFrameLists.mfllock);
	struct FrameInfo** storage_array = kmalloc(numOfFrames*sizeof(struct  FrameInfo* ));
	release_spinlock(&MemFrameLists.mfllock);

	    if (storage_array == NULL) {
	        return NULL;
	    }

	   // cprintf("\n---->end calling kmalloc in create frame storage\n");


	for(int i = 0 ; i < numOfFrames ; i++){
			storage_array[i] = NULL;
		}

    return  storage_array;
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
	//cprintf("\n---->start calling kmalloc in create share\n");
	acquire_spinlock(&MemFrameLists.mfllock);
	struct Share* sharedObj = kmalloc(sizeof(struct Share));
	release_spinlock(&MemFrameLists.mfllock);
	uint32 num_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	//cprintf("\n---->end calling kmalloc in create share\n");


	if (sharedObj == NULL) {
		return NULL;
	}

	sharedObj->n_frames=num_pages;
	sharedObj->ownerID = ownerID;
	sharedObj->size = size;
	sharedObj->isWritable = isWritable;
	sharedObj->references = 1;
	sharedObj->ID = ((int32)sharedObj & 0x7FFFFFFF);

	strcpy(sharedObj->name, shareName);
	sharedObj->framesStorage = create_frames_storage(num_pages);

		if (sharedObj->framesStorage  == NULL) {
//			acquire_spinlock(&MemFrameLists.mfllock);
			kfree(sharedObj->framesStorage );
//			release_spinlock(&MemFrameLists.mfllock);
			return NULL;
		}

	return sharedObj;
}//=============================
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
    struct Share* current_share;

    //cprintf("Searching for shared object with name: %s, ownerID: %d\n", name, ownerID);
    acquire_spinlock(&AllShares.shareslock);

    LIST_FOREACH(current_share, &AllShares.shares_list) {
       // cprintf("Checking object: %s, ownerID: %d\n", current_share->name, current_share->ownerID);

        if (current_share->ownerID == ownerID && strcmp(current_share->name, name) == 0) {
           // cprintf("Found shared object: %s, ownerID: %d\n", current_share->name, current_share->ownerID);
            release_spinlock(&AllShares.shareslock);
            return current_share; // Found the shared object
        }
    }

    //cprintf("Shared object not found.\n");
    release_spinlock(&AllShares.shareslock);
    return NULL; // Not found
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
		if(get_share(ownerID, shareName)!=NULL){
		return E_SHARED_MEM_EXISTS;
	}

	struct Share* newShare = create_share(ownerID,shareName,size,isWritable);

	    if (newShare == NULL)
	    {
	        return E_NO_SHARE;
	    }
		struct Env* myenv = get_cpu_proc();

	    uint32 n_pages=ROUNDUP(size,PAGE_SIZE)/PAGE_SIZE;

		for (uint32 i =0 ; i< n_pages;i++)
	    {
			uint32 va=(uint32)virtual_address+i*PAGE_SIZE;
			struct FrameInfo * frame=NULL;
				    allocate_frame(& frame);
				    map_frame(myenv->env_page_directory,frame,va,PERM_WRITEABLE | PERM_PRESENT | PERM_USER | PERM_MARKED);

				    newShare->framesStorage[i]=frame;
	    }


	    acquire_spinlock(&AllShares.shareslock);
	    LIST_INSERT_TAIL(&AllShares.shares_list, newShare);
	    release_spinlock(&AllShares.shareslock);
	    cprintf("id : %d \n",newShare->ID);

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

	    acquire_spinlock(&AllShares.shareslock);
	    uint32 nsize=ROUNDUP(share->size,PAGE_SIZE)/PAGE_SIZE;
	       for (uint32 i = 0; i < nsize; i++)
	          {

	    	  // cprintf("da5l elfor loop \n");

	              uint32* va = virtual_address +( i * PAGE_SIZE);

	             // cprintf("da5el y map  \n");
	              uint32 perms = PERM_USER;
	                    if (share->isWritable) {
	                        perms |= PERM_WRITEABLE;
	                    }
	                    map_frame(myenv->env_page_directory, share->framesStorage[i], (uint32)va, perms);

	             // cprintf("mapmazbota \n");
	             // cprintf("i = %d ,n_frames= %d \n",i ,share->n_frames);
	          }
	       	   release_spinlock(&AllShares.shareslock);

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
//========================
// [B2] Free Share Object:
//========================


void free_share(struct Share* ptrShare) {
    // Validate input
    if (!ptrShare) {
        cprintf("Error: free_share called with NULL share pointer\n");
        return;
    }
    struct Env* myenv = get_cpu_proc();
    cprintf("free_share: Starting for ShareID=%d with n_frames=%d\n",
            ptrShare->ID, ptrShare->n_frames);

    // Safely handle framesStorage
    if (ptrShare->framesStorage) {
unmap_frame(myenv->env_page_directory,(uint32)ptrShare->framesStorage);
        /*for (uint32 i = 0; i < ptrShare->n_frames; i++) {
            // Careful validation before freeing
             if (ptrShare->framesStorage[i] != NULL) {
                // Additional debug information

                // Only free if references reach zero
                if (ptrShare->framesStorage[i]->references == 0) {
                    // Attempt to free only if it's safe
                    free_frame(ptrShare->framesStorage[i]);

                } else {
                    cprintf("Frame %d still has %d references. Not freed.\n",
                            i, ptrShare->framesStorage[i]->references);
                }
            }
        }*/

        // Free the framesStorage array itself
        kfree(ptrShare->framesStorage);
        cprintf("free_share: Freed framesStorage array\n");
    } else {
        cprintf("free_share: framesStorage is NULL. No frames to deallocate.\n");
    }

    // Free the shared object structure
    kfree(ptrShare);
    cprintf("free_share: Completed share deallocation\n");
}

int freeSharedObject(int32 sharedObjectID, void *startVA) {
    // Input validation
    if (!startVA) {
        cprintf("Error: freeSharedObject called with NULL virtual address\n");
        return E_SHARED_MEM_NOT_EXISTS;
    }

    struct Env* myenv = get_cpu_proc();
    if (!myenv) {
        cprintf("Error: Calling environment is NULL\n");
        return E_SHARED_MEM_NOT_EXISTS;
    }

    // Align virtual address to page boundary
    uint32 va = (uint32)startVA;
    va = ROUNDDOWN(va, PAGE_SIZE);

    struct Share* current_share = NULL;
    uint32* ptr_page_table = NULL;

    // Safely locate the shared object
    acquire_spinlock(&AllShares.shareslock);
    int share_found = 0;
    LIST_FOREACH(current_share, &AllShares.shares_list) {
        if (current_share->ID == sharedObjectID) {
            share_found = 1;
            break;
        }
    }

    if (!share_found) {
        release_spinlock(&AllShares.shareslock);
        cprintf("Error: Shared object with ID=%d not found\n", sharedObjectID);
        return E_SHARED_MEM_NOT_EXISTS;
    }

    // Detailed logging of current share state
    cprintf("freeSharedObject: Found share ID=%d, References=%d, Frames=%d\n",
            current_share->ID, current_share->references, current_share->n_frames);

    // Unmap all frames associated with this shared object
    uint32 original_va = va;
    for (uint32 i = 0; i < current_share->n_frames; i++) {
        if (get_page_table(myenv->env_page_directory, va, &ptr_page_table) == TABLE_IN_MEMORY) {
            unmap_frame(myenv->env_page_directory, va);
            cprintf("freeSharedObject: Unmapped frame at VA=0x%x\n", va);

            // Clean up page directory if table is unused
            if (pd_is_table_used(myenv->env_page_directory, va) == 0) {
                pd_clear_page_dir_entry(myenv->env_page_directory, va);
                pd_set_table_unused(myenv->env_page_directory, va);

                if (ptr_page_table) {
                    kfree(ptr_page_table);
                    cprintf("freeSharedObject: Cleared page directory entry for VA=0x%x\n", va);
                }
            }
        }
        va += PAGE_SIZE;
    }

    // Decrement and check reference count
    current_share->references--;
    cprintf("freeSharedObject: References after decrement: %d\n", current_share->references);

    // If no references remain, fully deallocate
    if (current_share->references <= 0) {
        LIST_REMOVE(&AllShares.shares_list, current_share);
        release_spinlock(&AllShares.shareslock);

        // Complete deallocation
        free_share(current_share);
        cprintf("freeSharedObject: Shared object fully deallocated\n");

        // Flush TLB to apply all changes
        tlbflush();
        return 0;
    }

    release_spinlock(&AllShares.shareslock);
    cprintf("freeSharedObject: Shared object still in use. Skipping full deallocation.\n");

    return 0;
}


int32 getsharedid(void* virtual_address) {
    cprintf("Starting getsharedid for VA=0x%x\n", (uint32)virtual_address);

    struct Env* myenv = get_cpu_proc();
    if (!myenv) {
        return E_SHARED_MEM_NOT_EXISTS; // Invalid calling environment
    }

    uint32* ptr_page_table;
    struct FrameInfo* target_frame = get_frame_info(
        myenv->env_page_directory, (uint32)virtual_address, &ptr_page_table
    );

    if (!target_frame) {
        cprintf("No frame mapped to this virtual address.\n");
        return E_SHARED_MEM_NOT_EXISTS;
    }

    struct Share* current_share = NULL;
    acquire_spinlock(&AllShares.shareslock);
    LIST_FOREACH(current_share, &AllShares.shares_list) {
        for (int i = 0; i < current_share->n_frames; i++) {
            if (current_share->framesStorage[i] == target_frame) {
                release_spinlock(&AllShares.shareslock);
                cprintf("Found shared object ID=%d\n", current_share->ID);
                return current_share->ID;
            }
        }
    }
    release_spinlock(&AllShares.shareslock);

    cprintf("No shared object found for this frame.\n");
    return E_SHARED_MEM_NOT_EXISTS;
}
