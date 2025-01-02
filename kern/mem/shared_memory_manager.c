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
struct ShareInfo {
	int32 address ;
	int32 ID ;
};
static uint32 share_count = 0;
static struct ShareInfo shareInfo[NUM_OF_UHEAP_PAGES];
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
	acquire_spinlock(&AllShares.shareslock);
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL){
		release_spinlock(&AllShares.shareslock);
		return E_SHARED_MEM_NOT_EXISTS;}
	else{
		release_spinlock(&AllShares.shareslock);
		return ptr_share->size;
	}
	release_spinlock(&AllShares.shareslock);
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
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_frames_storage is not implemented yet");
	//Your Code is Here...
	if(numOfFrames<=0)
		return NULL;
    struct FrameInfo** frames_storage = (struct FrameInfo**) kmalloc(numOfFrames * sizeof(struct FrameInfo *));
    if (frames_storage == NULL)
    {
    	return NULL;
    }

  for(int i=0;i<numOfFrames;i++)
  {
	  frames_storage[i]=0;
  }
    return frames_storage;


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
	struct Share* sharedObj=NULL;
 sharedObj = (struct Share*) kmalloc(sizeof(struct Share));
    if(sharedObj==NULL)
    	return NULL;
    sharedObj->ownerID=ownerID;
    strcpy(sharedObj->name, shareName);
    sharedObj->size=size;
    if(isWritable==1)
    {
    	sharedObj->isWritable=PERM_WRITEABLE;
    }
    else
    	sharedObj->isWritable=0;
    sharedObj->ID=((uint32)sharedObj) & ~MSB_MASK;
unsigned int  numOfFrames=(unsigned int )((size+PAGE_SIZE-1)/PAGE_SIZE);
struct FrameInfo** frames_storage = create_frames_storage(numOfFrames);
if(frames_storage==NULL)
{
	kfree(sharedObj);
    return NULL;
	}
sharedObj->references=1;
sharedObj->framesStorage=frames_storage;
return sharedObj;
}

//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID , char* name)
{
    // TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
    // COMMENT THE FOLLOWING LINE BEFORE START CODING
    // panic("get_share is not implemented yet");
	int flag=0;
    struct Share *myptr = AllShares.shares_list.lh_first;
    while (myptr != NULL)
    {
        if (myptr->ownerID == ownerID && strcmp(myptr->name, name) == 0)
        {
            flag=1;
            break;
        }
        myptr = LIST_NEXT(myptr);
    }
    if(flag==0)
    return NULL;
    else
    return myptr;
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
	acquire_spinlock(&AllShares.shareslock);
	struct Env* myenv = get_cpu_proc(); //The calling environment
	struct Share* ptr_shared=NULL;
	ptr_shared=get_share(ownerID, shareName);
	if(ptr_shared!=NULL){
		release_spinlock(&AllShares.shareslock);
		return E_SHARED_MEM_EXISTS;
	}
   ptr_shared= create_share(ownerID, shareName, size, isWritable);
   shareInfo[share_count].ID=ptr_shared->ID;
   shareInfo[share_count].address=(uint32)virtual_address;
   share_count++;
	if(ptr_shared==NULL || ptr_shared->framesStorage==NULL){
		release_spinlock(&AllShares.shareslock);
		return E_NO_SHARE ;
	}
	uint32  num_frame = (uint32)((ptr_shared->size + PAGE_SIZE - 1) / PAGE_SIZE);
	uint32 total_size = (num_frame * PAGE_SIZE);
	int count=0;
	for(int i=0;i<num_frame;i++)
	{
		struct FrameInfo*ptr_frame_info=NULL;
		int ret=allocate_frame(&ptr_frame_info);
		ptr_shared->framesStorage[count]=ptr_frame_info;
		if (ret != 0) {release_spinlock(&AllShares.shareslock);
				    return E_NO_SHARE;
				}
		int ret2 = map_frame(myenv->env_page_directory, ptr_frame_info, (uint32)virtual_address,  PERM_WRITEABLE |MARK_BIT |PERM_USER);
		if (ret2 != 0) {release_spinlock(&AllShares.shareslock);
		    return E_NO_SHARE;
		}
		virtual_address+=PAGE_SIZE;
		count++;
	}

	LIST_INSERT_TAIL(&AllShares.shares_list, ptr_shared);
	release_spinlock(&AllShares.shareslock);
	return ptr_shared->ID;


}
int32 shared_id(uint32 virtual_address)
{
	int32 id=0;
	for(int i=0;i<share_count;i++)
   {
		if(virtual_address==shareInfo[i].address)
     {
	   id=shareInfo[i].ID;
	   break;
     }
   }
	return id;
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
	acquire_spinlock(&AllShares.shareslock);

	struct Env* myenv = get_cpu_proc(); //The calling environment
	struct Share* ptr_share=get_share(ownerID, shareName);
	if(ptr_share==NULL){
		release_spinlock(&AllShares.shareslock);
	 return E_SHARED_MEM_NOT_EXISTS ;
	}
	unsigned int  numOfFrames=(unsigned int )((ptr_share->size+ PAGE_SIZE -1)/PAGE_SIZE);
	for(int i=0;i<numOfFrames;i++)
	{

		struct FrameInfo*ptr_frame_info=ptr_share->framesStorage[i];
        int ret2 = map_frame(myenv->env_page_directory,ptr_frame_info, (uint32)virtual_address ,  ptr_share->isWritable |MARK_BIT |PERM_USER);
        virtual_address+=PAGE_SIZE;
	}
	ptr_share->references+=1;
	   shareInfo[share_count].ID=ptr_share->ID;
	   shareInfo[share_count].address=(uint32)virtual_address;
	   share_count++;
	release_spinlock(&AllShares.shareslock);
	return ptr_share->ID;


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
	//panic("free_share is not implemented yet");
	//Your Code is Here...
	//---------------------------------------------------
    //NUMBER 1: delete it
    LIST_REMOVE(&(AllShares.shares_list),ptrShare);
	//---------------------------------------------------
    //NUMBER 2: delete the "framesStorage" and the shared object itself
    kfree(ptrShare->framesStorage);
    kfree(ptrShare);
}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("freeSharedObject is not implemented yet");
	//Your Code is Here...
	//--------------------------------------------------------------
	//NUMBER 1: get the shared object from the list
	struct Share *ptrShare=AllShares.shares_list.lh_first;
	while(ptrShare!=NULL){
			if(ptrShare->ID == sharedObjectID){
	            break;
			}
		    ptrShare = LIST_NEXT(ptrShare);
		}
    if (ptrShare == NULL) {
        return E_SHARED_MEM_NOT_EXISTS;
    }
    struct Env* myenv = get_cpu_proc();
	//--------------------------------------------------------------
    //NUMBER 2: unmap it
    uint32 size= getSizeOfSharedObject(ptrShare->ownerID, ptrShare->name);
     int pagesNum =ROUNDUP(ptrShare->size,PAGE_SIZE)/PAGE_SIZE;
     for(int i=0; i<pagesNum;i++){
     	unmap_frame( myenv->env_page_directory , ((uint32) startVA + ( i * PAGE_SIZE )));
     }
	//--------------------------------------------------------------
    //NUMBER 3: remove page table if empty
    for ( uint32 i = (uint32)startVA; i < size+(uint32)startVA; i+=PAGE_SIZE) {
        uint32* page_table = NULL;
        int ret = get_page_table(myenv->env_page_directory, (uint32)i, &page_table);
        if(page_table!=NULL){
            bool isEmpty = 1;
            for (int j = 0; j < 1024; j++) {
                if (page_table[j] & PERM_PRESENT) {
                    isEmpty = 0;
                    break;
                }
            }
            if (isEmpty) {
            	myenv->env_page_directory[PDX(i)] = 0 ;
                kfree(page_table);
            }
        }
    }
	//--------------------------------------------------------------
    //NUMBER 4: update references
    ptrShare->references--;
	//--------------------------------------------------------------
    //NUMBER 5: if last share --> delete the share object
    if (ptrShare->references == 0) {
        free_share(ptrShare);
    }
	//--------------------------------------------------------------
    //NUMBER 6: Flush the cache
    tlbflush();

    return 0;
}
