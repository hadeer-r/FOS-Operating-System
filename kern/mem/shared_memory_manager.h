
#ifndef FOS_SHARED_MEMORY_MANAGER_H
#define FOS_SHARED_MEMORY_MANAGER_H
#ifndef FOS_KERNEL
#endif

//#include <inc/types.h>
//#include <inc/queue.h>
//#include <inc/trap.h>
//#include <inc/memlayout.h>
#include <inc/environment_definitions.h>
#include <kern/conc/spinlock.h>
struct idref
{
	int32 id ;
	uint32 refid ;
};
struct Share
{
	//Unique ID for this Share object
	//Should be set to the VA of the created object
	//after masking the most significant bit (to make it +ve)
	int32 ID ;
	//ID of the owner environment
	int32 ownerID ;
	//share name
	char name[64];
	//share size
	int size;
	//references, number of envs looking at this shared mem object
	uint32 references;
	//sharing permissions (0: ReadOnly, 1:Writable)
	uint8 isWritable;
uint32  freeva[100];
	//to store frames to be shared
	struct FrameInfo** framesStorage;
	uint32 n_frames;
	// list link pointers
	LIST_ENTRY(Share) prev_next_info;

};


//List of all shared objects
LIST_HEAD(Share_List, Share);		// Declares 'struct Share_List'
//LIST_HEAD(id_List, idforfree);
#if USE_KHEAP == 0
    // Max number of shared objects when using static memory allocation
    #define MAX_SHARES 100
    struct Share shares[MAX_SHARES];
   // struct idforfree ids[MAX_SHARES];
#else
    struct
    {
        struct Share_List shares_list; // List of all share objects created by any process
        struct spinlock shareslock;    // Spinlock to protect the shares_list in the kernel
    } AllShares;


    void sharing_init();
#endif


int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address);
int getSizeOfSharedObject(int32 ownerID, char* shareName);
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address);
int freeSharedObject(int32 sharedObjectID, void *startVA);
int32 getsharedid(void* virtual_adress);

#endif /* FOS_SHARED_MEMORY_MANAGER_H */
