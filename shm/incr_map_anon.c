#include	"unpipc.h"

#define	SEM_NAME	"/mysem"

/* include diff */
int
main(int argc, char **argv)
{
	int		i, nloop;
	int		*ptr;
	sem_t	*mutex;

	if (argc != 2)
		err_quit("usage: incr_map_anon <#loops>");
	nloop = atoi(argv[1]);

	/* map into memory */
	ptr = Mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
			   MAP_SHARED | MAP_ANON, -1, 0);
	/* end diff */

	/* create, initialize, and unlink semaphore */
	mutex = Sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1);
	Sem_unlink(SEM_NAME);

	setbuf(stdout, NULL);	/* stdout is unbuffered */
	if (Fork() == 0) {		/* child */
		for (i = 0; i < nloop; i++) {
			Sem_wait(mutex);
			printf("child: %d\n", (*ptr)++);
			Sem_post(mutex);
		}
		exit(0);
	}

		/* parent */
	for (i = 0; i < nloop; i++) {
		Sem_wait(mutex);
		printf("parent: %d\n", (*ptr)++);
		Sem_post(mutex);
	}
	exit(0);
}
