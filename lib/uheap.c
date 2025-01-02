#include <inc/lib.h>

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct AllocationInfo2 {
	uint32 address;
	uint32 size;
	int is_free;
}AllocationInfo2;
//gbt mmkn yshel kam allocation
static struct AllocationInfo2 allocations2[NUM_OF_UHEAP_PAGES];
static uint32 count2 = 0;
//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*)sys_sbrk(increment);
}


//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//USER SIDE
	//struct Env* e=NULL;
	  uint32 va=(uint32)(myEnv->UH_limit  + PAGE_SIZE);

	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

	if(sys_isUHeapPlacementStrategyFIRSTFIT()==(uint32)1){

		if(size <= DYN_ALLOC_MAX_BLOCK_SIZE){
			uint32* add22=(uint32*)alloc_block_FF(size);
			return (void*)add22;
		}

		//i want to allocate each address and mark(no need for saving size)
		else if(size>DYN_ALLOC_MAX_BLOCK_SIZE){
			uint32 numframes=(unsigned int )((size+PAGE_SIZE-1)/PAGE_SIZE);
			int add;
			int index;
			int found=0;
			int found2=0;
			int exist=0;
			int index2;
			for(int j=0;j<count2;j++){
					uint32 numframes_split=(unsigned int )((allocations2[j].size+PAGE_SIZE-1)/PAGE_SIZE);

				if(numframes_split>=numframes && allocations2[j].is_free==0){
					exist=1;
					index=j;
					break;

				}
			}

			// EMPTY PLACE FOUND SO ALLOCATE
			if(exist){
				if(allocations2[index].size>numframes){
					for(int i=count2;i>index+1;i--){
						allocations2[i]=allocations2[i-1];
					}
					uint32 numframes_split=(unsigned int )((allocations2[index].size+PAGE_SIZE-1)/PAGE_SIZE);

					allocations2[index+1].address = allocations2[index].address+(numframes*PAGE_SIZE);
					allocations2[index+1].size=(numframes_split*PAGE_SIZE)-(numframes*PAGE_SIZE);
					allocations2[index+1].is_free = 0;
					count2++;

				}


				allocations2[index].size = (numframes*PAGE_SIZE);
				allocations2[index].is_free = 1;

				sys_allocate_user_mem(allocations2[index].address,size);
				return (void*)allocations2[index].address;
			}


			//NO EMPTY PLACE FOUND SO ALLOCATE LAST
			else{
				uint32 address;
				if (count2==0) {
				//	cprintf("FIRST ALLOCATE");

					address=va;
					if(address+PAGE_SIZE*numframes>USER_HEAP_MAX){
						return NULL;
					}
				}
				else{
					uint32 frame_num=((allocations2[count2-1].size+PAGE_SIZE-1)/PAGE_SIZE);
					address=allocations2[count2-1].address+(frame_num*PAGE_SIZE);
					if(address+PAGE_SIZE*numframes>USER_HEAP_MAX){
						return NULL;
					}
				//	cprintf("NEXT WAS NOT ALLOCATED BEFOREEEE");
				}

				allocations2[count2].address = address;
				allocations2[count2].size = (numframes*PAGE_SIZE);
				allocations2[count2].is_free = 1;
				count2++;
				//cprintf("ADDRESS %d",address);

				sys_allocate_user_mem(address,size);
				return (void*)address;

			}

		}
	return NULL;
	}
	return NULL;

}
//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================

void free(void* virtual_address)
{
	 //TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
   //panic("free() is not implemented yet...!!");
	if(sys_isUHeapPlacementStrategyFIRSTFIT()==(uint32)1){
		uint32 va = (uint32)virtual_address;
		uint32 block_size = 0;
		uint32 dd=0;
		uint32 f =0;
		int block=0;
		uint32 saved=-1;
		uint32 saved2=-1;
		if((uint32)virtual_address>=myEnv->UH_start &&(uint32)virtual_address<myEnv->UH_limit){
			free_block(virtual_address);
		}

		for (int i = 0; i < count2; i++) {
			if (allocations2[i].address == (uint32)virtual_address && allocations2[i].is_free == 1) {
				allocations2[i].is_free = 0;
				dd=(uint32)virtual_address;
				block_size=allocations2[i].size;
				f=1;
				saved2=i;
				saved=i;
			}
		}
		if(f==0)return;
		 if(f==1){

			 //MERGE NEXT
			 uint32 next=0;
			 if (saved + 1 < count2 && allocations2[saved + 1].is_free == 0) {
				uint32 numframes=(unsigned int )((allocations2[saved].size+PAGE_SIZE-1)/PAGE_SIZE);
				uint32 numframes_next=(unsigned int )((allocations2[saved+1].size+PAGE_SIZE-1)/PAGE_SIZE);
				 allocations2[saved].size=(numframes*PAGE_SIZE)+(numframes_next*PAGE_SIZE);
				//allocations2[saved].is_free = 0;

				for(int i=saved+1;i<count2-1;i++){
					allocations2[i]=allocations2[i+1];
				}
				count2--;
				next=1;
			 }

			 //MERGE PREV
			 uint32 prev=0;
			 if (saved > 0 && allocations2[saved2 - 1].is_free == 0) {
				uint32 numframes=(unsigned int )((allocations2[saved].size+PAGE_SIZE-1)/PAGE_SIZE);
				uint32 numframes_next=(unsigned int )((allocations2[saved-1].size+PAGE_SIZE-1)/PAGE_SIZE);
				 allocations2[saved2-1].size = (numframes*PAGE_SIZE)+(numframes_next*PAGE_SIZE);
				 allocations2[saved2-1].is_free = 0;
					for(int i=saved2;i<count2-1;i++){
						allocations2[i]=allocations2[i+1];
					}
					count2--;
				 prev=1;
			 }

		 if((uint32)virtual_address>=myEnv->UH_limit +PAGE_SIZE){

				sys_free_user_mem((uint32)virtual_address,block_size);
			}
		 else{
			 panic("no va for page allocator/block allocator ");
		 }
	 }



	}
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	if (size == 0)
		return NULL;

	uint32 va = (uint32)(myEnv->UH_limit + PAGE_SIZE);
	uint32 numframes = (unsigned int)((size + PAGE_SIZE - 1) / PAGE_SIZE);
	int exist = 0;
	int index;

	// Check for an existing allocation that can fit the requested size
	for(int j=0;j<count2;j++){
			uint32 numframes_split=(unsigned int )((allocations2[j].size+PAGE_SIZE-1)/PAGE_SIZE);

		if(numframes_split>=numframes && allocations2[j].is_free==0){
			exist=1;
			index=j;
			break;

		}
	}

	// If a suitable free allocation exists
	if(exist){
		if(allocations2[index].size>numframes){
			for(int i=count2;i>index+1;i--){
				allocations2[i]=allocations2[i-1];
			}
			uint32 numframes_split=(unsigned int )((allocations2[index].size+PAGE_SIZE-1)/PAGE_SIZE);

			allocations2[index+1].address = allocations2[index].address+(numframes*PAGE_SIZE);
			allocations2[index+1].size=(numframes_split*PAGE_SIZE)-(numframes*PAGE_SIZE);
			allocations2[index+1].is_free = 0;
			count2++;

		}


		allocations2[index].size = (numframes*PAGE_SIZE);
		allocations2[index].is_free = 1;

		int ret = sys_createSharedObject(sharedVarName, size, isWritable, (void*)allocations2[index].address);
		if (ret == E_SHARED_MEM_EXISTS){
					   return NULL;
			}

		   if (ret == E_NO_SHARE)
		   {
			   return NULL;
		   }
		return (void*)allocations2[index].address;
	} else {
		// Allocate new memory
		uint32 address;
		if (count2 == 0) {
			address = va;
			if (address + PAGE_SIZE * numframes > USER_HEAP_MAX) {
				return NULL;
			}
		} else {
			uint32 frame_num = ((allocations2[count2 - 1].size + PAGE_SIZE - 1) / PAGE_SIZE);
			address = allocations2[count2 - 1].address + (frame_num * PAGE_SIZE);
			if (address + PAGE_SIZE * numframes > USER_HEAP_MAX) {
				return NULL;
			}
		}
		allocations2[count2].address = address;
		allocations2[count2].size = (numframes*PAGE_SIZE);
		allocations2[count2].is_free = 1;
		count2++;

		int ret = sys_createSharedObject(sharedVarName, size, isWritable, (void*)address);
		if (ret == E_SHARED_MEM_EXISTS){
				   return NULL;
		}

	   if (ret == E_NO_SHARE)
	   {
		   return NULL;
	   }
		return (void*)address;
	}
	return(void*)NULL;
}
//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
//	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
//	// Write your code here, remove the panic and write your code
//	panic("sget() is not implemented yet...!!");
	int size=sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
	 if (size == 0 ||size == E_SHARED_MEM_NOT_EXISTS )
		  return NULL;

		uint32 va = (uint32)(myEnv->UH_limit + PAGE_SIZE);
		uint32 numframes = (unsigned int)((size + PAGE_SIZE - 1) / PAGE_SIZE);
		int exist = 0;
		int index;

		// Check for an existing allocation that can fit the requested size
		for(int j=0;j<count2;j++){
				uint32 numframes_split=(unsigned int )((allocations2[j].size+PAGE_SIZE-1)/PAGE_SIZE);

			if(numframes_split>=numframes && allocations2[j].is_free==0){
				exist=1;
				index=j;
				break;

			}
		}



		// If a suitable free allocation exists
		if(exist){
		if(allocations2[index].size>numframes){
			for(int i=count2;i>index+1;i--){
				allocations2[i]=allocations2[i-1];
			}
			uint32 numframes_split=(unsigned int )((allocations2[index].size+PAGE_SIZE-1)/PAGE_SIZE);

			allocations2[index+1].address = allocations2[index].address+(numframes*PAGE_SIZE);
			allocations2[index+1].size=(numframes_split*PAGE_SIZE)-(numframes*PAGE_SIZE);
			allocations2[index+1].is_free = 0;
			count2++;

		}


		allocations2[index].size = (numframes*PAGE_SIZE);
		allocations2[index].is_free = 1;


			int ret2=sys_getSharedObject(ownerEnvID, sharedVarName, (void*)allocations2[index].address);
			if (ret2 == E_SHARED_MEM_NOT_EXISTS)
				return NULL;

			if (ret2 == E_NO_SHARE)
				return NULL;
			//cprintf("end sget1\n");
			return (void*)allocations2[index].address;
		} else {
			// Allocate new memory
			uint32 address;
			if (count2 == 0) {
				address = va;
				if (address + PAGE_SIZE * numframes > USER_HEAP_MAX)
					return NULL;
			} else {
				uint32 frame_num = ((allocations2[count2 - 1].size + PAGE_SIZE - 1) / PAGE_SIZE);
				address = allocations2[count2 - 1].address + (frame_num * PAGE_SIZE);
				if (address + PAGE_SIZE * numframes > USER_HEAP_MAX)
					return NULL;

			}

			allocations2[count2].address = address;
			allocations2[count2].size = (numframes*PAGE_SIZE);
			allocations2[count2].is_free = 1;
			count2++;

			int ret=sys_getSharedObject(ownerEnvID, sharedVarName, (void*)address);
			if (ret == E_SHARED_MEM_NOT_EXISTS|| ret < 0)
					   return NULL;


				   if (ret == E_NO_SHARE)
					   return NULL;

			//cprintf("end sget2\n");
			return (void*)address;
		}

		return(void*)NULL;
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
	//panic("sfree() is not implemented yet...!!");

	//----------------------------------------------------------------------
	//NUMBER 1: find the id
	int32 id=sys_shared_id((uint32)virtual_address);

	 //NUMBER 2: call to free
	  int ret = sys_freeSharedObject(id, virtual_address);
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
