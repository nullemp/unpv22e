/*
demonstrate that a shared memory object can be memory mapped starting 
at different addresses in different processes.
*/
#include	"unpipc.h"
#include <stdio.h>
#include <sys/mman.h>
int
main(int argc, char **argv)
{
	int		fd1, fd2, *ptr1, *ptr2;
	pid_t	childpid;
	struct stat	stat;

	if (argc != 2)
		err_quit("usage: test3 <name>");

	// shm_unlink(Px_ipc_name(argv[1]));
	shm_unlink(argv[1]);
	fd1 = Shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	Ftruncate(fd1, sizeof(int));

	fd2 = Open("./test.data", O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	Lseek(fd2, 4095, SEEK_SET);
	Write(fd2, "", 1);
	Fstat(fd2, &stat);

	if ( (childpid = Fork()) == 0) {
			/* child */
		ptr2 = Mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
		ptr1 = Mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
					MAP_SHARED, fd1, 0);
		printf("child: shm ptr = %p, motd ptr = %p\n", ptr1, ptr2);

		sleep(5);
		printf("shared memory integer = %d\n", *ptr1);
		exit(0);
	}
		/* parent: mmap in reverse order from child */
	ptr1 = Mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	ptr2 = Mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0);
	printf("parent: shm ptr = %p, motd ptr = %p\n", ptr1, ptr2);
	*ptr1 = 777;
	Waitpid(childpid, NULL, 0);

	exit(0);
}
