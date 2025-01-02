// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	struct semaphore sem;
	sem.semdata=NULL;
	if(semaphoreName==NULL||value<0){

		return sem;
	}

	//void *va=NULL;
	sem.semdata=(struct __semdata *)smalloc(semaphoreName,sizeof(struct semaphore),1);
	//int res=sys_createSharedObject(semaphoreName,sizeof(struct __semdata),1,&va);
	if(sem.semdata==NULL){

		return sem;
	}
	//sem.semdata = (struct __semdata*)vaa;
	sem.semdata->count=value;
	sem.semdata->lock=0;
	strcpy(sem.semdata->name,semaphoreName);
	//init_queue why not working?
//	if(&(sem.semdata->queue) != NULL)
//	{
//		LIST_INIT(&sem.semdata->queue);
//
//	}

	//sys_init(&(sem.semdata->queue));
	return sem;
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...

	struct semaphore sem;
	 sem.semdata = NULL;


	 void* semaphore_addr = sget(ownerEnvID, semaphoreName);


	 if (semaphore_addr == NULL) {
			 return sem;  // Return the semaphore with semdata still NULL
		 }


		// if found, hn cast ll type bta3 ll semaphores
	   // struct semaphore sem;
		sem.semdata = (struct __semdata*)semaphore_addr;

		return sem;
}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...

	  if (sem.semdata == NULL) {
	        return;
	    }
	  while (xchg(&sem.semdata->lock, 1) != 0);
	sem.semdata->count--;
	if(sem.semdata->count<0){
		sys_enqueue(&(sem.semdata->queue),sem.semdata);

		return;
	}

	sem.semdata->lock=0;


}
void signal_semaphore(struct semaphore sem)
	{
		//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
		//COMMENT THE FOLLOWING LINE BEFORE START CODING
		//panic("signal_semaphore is not implemented yet");
		//Your Code is Here...
			// Acquire the lock using atomic exchange
			while (xchg(&sem.semdata->lock, 1) != 0) ;
			// Increment the semaphore count
			sem.semdata->count++;

	// Check if there are waiting processes
			if (sem.semdata->count <= 0)
			{
	   //  Remove a process from the semaphore queue
		struct Env *waitingProcess = LIST_FIRST(&sem.semdata->queue);
		if (waitingProcess != NULL)
		{
			//LIST_REMOVE(&sem.semdata->queue, waitingProcess);
			// Add the process to the ready list
		   //enqueue(&(ProcessQueues.env_ready_queues[0]), cur_env);
			//sys_enlist(&(sem.semdata->list));
			sys_list(waitingProcess,&(sem.semdata->queue),sem.semdata);

		}
			}
		// Release the lock
		sem.semdata->lock = 0;

}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}

