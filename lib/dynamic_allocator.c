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
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ; // sizeof(int32) va -> h
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
uint32* endPtr;
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{

	// lw el initSizeOfAllocatedSpace odd 3melo even
    if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++;
    // lw b zero a5rog men el func
    if (initSizeOfAllocatedSpace == 0)
        return;
    			// a3mel el init en hya init b 1
    			is_initialized = 1;

    // a3mel list yalaaaaaa
    LIST_INIT(&freeBlocksList);

			   // a3mel el address beta3 el terminators (beg & end)
			   // el beg 7tba fel bdaya (dastart) w hatba allocated b 1 w size 0
				uint32* dallocBegg = (uint32*)daStart;

				// el beg 7tba fel nehaya (daEnd) w hatba allocated b 1 w size 0
				 dallocEndd = (uint32*)(daStart + initSizeOfAllocatedSpace - sizeof(int));
				//endPtr =  (uint32*)dallocEndd;
    //  allocated b 1 w size 0
    *dallocBegg = 0|1;
    // allocated b 1 w size 0
    *dallocEndd = 0|1;

			// bs kda laaaaaaa3
			// tamam el header bta3 el block hayro7 l awl el list
			uint32* SblkHeaderS = (uint32*)(daStart + sizeof(int));
			// tb el footer ..?
			// hayba b3d el header w size w nshel el 2*4 bytes bto3 el H w F
			uint32* SblkFooterS = (uint32*)(daStart + initSizeOfAllocatedSpace - 2 * sizeof(int));
			// tb el hyshel eh ..?
			//el init mtshal mno el H w F
			*SblkHeaderS = initSizeOfAllocatedSpace - 2 * sizeof(int);
			//el init mtshal mno el H w F
			*SblkFooterS = initSizeOfAllocatedSpace - 2 * sizeof(int);

    // b3deeeen han3mel el awel block baa2a
    // ezay ..?
    // han3mel el block yesawe el bedaya(dastart) w el H w F
    struct BlockElement* firstFreeBlock = (struct BlockElement*)(daStart + 2 * sizeof(int));
    // b3deeeen handeeef el blk to list.
    LIST_INSERT_HEAD(&freeBlocksList, firstFreeBlock);
    //DONEEEEEEEEEE :)
}//==================================

// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
	//Your Code is Here...
	// Ensure the total size is at least 16

	// Define the struct inside the function
	// Ensure the total size is at least 16 bytes (minimum block size)
	    if (totalSize < 16) {
	        totalSize = 16;
	    }

	    // Ensure the total size is even
	    if (totalSize % 2 != 0) {
	        totalSize++;  // Adjust to make it even
	    }

	    // Calc the position lihm
	        uint32* header = (uint32*)((char*)va - sizeof(uint32)); // Header before the data
	        uint32* footer = (uint32*)((char*)va + totalSize - 2* sizeof(uint32));

	        //hnstakhdm bitwise to get if allocated or not
	        uint32 data = totalSize | (isAllocated ? 1 : 0);

	        // both pointers hyshawro 3la nfs l data
	        *header = data;
	        *footer = data;

}
//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
//	==================================================================================
//	DON'T CHANGE THESE LINES==========================================================
//	==================================================================================
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
	//Your Code is Here...
	if( size<=0){
		return NULL;
	}


	size=size+2*sizeof(uint32);

	//get first element
	struct BlockElement* firstElementList=LIST_FIRST(&freeBlocksList);
	//	FOR FIRST FIT
	while(firstElementList!=NULL){
		//get size of the first element
		//return size onlyy
		uint32 blocksize=get_block_size((void *)firstElementList);
        //cprintf("bb %d", blocksize);

		 if(blocksize>=size){
			 //kda byshawer 3ala awel element b3d el header
			uint32* payload=(uint32*)(firstElementList);
			uint32 remaining=blocksize-size;
			uint32* Header=(uint32*)((char*)payload-sizeof(uint32));

			//make the second part due to splitting the allocate=0(head and footer) and BITWISE AND sets lst to 0
			if (remaining>=(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE +2*sizeof(uint32))){
				set_block_data((void*)payload, size, 1);
				uint32  *Header2= (uint32 *)((char*)Header+size);
				set_block_data((void*)((char*)Header2+sizeof(uint32)), remaining, 0);

				LIST_INSERT_AFTER(&freeBlocksList,(struct BlockElement *)((char*)Header+sizeof(uint32*)),(struct BlockElement *)((char*)Header2+sizeof(uint32*)));
				LIST_REMOVE(&freeBlocksList,(struct BlockElement *)((char*)Header+sizeof(uint32*)));
			}
			else if(remaining<(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE+2*sizeof(uint32))){
				set_block_data(payload, blocksize, 1);
			   LIST_REMOVE(&freeBlocksList,firstElementList);
		   }

          return (void *)(payload);

		}
		firstElementList=LIST_NEXT(firstElementList);
	}
		//1-a7arak end(break) f el kheap one page(4kb) then =>add free bel(LIST_INSERT_AFTER) b3d el LIST_LAST()
		//block= header w footer 4+2kb(load size )+4
		//and make them allocated
		//conditiom fel kheap : if before end is free then merge them together(bard hane3mel step 1 de)

	// size + h + f
	/*
	 * daEnd = start  + 2*pages - sizeof
	 * size < page_SIZE (4KB)
	 * blk  1. prev empty -> merge -> alloc -> split & adding freelist
	 * 		2. \\   full  -> alloc -> split & adding freelist
	 * */
	void* sbrkk = sbrk((size, PAGE_SIZE)/PAGE_SIZE);
		if (sbrkk == (void *)-1) {
		    return NULL;
		} else {
			// 4KB ->
			uint32 remaining_size  ;
			set_block_data(sbrkk,PAGE_SIZE, 1);


		    uint32* newLocation = (uint32*)((char*)dallocEndd + PAGE_SIZE);
		   *newLocation = 0|1; // 0x1 => size 0 & allocated
			free_block(sbrkk);

		   dallocEndd = newLocation;
			return alloc_block_FF(size-2*sizeof(uint32));
//		    struct BlockElement *prev_free_block = NULL;
//		    uint32* PrevFooter = (uint32*)((char*)Header -  sizeof(uint32)) ;
//		    uint32 prevBlockSize = (*PrevFooter) & ~(0x1);// va
//		    uint32* PrevHeader = (uint32*)((char*)PrevFooter - prevBlockSize +  sizeof(uint32)) ;
//
//		    uint32 total ;
//
//		    //merge==>if rem<16 allocate all,
//		    //if rem>16 split, allocate size only
//
//		    if((*PrevFooter & 1) == 0){ // Free merge -> split
//
//		    	total=prevBlockSize+PAGE_SIZE;
//				 remaining_size = total - size;
//				 if(remaining_size<(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE + 2*sizeof(uint32))){
//		    	Header = PrevHeader;
//		    	total = prevBlockSize +  PAGE_SIZE; //
//				*Header = total | 1;
//				 footer = (uint32*)((char*)Header + total - sizeof(uint32));
//				*footer = total | 1;
//				PrevFooter=footer;
//				LIST_REMOVE(&freeBlocksList,(struct BlockElement*)(char*)Header+sizeof(uint32));
//				 }
//				 else{
//					 Header = PrevHeader;
//					total = prevBlockSize +  PAGE_SIZE; //
//					*Header = size | 1;
//					footer = (uint32*)((char*)Header + size - sizeof(uint32));
//					*footer = size | 1;
//					PrevFooter=footer;
//					LIST_REMOVE(&freeBlocksList,(struct BlockElement*)(char*)Header+sizeof(uint32));
//
//						//SPLIT
//						uint32* ptrnxtheader = (uint32*)((char*)footer + sizeof(uint32) );
//						*ptrnxtheader = remaining_size | 0;
//						footer = (uint32*)((char*)ptrnxtheader + remaining_size - sizeof(uint32));
//						*footer = remaining_size | 0;
//
//						 LIST_INSERT_TAIL(&freeBlocksList,  (struct BlockElement*)((char*)ptrnxtheader + sizeof(uint32)));
//					 }
//
//		}
//
//		    else{
//		    	remaining_size = PAGE_SIZE - size;
//				 if(remaining_size<(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE + 2*sizeof(uint32))){
//		    	*Header = PAGE_SIZE | 1;
//				 footer = (uint32*)((char*)Header + PAGE_SIZE - sizeof(uint32));
//				*footer = PAGE_SIZE | 1;
//				 }
//
//				 else{
//					*Header = size | 1;
//					 footer = (uint32*)((char*)Header + size - sizeof(uint32));
//					*footer = size | 1;
//
//				uint32* ptrnxtheader = (uint32*)((char*)footer + sizeof(uint32) );
//				*ptrnxtheader = remaining_size | 0;
//				footer = (uint32*)((char*)ptrnxtheader + remaining_size - sizeof(uint32));
//				*footer = remaining_size | 0;
//
//				 LIST_INSERT_TAIL(&freeBlocksList,  (struct BlockElement*)((char*)ptrnxtheader + sizeof(uint32)));
//				 }
//
//		    }
	//===================================merge========================

//		    return (void*)((char*)Header + sizeof(uint32));
		}
		return NULL;

	}// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size){
	//	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//	//panic("alloc_block_BF is not implemented yet");
	//	//Your Code is Here...
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
	if(size<=0){
		return NULL;
	}
		size=size+2*sizeof(uint32);

		//get first element
		struct BlockElement* firstElementList=LIST_FIRST(&freeBlocksList);
		struct BlockElement* load=NULL;

		uint32 smallest=0;
		uint32 largest=0xFFFFFFFF;

		//	FOR BEST FIT
		while(firstElementList!=NULL){
		uint32 blocksize=get_block_size((void *)firstElementList);


        //cprintf("bb %d", blocksize);

		 if(blocksize>=size){
			smallest=blocksize-size;
			if(largest>smallest){
				largest=smallest;
				load=firstElementList;
//				cprintf("largest %x ",largest);
//				cprintf("smallest %x",smallest);
			}
		}


		//get the next elemenet in list
		firstElementList=LIST_NEXT(firstElementList);
	}
		//kda byshawer 3ala awel element b3d el header
		if(load!=NULL){
		uint32* payload=(uint32*)(load);
		uint32 actual_payload=(uint32)payload;
		uint32* Header=(uint32*)((char*)payload-sizeof(uint32));
		uint32 wholeblocksize=get_block_size((void*)load);
		uint32 remaining=wholeblocksize-size;

		//------------------------------------------------------------------------
		//splittingggg
		//------------------------------------------------------------------------


		//make the second part due to splitting the allocate=0(head and footer) and BITWISE AND sets lst to 0
		if (remaining>(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE +2*sizeof(uint32))){
		*Header=size|1;
		//cprintf("headff  %d", *((uint32*)(payload)-1));
		uint32 *Footer=(uint32 *)((char*)payload + size -2*sizeof(uint32));
		*Footer=size|1;
		uint32  *Header2= (uint32 *)((char*)payload+size-sizeof(uint32));
		*Header2=(remaining);
		//get_blocksize(Header2)===>header+footer+payload
		uint32 *Footer2 = (uint32 *)((char*)Header2 +remaining-sizeof(uint32));
		//footer should also be resized i think
		*Footer2=((remaining));

		LIST_INSERT_AFTER(&freeBlocksList,load,(struct BlockElement *)((char*)Header2+sizeof(uint32*)));
		LIST_REMOVE(&freeBlocksList,load);
		}
		else if(remaining<=(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE+2*sizeof(uint32))){
		*Header=(wholeblocksize|1);
		//*payload=blocksize-2*sizeof(uint32);
		//the char for the beggining of the address then add the payload and the size of the head
		uint32 *Footer=(uint32 *)((char *)payload+wholeblocksize-2*sizeof(uint32));
		*Footer=(wholeblocksize|1);
	   // cprintf("Allocated entire block of size %d at %d.\n", blocksize, Header);
	   LIST_REMOVE(&freeBlocksList,(struct BlockElement *)((char*)Header+sizeof(uint32*)));
	   }

		return (void *)(payload);

		}
		else{
			void* sbrkk = sbrk(1);
			if (sbrkk == (void *)-1) {
			    return NULL;
			} else {
				// 4KB ->
				uint32 remaining_size  ;
			    uint32* Header = (uint32*)((char*)sbrkk - sizeof(uint32)); //
			    *Header = PAGE_SIZE | 0;
			   // uint32* va = (uint32*)((char*)Header + sizeof(uint32));
			    uint32* footer = (uint32*)((char*)Header + PAGE_SIZE - sizeof(uint32));

			    *footer = PAGE_SIZE | 0;
			    // BOLCK DONE

			    // PREV BLOCK
			    uint32* PrevFooter = (uint32*)((char*)Header -  sizeof(uint32)) ;
			    uint32 prevBlockSize = (*PrevFooter) & ~(0x1);// va
			    uint32* PrevHeader = (uint32*)((char*)PrevFooter - prevBlockSize +  sizeof(uint32)) ;
		//	    PREV BLOCK CREATED

			    /* MERGING */

			    uint32 total ;

			    if((*PrevFooter & 1) == 0){ // Free merge -> split
		//	    	Header = PrevHeader;
		//	    	*Header = (*PrevFooter) & ~(0x1) + size | 0;

			    	Header = PrevHeader;
			    	total = prevBlockSize +  PAGE_SIZE; //
					*Header = total | 0;
					  footer = (uint32*)((char*)Header + total - sizeof(uint32));
					*footer = total | 0;
					PrevFooter = footer;

		//			sbrkk = (uint32*)((char*)PrevHeader +  sizeof(uint32)) ;

		//			sbrkk = (struct BlockElement*)((char*)Header + sizeof(uint32));
					 remaining_size = total - size;
					 cprintf("\nMERGING WITH THE PREV..\n");
					 	    	cprintf ("\n");
					/* END */
			    }

			    else{
			    	remaining_size = PAGE_SIZE - size;
			    }



		//			PrevFooter = (uint32*)((char*)footer -remaining_size);
		//			Header =  (uint32*)((char*)PrevFooter + sizeof(uint32));
		//			*PrevHeader = size | 1;
		//			*PrevFooter = size | 1;





			    		    if (remaining_size>(uint32)(DYN_ALLOC_MIN_BLOCK_SIZE + 2*sizeof(uint32))) {


			    		    	// prev FOR SPLITTINGGGG
			    		    		    uint32* ptrprevfooter = (uint32*)((char*)Header + size - sizeof(uint32));
			    		    		    *ptrprevfooter = size | 1;
			    		    		    *Header = size | 1;

			    		    		    // next FOR SPLITTINGGGG
			    		    		    uint32* ptrnxtheader = (uint32*)((char*)ptrprevfooter + sizeof(uint32) );
			    		    		    *ptrnxtheader = remaining_size | 0;
			    		    		    footer = (uint32*)((char*)ptrnxtheader + remaining_size - sizeof(uint32));
			    		    		    *footer = remaining_size | 0;




		//	    		        struct BlockElement* free_block = (struct BlockElement*)((char*)PrevFooter + 2*sizeof(uint32));
		//
		//
		//	    		        uint32* free_header = (uint32*)((char*)PrevFooter + sizeof(uint32));
		//	    		        *free_header = remaining_size|0;
		//
		//	    		        uint32* free_footer = (uint32*)((char*)free_header + remaining_size - sizeof(uint32));
		//	    		        *free_footer = remaining_size|0;


			    		        LIST_INSERT_TAIL(&freeBlocksList,  (struct BlockElement*)((char*)ptrnxtheader + sizeof(uint32)));

		//	    		        cprintf("\nSMALLER THAN PAGE_SIZE..\n");


			    		    }


			    uint32* newLocation = (uint32*)((char*)dallocEndd + PAGE_SIZE);


			       *newLocation = 0|1; // 0x1 => size 0 & allocated

			       dallocEndd = newLocation;
		//

			    return (void*)((char*)Header + sizeof(uint32));
			}
		}
}//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_block is not implemented yet");
	//Your Code is Here...

	// lw el add b null out
	if (va == NULL) {
	        return;
	    }

    uint32 _currBlockSize_ = get_block_size(va);
    uint32* _currHeader_ = (uint32*)((char*)va - sizeof(uint32));
    uint32* _currFooter_ = (uint32*)((char*)va + _currBlockSize_ - 2*sizeof(uint32));
    struct BlockElement* _currBlock_ = (struct BlockElement*)va;

    uint32 _prev = 0, next_ = 0;

    //lw mesh free khleeeh freeee easy :)
    // han5ear el alloc
    // nshof el prev and nxt
							// 1. lw el prev freee merge
							// 2. lw el next freee merge
							// 3. lw el prev & nxt freee merge
    					    // 4. else just free
    if (!is_free_block(va)) {
        // e3mel el alloc b zero

    	set_block_data(va, _currBlockSize_, 0);


        struct BlockElement* nextBlk_ = NULL;

        struct BlockElement* _prevBlk = NULL;

        // Check if we can merge with the previous block
        uint32* _prevFooter = (uint32*)((char*)_currHeader_ - sizeof(uint32));
        if ((*_prevFooter & 1) == 0) {  // Previous block is free
            uint32 _prevBlockSize = (*_prevFooter) & ~1;
            uint32* _prevHeader = (uint32*)((char*)_prevFooter - _prevBlockSize + sizeof(uint32));
             //prevBlk=(struct BlockElement*)((char*)prevHeader+sizeof(uint32*));
             // n8ayer el addresses bto3 el prev w curr
            _currBlockSize_ += _prevBlockSize;
            set_block_data((char*)_prevHeader + sizeof(uint32), _currBlockSize_, 0);
            _currHeader_ = _prevHeader;      // harak el header le el prevblock ;)
            // harak el curr le ba3d el H b 4bytes
            _currBlock_ = (struct BlockElement*)((char*)_currHeader_ + sizeof(uint32));
            // assign b 1 3lashan a3raf ana el 2bly free of not 1 b free
            _prev = 1;

        }

        // Check if we can merge with the next block
        uint32* nextHeader = (uint32*)((char*)_currFooter_ + sizeof(uint32));
        if ((*nextHeader & 1) == 0) {  // b3de b free
            uint32 nextBlockSize = (*nextHeader) & ~1; // alloc b 0
            // harak
            nextBlk_=(struct BlockElement*)((char*)nextHeader+sizeof(uint32*));
            uint32* nextFooter = (uint32*)((char*)nextHeader + nextBlockSize - sizeof(uint32));
            _currBlockSize_ += nextBlockSize;
            set_block_data((char*)_currHeader_ + sizeof(uint32), _currBlockSize_, 0);

            _currFooter_ = nextFooter;      // harek el footer le el nxtblock ;)
            // assign b 1 3lashan a3raf ana el 2bly free of not 1 b free
            next_ = 1;
        }
        //merge with next ba2a
							//1. lw el nxt bool b 1 w prev bool b 0 ==> def fe el list w ahzef
							//2. lw el nxt bool b 0 w prev bool b 1 ==> nothing
        					//3. lw el nxt bool b 1 w prev bool b 1 ==> ahzef
        if(next_ == 1 && _prev == 0){
        	LIST_INSERT_AFTER(&freeBlocksList,nextBlk_,_currBlock_);
        	LIST_REMOVE(&freeBlocksList,nextBlk_);
        	 return;
        }
        else if(next_ == 0 && _prev == 1){
//			LIST_INSERT_AFTER(&freeBlocksList,prevBlk,currBlock);
//			LIST_REMOVE(&freeBlocksList,prevBlk);
			 return;
		}

        else  if(next_ == 1 && _prev == 1){
//        	LIST_INSERT_AFTER(&freeBlocksList,prevBlk,currBlock);
//		   LIST_REMOVE(&freeBlocksList,prevBlk);
		   LIST_REMOVE(&freeBlocksList,nextBlk_);

		  return;
		 }

    }


    // No merging happened, proceed to insert the block into the free list
    								// loop 3la el list le find afdal makaannn ;)
    												//1. lw a2l men currheader add a3mel abl w a3mel el insert b 1
    												//2. equal nothing w o2l en hwa inserted
    												//3. 8er kda a3mel f al a5er
    uint32 insertedtOLISsst = 0;
    struct BlockElement* blk = LIST_FIRST(&freeBlocksList);
    while (blk != NULL) {
        uint32* headerBlk = (uint32*)((char*)blk - sizeof(uint32));

         if ((uint32*)headerBlk > (uint32*)_currHeader_) {
            // Insert the block before the current block
            LIST_INSERT_BEFORE(&freeBlocksList, blk, _currBlock_);

            insertedtOLISsst = 1;
            break;
        }
        blk = LIST_NEXT(blk);
    }
    // If the block wasn't inserted in the middle, add it to the tail
    if (insertedtOLISsst == 0) {
        LIST_INSERT_TAIL(&freeBlocksList, _currBlock_);
    }
}//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
//	TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
//	COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("realloc_block_FF is not implemented yet");
//	Your Code is Here...
	if (va == NULL && new_size == 0) {
			   return NULL;
		}
	if (va == NULL) {
		        return alloc_block_FF(new_size);
		    }
	if (new_size == 0) {
		free_block(va);
		return NULL;
	}
	if(new_size>=DYN_ALLOC_MAX_SIZE){

	}
	 uint32 current_size = get_block_size(va);
     /*stored data must be the same !!! */
	//case 1 new size = same current size      //do nothing
	 if (new_size == current_size) {
	         return (void *)va;
	     }
	 //dr amelha fel allocate ff
	 if (new_size % 2 != 0) {
	         new_size++;
	 }
	 if (new_size < DYN_ALLOC_MIN_BLOCK_SIZE) {
		 new_size = DYN_ALLOC_MIN_BLOCK_SIZE;
	 }

	 //case 2 new size = more than current one  //1) enough space: resize in same place
	 	                                           //2) not enough: alloc_FF (BY CUTTING IT)
	 	                                           //no free blocks from 2, call SBRK
	     //===============================================================================

         //next free ==> new > current ==> [next + current >= new] 1- take the required 1-split  rem>16
         // 																			2-NO SPLIT rem<16
         //							   ==> [next + current < new] the realloccc

         //			 ==> new < current ==>  merge with next

         //next not free ==> new > current ==> realloc
         // 			 ==> new <= current	1- split  rem >= 16
         //				 					2- internl frag rem<16
         //

	 	 //DIFFERENT PLACE==> REMOVE THE OLD AND PUT IT ANOTHER PLACE

	     //===============================================================================

	     uint32* Header = (uint32*)((char*)va - sizeof(uint32));
	     uint32 *Footer = (uint32 *)((char*)Header + new_size + sizeof(uint32));

	         struct BlockElement* next_block = (struct BlockElement*)((char*)va + current_size);

	         uint32 next_block_size = get_block_size((void*)next_block);
	         if (is_free_block(next_block)) {

	        	 if (next_block_size + current_size >= (new_size+ 2 * sizeof(uint32))) {
						 uint32 remaining_space = (current_size + next_block_size) - (new_size+ 2 * sizeof(uint32));
						 //split
					 if (remaining_space >= (uint32)(DYN_ALLOC_MIN_BLOCK_SIZE + 2 * sizeof(uint32))) {
						 set_block_data((void*)(char*)Header+sizeof(uint32),new_size+ 2 * sizeof(uint32),1);

						 uint32 *Header2 = (uint32 *)((char*)va + new_size + sizeof(uint32));
						 set_block_data((void*)(char*)Header2+sizeof(uint32),remaining_space,0);

						 LIST_INSERT_AFTER(&freeBlocksList, next_block, (struct BlockElement *)((char*)Header2 + sizeof(uint32)));
						 LIST_REMOVE(&freeBlocksList, next_block);
					 }
						 //no split
						 else if (remaining_space < (uint32)(DYN_ALLOC_MIN_BLOCK_SIZE + 2 * sizeof(uint32))) {
							 set_block_data((void*)(char*)Header+sizeof(uint32),(new_size+ 2 * sizeof(uint32)),1);

							 LIST_REMOVE(&freeBlocksList, next_block);
						 }
			    }


	        	 else {
					 void *va2 = alloc_block_FF(new_size);
					 if (va2==NULL) {
					 return va;
					 }
						 memcpy(va2, va, current_size);
						 free_block(va);
						 return va2;
				 }

	         }
		 else if(!is_free_block(next_block)){

	  if (new_size+2*sizeof(uint32) <= current_size) {
		 uint32 remaining = current_size -  (new_size + 2 * sizeof(uint32));
		 	 if(remaining >= (uint32)(DYN_ALLOC_MIN_BLOCK_SIZE + 2 * sizeof(uint32))){

					 *Header=(new_size+2*sizeof(uint32))|1;
					 *Footer=(new_size+2*sizeof(uint32))|1;

					 uint32* freeHeader = (uint32 *)((char*)Footer + sizeof(uint32));
					 uint32* freeFooter =(uint32 *)((char*)freeHeader+ remaining -sizeof(uint32));

					 *freeFooter =(remaining|0);
					 *freeHeader = (remaining|0);

					struct BlockElement *newBlkAddr = (struct BlockElement *)((char*)freeHeader+sizeof(uint32));

					free_block(newBlkAddr);

		 	 }
		 	 else {
		 		Footer = (uint32 *)((char*)Header + current_size - sizeof(uint32));

		 		*Header = (current_size|1);
		 		*Footer = (current_size|1);

		 	 }


	 }
	  else {
		 void *va2 = alloc_block_FF(new_size);
		 if (va2==NULL) {
		 return va;
		 }

			 memcpy(va2, va, current_size);
			 free_block(va);
			 return va2;
	 }

	}




   return (void*) va;

}
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
