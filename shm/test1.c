#include	"unpipc.h"

int
main(int argc, char **argv)
{
	unsigned long		fd, i;
	char	*ptr;
	size_t	filesize, mmapsize, pagesize;

	if (argc != 4)
		err_quit("usage: test1 <pathname> <filesize> <mmapsize>");

	filesize = atoi(argv[2]);
	mmapsize = atoi(argv[3]);

	/* open file: create or truncate; set file size */
	fd = Open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE);
	//更改文件大小的一种方法
	Lseek(fd, filesize-1, SEEK_SET);

	Write(fd, "", 1);

	ptr = Mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	Close(fd);

	/*
	Linux wgg-MacBookPro 6.2.0-39-generic #40~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Thu Nov 16 10:53:04 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
	A  file  is  mapped  in multiples of the page size.  
	For a file that is not a multiple of the page size, the remaining bytes in the partial
    page at the end of the mapping are zeroed when mapped, 
	and modifications to that region are not written out to the  file.   
	The  effect  of changing the size of the underlying file of a mapping on the pages 
	that correspond to added or removed regions of the file is unspecified.
	*/
	pagesize = Sysconf(_SC_PAGESIZE);
	printf("PAGESIZE = %ld\n", (long) pagesize);

	for (i = 0; i < max(filesize, mmapsize); i += pagesize) 
	{
		printf("ptr[%lu] = %d\n", i, ptr[i]);
		ptr[i] = 1;
		printf("ptr[%lu] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
		ptr[i + pagesize - 1] = 1;
	}

	printf("ptr[%lu] = %d\n", i, ptr[i]);
	// ptr[i] = 1;
	// printf("ptr[%lu] = %d\n", i, ptr[i]);
	exit(0);
}


/*
env: Darwin airdev 21.6.0 Darwin Kernel Version 21.6.0: Sat Jun 18 17:07:28 PDT 2022; root:xnu-8020.140.41~1/RELEASE_ARM64_T8110 arm64
(base) guigangwang@airdev shm % ./test1 foo 5000 5000
PAGESIZE = 16384
ptr[0] = 0
ptr[16383] = 0
zsh: segmentation fault  ./test1 foo 5000 5000
(base) guigangwang@airdev shm %   

env:Linux wgg-MacBookPro 6.2.0-39-generic #40~22.04.1-Ubuntu SMP PREEMPT_DYNAMIC Thu Nov 16 10:53:04 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
wgg@wgg-MacBookPro:~/unpv22e/shm$ ./test1 foo 5000 5000
PAGESIZE = 4096
ptr[0] = 0
ptr[4095] = 0
ptr[4096] = 0
ptr[8191] = 0
ptr[8192] = 80
wgg@wgg-MacBookPro:~/unpv22e/shm$ 


*/
