// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value) {
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_semaphore is not implemented yet");
	uint32 sz = sizeof(struct __semdata);
	struct semaphore sem;
	sem.semdata = (struct __semdata*) smalloc(semaphoreName, sz, 1); // what if i can not allocate???
	if (sem.semdata == NULL) {
		return sem;
	}
	sem.semdata->count = value;
	sem.semdata->lock = 0;
	strcpy(sem.semdata->name, semaphoreName);
	//cprintf("before intializatioooooooooooooon");
	sys_intialize_sem_q(sem.semdata); // sys_call
	//cprintf("syscall11111111111111111");
	return sem;
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName) {
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");

	struct semaphore sem;
	sem.semdata = (struct __semdata*) sget(ownerEnvID, semaphoreName); // what if null
	return sem;
}

void __acquire(struct __semdata* data) {
	while (xchg(&(data->lock), 1) != 0);
}
void __release(struct __semdata* data) {
	data->lock = 0;
}
void wait_semaphore(struct semaphore sem) {
	__acquire(sem.semdata);
	sem.semdata->count--;

	if (sem.semdata->count < 0) {
		sys_make_blocked(sem.semdata);
		//cprintf("syscall2222222222222222222222");
	}
	__release(sem.semdata);
}

void signal_semaphore(struct semaphore sem) {
	__acquire(sem.semdata);
	sem.semdata->count++;

	if (sem.semdata->count <= 0) {

		sys_make_ready(sem.semdata);
		//cprintf("syscall33333333333333333333333");
	}
	__release(sem.semdata);

}

int semaphore_count(struct semaphore sem) {
	return sem.semdata->count;
}
