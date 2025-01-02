/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include <kern/proc/user_environment.h>
#include "kheap.h"
#include "memory_manager.h"
#include <inc/queue.h>

//extern void inctst();

/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va on the given page_directory
//	If the page table at any destination page in the range is not exist, it should create it
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, cut-paste the number of pages and return 0
//	ALL 12 permission bits of the destination should be TYPICAL to those of the source
//	The given addresses may be not aligned on 4 KB
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//[PROJECT] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	panic("cut_paste_pages() is not implemented yet...!!");
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va on the given page_directory
//	Ranges DO NOT overlapped.
//	If ANY of the destination pages exists with READ ONLY permission, deny the entire process and return -1.
//	If the page table at any destination page in the range is not exist, it should create it
//	If ANY of the destination pages doesn't exist, create it with the following permissions then copy.
//	Otherwise, just copy!
//		1. WRITABLE permission
//		2. USER/SUPERVISOR permission must be SAME as the one of the source
//	The given range(s) may be not aligned on 4 KB
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//[PROJECT] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the //panic and write your code
	panic("copy_paste_chunk() is not implemented yet...!!");
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va on the given page_directory
//	Ranges DO NOT overlapped.
//	It should set the permissions of the second range by the given perms
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, share the required range and return 0
//	If the page table at any destination page in the range is not exist, it should create it
//	The given range(s) may be not aligned on 4 KB
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//[PROJECT] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the //panic and write your code
	panic("share_chunk() is not implemented yet...!!");
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate the given virtual range [<va>, <va> + <size>) in the given address space  <page_directory> with the given permissions <perms>.
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, allocate the required range and return 0
//	If the page table at any destination page in the range is not exist, it should create it
//	Allocation should be aligned on page boundary. However, the given range may be not aligned.
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//[PROJECT] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the //panic and write your code
	panic("allocate_chunk() is not implemented yet...!!");
}

//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//[PROJECT] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	panic("calculate_allocated_space() is not implemented yet...!!");
}

//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
//This function should calculate the required number of pages for allocating and mapping the given range [start va, start va + size) (either for the pages themselves or for the page tables required for mapping)
//	Pages and/or page tables that are already exist in the range SHOULD NOT be counted.
//	The given range(s) may be not aligned on 4 KB
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//[PROJECT] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	panic("calculate_required_frames() is not implemented yet...!!");
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
/* DYNAMIC ALLOCATOR SYSTEM CALLS */
//=====================================
void* sys_sbrk(int numOfPages)
       {
       /* numOfPages > 0: move the segment break of the current user program to increase the size of its heap
        * by the given number of pages. You should allocate NOTHING,
        * and returns the address of the previous break (i.e. the beginning of newly mapped memory).
        * numOfPages = 0: just return the current position of the segment break
        *
        * NOTES:
        * 1) As in real OS, allocate pages lazily. While sbrk moves the segment break, pages are not allocated
        * until the user program actually tries to access data in its heap (i.e. will be allocated via the fault handler).
        * 2) Allocating additional pages for a process’ heap will fail if, for example, the free frames are exhausted
        * or the break exceed the limit of the dynamic allocator. If sys_sbrk fails, the net effect should
        * be that sys_sbrk returns (void*) -1 and that the segment break and the process heap are unaffected.
        * You might have to undo any operations you have done so far in this case.
        */

       //TODO: [PROJECT'24.MS2 - #11] [3] USER HEAP - sys_sbrk
       /*====================================*/
       /*Remove this line before start coding*/

       /*====================================*/
       		struct Env* e = get_cpu_proc();
       		uint32 oldbreak = e->UH_brk;
       		if (numOfPages == 0)
       		 {
       			 return (void*)e->UH_brk;
       		 }

       		uint32 expand_size = numOfPages * PAGE_SIZE;
       		uint32 new_size = oldbreak + expand_size;

       		if (new_size > e->UH_limit) {
       			 return (void*)-1;
       		 }

       		  uint32 start_address = e->UH_brk;
       		  e->UH_brk=new_size;
       		cprintf("\nsbrk in Uheap\n");
       		  allocate_user_mem(e, oldbreak,expand_size);
       	return(void*) oldbreak;

}

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
    unsigned int num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    uint32 va = (uint32)virtual_address;

    for (uint32 addr = virtual_address; addr < virtual_address + size; addr += PAGE_SIZE)
    {
        uint32* ptr_page_table = NULL;


        int ret = get_page_table(e->env_page_directory, addr, &ptr_page_table);
        if (ret == TABLE_NOT_EXIST)
        {

            create_page_table(e->env_page_directory, addr);
        }


        ret = get_page_table(e->env_page_directory, addr, &ptr_page_table);
        if (ret == TABLE_NOT_EXIST)
        {

            panic("Page table creation failed!");
        }

        ptr_page_table[PTX(addr)] |= MARK_BIT;
    }

}



//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	/*====================================*/
	/*Remove this line before start coding*/
//	inctst();
//	return;
	/*====================================*/

	//TODO: [PROJECT'24.MS2 - #15] [3] USER HEAP [KERNEL SIDE] - free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");

			/****  PAGE TABLE UMARK  ****/

			uint32 va = (uint32)virtual_address;
			if(va>=e->UH_limit+PAGE_SIZE){
				unsigned int  numframes=(unsigned int )((size+PAGE_SIZE-1)/PAGE_SIZE);
							uint32 *ptr_page_table = NULL ;

				while(numframes--){
					uint32 * ptr_page_table=NULL;
					struct FrameInfo * ptr_frame_info =get_frame_info(e->env_page_directory, va, &ptr_page_table);
					unmap_frame(e->env_page_directory,va);
					ptr_page_table[PTX(va)] &= ~MARK_BIT;
	//                    / PAGE FILE  /
						pf_remove_env_page(e,va);
	//                    /  WS /
						env_page_ws_invalidate(e,va);



				va+=PAGE_SIZE;
		}


				__placement_after_free(e);


			}



	//TODO: [PROJECT'24.MS2 - BONUS#3] [3] USER HEAP [KERNEL SIDE] - O(1) free_user_mem
}

void __placement_after_free(struct Env* e){
		struct WorkingSetElement *wse ,*target_wse = NULL;
			LIST_FOREACH(wse, &(e->page_WS_list))
			{
				if (e->page_last_WS_element > wse){
						target_wse=wse;
						//env_page_ws_invalidate(e,wse->virtual_address);
						LIST_REMOVE(&(e->page_WS_list), wse);
//						struct FrameInfo *frame;
//						int ret = allocate_frame(&frame);
//						if(ret!=0)
//							 cprintf("free_user_mem: Failed to allocate VA: %x\n", target_wse->virtual_address);
//						else{
//						map_frame(e->env_page_directory, frame, (uint32)target_wse,PERM_USER| PERM_WRITEABLE| PERM_PRESENT);
//					}
						LIST_INSERT_TAIL(&(e->page_WS_list), target_wse);
				}

				if (e->page_last_WS_element == wse)
						break;

			}

//			cprintf("plc after freeee\n");

}
//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//[PROJECT] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//
