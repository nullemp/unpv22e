#include	"unpipc.h"
#include <unistd.h>

#define	SEM_NAME	"/mysem"

int main(int argc, char **argv)
{
	int		fd, i, nloop, zero = 0;
	int		*ptr;
	sem_t	*mutex;

	if (argc != 3)
		err_quit("usage: incr2 <pathname> <#loops>");
	
	nloop = atoi(argv[2]);

	/* open file, initialize to 0, map into memory */
	fd = Open(argv[1], O_RDWR | O_CREAT, FILE_MODE);
	Write(fd, &zero, sizeof(int));

	/*
		After map returns success, the fd argument can be closed. 
		This has no effect on the mapping that was established by mmap.
	*/
	ptr = Mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	Unlink(argv[1]);
	Close(fd);

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
