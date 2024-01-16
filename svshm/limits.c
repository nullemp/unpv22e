#include	"unpipc.h"

#define	MAX_NIDS	8192

/*
	shmmax	max #bytes for a shared memory segment
	shmmnb 	min #bytes for a shared memory segment
	shmmni 	max #shared memory identifiers,systemwide
	shmseg	max #shared memory segments attached per process
*/
int main(int argc, char **argv)
{
	int		i, j, shmid[MAX_NIDS];
	void	*addr[MAX_NIDS];
	unsigned long	size;

	/* see how many identifiers we can "open" */
	for (i = 0; i <= MAX_NIDS; i++) {
		shmid[i] = shmget(IPC_PRIVATE, 1024, SVSHM_MODE | IPC_CREAT);
		if (shmid[i] == -1) {
			printf("SHMMNI: %d identifiers open at once\n", i);
			break;
		}
	}
	for (j = 0; j < i; j++)
		Shmctl(shmid[j], IPC_RMID, NULL);
	
	// shmseg > shmmni
	/* now see how many we can "attach" */
	for (j = 0; j < i ; j++) {
		shmid[j] = Shmget(IPC_PRIVATE, 1024, SVSHM_MODE | IPC_CREAT);
		addr[j] = shmat(shmid[j], NULL, 0);
		if (addr[j] == (void *) -1) {
			printf("%d shared memory segments attached at once\n", i);
			Shmctl(shmid[j], IPC_RMID, NULL);	/* the one that failed */
			break;
		}
	}

	for (i = 0; i < j; i++) {
		Shmdt(addr[i]);
		Shmctl(shmid[i], IPC_RMID, NULL);
	}
	

		/* see how small a shared memory segment we can create */
	for (size = 1; ; size++) {
		shmid[0] = shmget(IPC_PRIVATE, size, SVSHM_MODE | IPC_CREAT);
		if (shmid[0] != -1) {		/* stop on first success */
			printf("minimum size of shared memory segment = %lu\n", size);
			Shmctl(shmid[0], IPC_RMID, NULL);
			break;
		}
	}

	/* see how large a shared memory segment we can create */
	for (size = 65536; ; size += 4096) {
		shmid[0] = shmget(IPC_PRIVATE, size, SVSHM_MODE | IPC_CREAT);
		if (shmid[0] == -1) {		/* stop on first failure */
			printf("maximum size of shared memory segment = %lu\n", size-4096);
			break;
		}
		Shmctl(shmid[0], IPC_RMID, NULL);
	}

	exit(0);
}
