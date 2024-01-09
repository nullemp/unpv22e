#include	"unpipc.h"

int
main(int argc, char **argv)
{
	int		semid, nsems, i;
	struct semid_ds	seminfo;
	unsigned short	*ptr;
	
	union semun	arg;

	if (argc != 2)
		err_quit("usage: semgetvalues <pathname>");

	/* first get the number of semaphores in the set */
	/*
       IPC_STAT
            Copy  information from the kernel data structure 
			associated with semid into the semid_ds structure pointed to by arg.buf.  
			The argument semnum is ignored.  
			The calling process must have read permission on the semaphore set.
	*/
	semid = Semget(Ftok(argv[1], 0), 0, 0);
	arg.buf = &seminfo;
	Semctl(semid, 0, IPC_STAT, arg);
	nsems = arg.buf->sem_nsems;

	/* allocate memory to hold all the values in the set */
	ptr = Calloc(nsems, sizeof(unsigned short));
	arg.array = ptr;

	/* fetch the values and print */
	/*
		GETALL Return semval (i.e., the current value) for all semaphores of the set into arg.array.  
		The argument semnum is ignored.  
		The  calling process must have read permission on the semaphore set.
	*/
	Semctl(semid, 0, GETALL, arg);
	for (i = 0; i < nsems; i++)
		printf("semval[%d] = %d\n", i, ptr[i]);

	exit(0);
}
