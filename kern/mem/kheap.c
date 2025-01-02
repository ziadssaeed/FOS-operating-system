#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"
//
uint32 frame_map[TOTAL_FRAMES] = {0};
uint32 block_map[TOTAL_FRAMES] = {0};

struct AllocationInfo {
    uint32 address;
    uint32 size;
    int is_free;
};

static int kernel_lock_is_init=0;
static struct AllocationInfo allocations[NUM_OF_KHEAP_PAGES];
static uint32 count = 0;
//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC

uint32 myLIMIt;
void init_kernel_lock()
{ if(kernel_lock_is_init==1)
	return;
init_spinlock(&kernel_lock,"kernel_lock");

kernel_lock_is_init=1;
return;
	}
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code
	//panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");

	KH_start=daStart;
	KH_limit=daLimit;
	myLIMIt=KH_limit;
	KH_brk=initSizeToAllocate+daStart;
	int t=0;
	int m=0;
	for(int i =daStart;i<KH_brk;i += 4096 )
	{
		uint32*ptr_page_table=NULL;
		struct FrameInfo * ptr_frame_info=get_frame_info(ptr_page_directory,i,&ptr_page_table);
		int ret=allocate_frame(&ptr_frame_info);
		if(ret==E_NO_MEM){
			m=1;

			panic("there is no available frames to allocate ");
		}
		else
		{
			int ret2=map_frame(ptr_page_directory,(void*)ptr_frame_info,i,PERM_PRESENT |PERM_WRITEABLE);
			if(ret2==E_NO_MEM){
				free_frame(ptr_frame_info);
				t=1;

					panic("there is no space ");
				}
		}

	}
	//release_spinlock(&MemFrameLists.mfllock);
	if(m==1 || t==1)
		return E_NO_MEM;
	else{
	initialize_dynamic_allocator(daStart,initSizeToAllocate);
	return 0;
	}

}
void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
	//return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code
	//panic("sbrk() is not implemented yet...!!");
	uint32 oldbreak=KH_brk;
	int flag1 =0;
	int flag2 =0;
	if(numOfPages==0)
		return (void*) oldbreak;
	uint32 expand_size=(numOfPages*PAGE_SIZE);
	if ((expand_size +KH_brk) > KH_limit || numOfPages<0)
	{
		return(void*) -1;
	}

	 KH_brk=KH_brk+expand_size;
		for(uint32 i =oldbreak;i<KH_brk;i+=PAGE_SIZE)
		{
			uint32* ptr_page_table=NULL;
			struct FrameInfo * ptr_frame_info=get_frame_info(ptr_page_directory,i,&ptr_page_table);
			int ret=allocate_frame(&ptr_frame_info);
			if(ret==E_NO_MEM){
					flag1=1;

					break;
		}
			int ret2=map_frame(ptr_page_directory,ptr_frame_info,i,PERM_PRESENT |PERM_WRITEABLE);
			if(ret2==E_NO_MEM){
				free_frame(ptr_frame_info);
				flag2=1;

				break;
			}
		}

			if(flag1==1||flag2==1)
	{
			KH_brk=oldbreak;
		return (void*)-1;
	}
 return (void*) oldbreak;
}
//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

//va = 1,2,3,4,5,6
//kfree=free=3
//struct=>
//kmalloc=>8->2 frames

void* kmalloc(unsigned int size)
{
	if(kernel_lock_is_init==0)
		init_kernel_lock();
  uint32 va=KH_limit  + PAGE_SIZE;
	//TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	if(isKHeapPlacementStrategyFIRSTFIT()==(uint8)1){

		if(size+va>KERNEL_HEAP_MAX){
			return NULL;
		}
		if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
			if(!holding_spinlock(&kernel_lock))
			acquire_spinlock(&kernel_lock);
			uint32* add22=(uint32*)alloc_block_FF(size);
			allocations[count].address =(uint32)add22;
			allocations[count].size =(uint32)size;
			allocations[count].is_free = 1;
			count++;
			uint32 myaddr=(uint32)add22;
			while(size--){
				uint32 pa=kheap_physical_address((uint32)myaddr);
				block_map[pa>>12]=(uint32)myaddr;
				myaddr++;
			}

					release_spinlock(&kernel_lock);
			return (void*)add22;

		}

	    else{

    		unsigned int  numframes=(unsigned int )((size+PAGE_SIZE-1)/PAGE_SIZE);
	    	int add;
	    	if(!holding_spinlock(&kernel_lock))
	    	acquire_spinlock(&kernel_lock);
	    for(int i=KH_limit + PAGE_SIZE;i<=KERNEL_HEAP_MAX-(numframes*PAGE_SIZE);i+=PAGE_SIZE){
			uint32 *ptr_page_table;
			uint32 found=1;
			struct FrameInfo* ret=get_frame_info(ptr_page_directory,va,&ptr_page_table);
			if(ret==NULL){
	    		for(int j=0;j<numframes;j++){
	    			struct FrameInfo* ret2=get_frame_info(ptr_page_directory,va+(j*PAGE_SIZE),&ptr_page_table);
	    			if(ret2!=(struct FrameInfo*)0){
	    				found=0;
	    				break;
	    			}
				}
	    	if(found==1){
	    			add=va;

	    			while(numframes>0){

	    				struct FrameInfo* ptr_frameinfo;
						int allocreturn=allocate_frame(&ptr_frameinfo);
						if(allocreturn!=0){
							release_spinlock(&kernel_lock);
							return NULL;
						}

							int mapreturn=map_frame(ptr_page_directory,ptr_frameinfo,va,PERM_WRITEABLE);
							if(mapreturn!=0){
									free_frame(ptr_frameinfo);
									release_spinlock(&kernel_lock);
								return NULL;
							}
							 uint32 framno=to_frame_number(ptr_frameinfo);
							 frame_map[framno]=va;
								numframes--;
							  va=va+(PAGE_SIZE);
					}

					allocations[count].address = add;
					allocations[count].size = size;
					allocations[count].is_free = 1;
					count++;
					release_spinlock(&kernel_lock);
					return (void*)(add);
				}
			}

	    			va=va+(PAGE_SIZE);

		}
	    release_spinlock(&kernel_lock);
	    	return NULL;
	}
}
	else if(isKHeapPlacementStrategyBESTFIT()==1){
		if(size>=KERNEL_HEAP_MAX-(KERNEL_HEAP_START + DYN_ALLOC_MAX_SIZE + PAGE_SIZE)||size==0){
			release_spinlock(&kernel_lock);
			return NULL;
		}

		//go to block alloctor
		//size <=  2kb
		if(size<=DYN_ALLOC_MAX_BLOCK_SIZE){
			release_spinlock(&kernel_lock);
			return alloc_block_BF(size);

		}

		else{
				uint32 add=va;
				if(size+va>KERNEL_HEAP_MAX){
					release_spinlock(&kernel_lock);
					return NULL;
				}
				//uint32 va=(uint32)(KERNEL_HEAP_MAX-(PAGE_SIZE*(NUM_OF_KHEAP_PAGES-1)));
				unsigned int  numframes=(unsigned int )((size+PAGE_SIZE-1)/PAGE_SIZE);
				//gets the dir 10 bits +cr3 reg
				while(numframes>0){
					struct FrameInfo* ptr_frameinfo;
					//if allocation successful then map
					//allocate handles freeframelist inside
					int allocreturn=allocate_frame(&ptr_frameinfo);
					if(allocreturn!=0){
						release_spinlock(&kernel_lock);
						return NULL;
					}
					//means present in mem and accessable by user space i think its correct but nor sure
					//mapping sets values inside dir table and page table
						int mapreturn=map_frame(ptr_page_directory,ptr_frameinfo,va,PERM_WRITEABLE);
						if(mapreturn!=0){

							//i think i should free  frame
							//sa7abt frame mn allocate w haraga3o 3ashan msh ha3raf a map

							//free_frame(ptr_frameinfo);
								free_frame(ptr_frameinfo);
                            release_spinlock(&kernel_lock);
							return NULL;
						}
							va=va+(uint32)(PAGE_SIZE);
							numframes--;
				}
				release_spinlock(&kernel_lock);
				return (void*)(add);
			}
		}
		release_spinlock(&kernel_lock);
		return NULL;
}

void kfree(void* virtual_address)
{
	if(!holding_spinlock(&kernel_lock))
	acquire_spinlock(&kernel_lock);
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
		uint32 vas = (uint32)virtual_address;
//		uint32 vas2=vas >>12;
//	    uint32 size=saved_size[vas2];
//	    cprintf("yes %x %d",size);

//
		uint32 size=0;
		 int found = 0;
		 uint32 dd;
		 for (int i = 0; i < count; i++) {
			//	cprintf("yes %x %d",allocations[count].address,size);
			if (allocations[i].address == vas && allocations[i].is_free == 1) {
				allocations[i].is_free = 0;  // Mark as freed
				size = allocations[i].size;
				dd=allocations[i].address;
				//cprintf("yes %x %x %d",dd,vas,size);
				found = 1;
				break;
			}
		}

		if(found==0){
			release_spinlock(&kernel_lock);
			return;
		}

	     if(dd >= KH_limit+PAGE_SIZE){

	        // Free as page memory if within range and size >= 2KB
	        // Calculate the number of frames to free based on the size
	        unsigned int numframes = (unsigned int)((size + PAGE_SIZE - 1) / PAGE_SIZE);

	            while (numframes > 0) {
	            // Unmap and free each frame
	            	uint32 * ptr_page_table=NULL;
	            	struct FrameInfo * ptr_frame_info =get_frame_info(ptr_page_directory, vas, &ptr_page_table);
	   			    uint32 framno=to_frame_number(ptr_frame_info);
	   			    frame_map[framno]=0;

	                unmap_frame(ptr_page_directory, vas);
	            	numframes--;
	            	if(numframes!=0)
	            	vas += PAGE_SIZE;

	            }
	            release_spinlock(&kernel_lock);
	            			return;

	    }
	     //Determine if this is a block or page allocation based on size and address range
	    else  if ( dd <= KH_limit && dd>=KH_start) {
	        // Free as block memory if within range and size < 2KB
	    	uint32 * ptr_page_table2=NULL;
			get_page_table(ptr_page_directory,(uint32)vas,&ptr_page_table2);
			uint32 page_table_entry = ptr_page_table2[PTX((uint32)vas)];
//			uint32 pa=EXTRACT_ADDRESS(page_table_entry);
//			pa += ((uint32)vas & ~0xFFFFF000);
			uint32 pa=kheap_physical_address(vas);
		//	block_map[pa>>12]=0;
	        free_block((void*)vas);
	        release_spinlock(&kernel_lock);
	        			return;
	    }
	    else {
		// If address is outside of expected range, trigger a panic
				panic("kfree(): Invalid address or size for freeing memory.");
		}
	     release_spinlock(&kernel_lock);
	     			return;


}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	uint32 * ptr_page_table=NULL;
	get_page_table(ptr_page_directory,virtual_address,&ptr_page_table);
	if(ptr_page_table==NULL) //if no mapping
		return 0;
	uint32 page_table_entry = ptr_page_table[PTX(virtual_address)];
	if((page_table_entry & PERM_PRESENT)==0){
		return 0;
	}
	uint32 pa=EXTRACT_ADDRESS(page_table_entry);
	pa += (virtual_address & ~0xFFFFF000);
	//cprintf("the physical address is %x",pa);
	return pa;
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	 if(block_map[physical_address>>12]!=0){
		uint32 va=0;
		va=block_map[physical_address>>12];
		return((va & 0xFFFFF000) +  PGOFF(physical_address));

	}
	struct FrameInfo* ptr_frameinfo=to_frame_info(physical_address);
	uint32 frameno=to_frame_number(ptr_frameinfo);
	if(frame_map[frameno]!=0)
	return ( frame_map[frameno]+  PGOFF(physical_address) );

return 0;


//return the virtual address corresponding to given physical_address
//refer to the project presentation and documentation for details

//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}//=================================================================================//

// krealloc():
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size) {
    if (virtual_address == NULL) {
//        cprintf("krealloc: Virtual address is NULL, allocating new memory of size %d\n", new_size);
        return kmalloc(new_size);
    }
    if (new_size == 0) {
//        cprintf("krealloc: New size is 0, freeing memory at address %p\n", virtual_address);
        kfree(virtual_address);
        return NULL;
    }


    uint32 va = (uint32)virtual_address;
    uint32 old_size = 0;
    int found = 0;

    int k = -1;
    for (int i = 0; i < count; i++) {
        if (allocations[i].address == va && allocations[i].is_free == 1) {
            old_size = allocations[i].size;
            k = i;
            found = 1;
            break;
        }
    }

    if (!found) {
//        cprintf("krealloc: Virtual address %p not found in allocation list\n", virtual_address);
        return NULL;
    }

//    cprintf("\nkrealloc: BEFORE reallocation, old_size = %d, new_size = %d\n", old_size, new_size);

    if (new_size == old_size) {
//        cprintf("krealloc: Old size and new size are equal, returning same address %p\n", virtual_address);
        return virtual_address;
    }
//    if ((va + new_size) > KERNEL_HEAP_MAX) {
//            	                return NULL;
//            	            }
    unsigned int old_frames = (old_size + PAGE_SIZE - 1) / PAGE_SIZE;
    unsigned int new_frames = (new_size + PAGE_SIZE - 1) / PAGE_SIZE;

//    cprintf("krealloc: Old frames = %d, New frames = %d\n", old_frames, new_frames);

    // Handling for kernel heap block
    //blk
    if (va >= KH_start && va < KH_limit) {
//        cprintf("krealloc: Address within kernel heap, handling block allocation\n");
    	//blk to page
        if (new_size > DYN_ALLOC_MAX_BLOCK_SIZE) {

        	void *new_va = kmalloc(new_size);
        	if (new_va != NULL) {
        	    memcpy(new_va, virtual_address, old_size);
        	    kfree(virtual_address);
        	    allocations[k].address = (uint32)new_va;
        	    allocations[k].size = new_size;
        	}

        	return new_va;
        	//blk to page
        } else {
        	void *new_va = realloc_block_FF(virtual_address, new_size);
        	allocations[k].address = (uint32)new_va;
        				   allocations[k].size = new_size;
        				   allocations[k].is_free = 1;
            return new_va;
        }
    }

    // Handling for user page-based allocation
    //page
    else if (va >= KH_limit + PAGE_SIZE) {
//        cprintf("krealloc: Address within user space, handling page allocation\n");
    	//page to page
        if (new_size > DYN_ALLOC_MAX_BLOCK_SIZE) {

            if (new_size < old_size) {
                uint32 free_va = va + (new_frames * PAGE_SIZE);
                for (uint32 i = 0; i < (old_frames - new_frames); i++) {
                    uint32 *ptr_page_table = NULL;
                    struct FrameInfo *frame_info = get_frame_info(ptr_page_directory, free_va, &ptr_page_table);
                    if (frame_info) {
                        unmap_frame(ptr_page_directory, free_va);
                    }
                    free_va += PAGE_SIZE;
                }

                allocations[k].size = new_size;
                allocations[k].is_free = 1;
//                cprintf("krealloc: Successfully freed pages, returning address %p\n", virtual_address);
                return virtual_address;
            }

            if (new_size > old_size) {
            	 uint32 end_va = va + (old_frames * PAGE_SIZE);
            	            int sufficient_space = 1;

            	            for (uint32 i = 0; i < (new_frames - old_frames); i++) {
            	                uint32 *ptr_page_table = NULL;
            	                struct FrameInfo *frame_info = get_frame_info(ptr_page_directory, end_va, &ptr_page_table);
            	                if (frame_info != NULL) {
            	                    sufficient_space = 0;
            	                    break;
            	                }
            	                end_va += PAGE_SIZE;
            	            }
            	            if(!sufficient_space){
            	            	void *new_va = kmalloc(new_size);
            	            	if (new_va != NULL) {
            	            	    memcpy(new_va, virtual_address, old_size);
            	            	    kfree(virtual_address);
            	            	    allocations[k].address = (uint32)new_va;
            	            	    allocations[k].size = new_size;
            	            	}
            	            	return new_va;
            	            }
            	            else if (sufficient_space) {

            	                uint32 frame_va = va + (old_frames * PAGE_SIZE);
            	                for (uint32 i = 0; i < (new_frames - old_frames); i++) {
            	                    struct FrameInfo *frame_info;
            	                    int ret = allocate_frame(&frame_info);
            	                    if (ret == E_NO_MEM) {
            	                        return NULL;
            	                    }

            	                    ret = map_frame(ptr_page_directory, frame_info, frame_va,PERM_WRITEABLE);
            	                    if (ret == E_NO_MEM) {
            	                        free_frame(frame_info);
            	                        return NULL;
            	                    }

            	                    frame_va += PAGE_SIZE;
            	                }

            	                allocations[k].address = (uint32)virtual_address;
								allocations[k].size = new_size;
								allocations[k].is_free = 1;

            	                return virtual_address;
            	            }
            	        }
            	    }
        //page to blk
        else {
//            cprintf("krealloc: New size smaller than block size, allocating new memory\n");
            void *new_va = kmalloc(new_size);
            if (new_va != NULL) {
                memcpy(new_va, virtual_address, new_size);
                kfree(virtual_address);
            }
            return new_va;
        }
    }

    return NULL;
}
