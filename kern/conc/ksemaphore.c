// Kernel-level Semaphore

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "ksemaphore.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_ksemaphore(struct ksemaphore *ksem, int value, char *name)
{
	//[PROJECT'24.MS3]
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("init_ksemaphore is not implemented yet");
	//Your Code is Here...
//	if(ksem==NULL||value<0||name==NULL){
//		return;
//	}
	init_channel(&ksem->chan,name);


	ksem->count=value;
	strcpy(ksem->name,name);
	init_queue(&(ksem->chan.queue));
	init_spinlock(&ksem->lk,name);

}

void wait_ksemaphore(struct ksemaphore *ksem)
{
	//[PROJECT'24.MS3]
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_ksemaphore is not implemented yet");
	//Your Code is Here...
	 acquire_spinlock(&ksem->lk);
	struct Env* cur_envv=get_cpu_proc();
	if(cur_envv==NULL){
		return ;
	}
	ksem->count--;
	if(ksem->count<0){
		sleep(&ksem->chan,&ksem->lk);
	}

		release_spinlock(&ksem->lk);
	//else{
//
	//}
}

void signal_ksemaphore(struct ksemaphore *ksem)
{
	//[PROJECT'24.MS3]
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_ksemaphore is not implemented yet");
	//Your Code is Here...
	//cprintf("signal ksem");
   // acquire_spinlock(&ksem->lk);
    ksem->count++;
   // if (ksem->count <= 0) {
		struct Env *waitingProcess = LIST_FIRST(&ksem->chan.queue);
		if (waitingProcess != NULL) {
		   wakeup_one(&(ksem->chan));
		}
  //  }
   // release_spinlock(&ksem->lk);
}



