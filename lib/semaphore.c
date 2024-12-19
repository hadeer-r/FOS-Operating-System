// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value) {
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_semaphore is not implemented yet");
	uint32 sz = sizeof(struct semaphore);
	void*va = smalloc(semaphoreName, sz, 1); // what if i can not allocate???
	if (va == NULL)
		panic("no memory to create");
	struct semaphore sem;
	sem.semdata->count = value;
	sem.semdata->lock = 0;
	sem.semdata->name= semaphoreName;
	intialize_sem_q(sem); // sys_call
	return sem;
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName) {
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");

	void* sem = sget(ownerEnvID, semaphoreName); // what if null
	if (sem == NULL)
		panic("there is no such semaphore");
	return (struct semaphore*)sem;

}

void __acquire(struct semaphore s) {
	while (xchg(&(s.semdata->lock), 1) != 0);
}
void __release(struct semaphore s) {
	s.semdata->lock = 0;
}
void wait_semaphore(struct semaphore sem) {
	__acquire(sem);
	sem.semdata->count--;

	if (sem.semdata->count < 0) {
		//struct Env* env = get_cpu_proc();
		make_blocked(sem);
	}
	__release(sem);
}

void signal_semaphore(struct semaphore sem) {
	__acquire(sem);
	sem.semdata->count++;

	if (sem.semdata->count <= 0) {

		make_ready(sem);
	}
	__release(sem);

}

int semaphore_count(struct semaphore sem) {
	return sem.semdata->count;
}
