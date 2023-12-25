#include	"unpipc.h"

#define	FIFO1	"/tmp/fifo.1"
#define	FIFO2	"/tmp/fifo.2"

void	client(int, int), server(int, int);

int
main(int argc, char **argv)
{
	int		readfd, writefd;
	pid_t	childpid;

		/* create two FIFOs; OK if they already exist */
	if ((mkfifo(FIFO1, FILE_MODE) < 0) && (errno != EEXIST))
		err_sys("can't create %s", FIFO1);
	if ((mkfifo(FIFO2, FILE_MODE) < 0) && (errno != EEXIST)) {
		unlink(FIFO1);
		err_sys("can't create %s", FIFO2);
	}
	
	/*
		ubuntu 12.04 LTS
		wgg@wgg-MacBookPro:/tmp$ ll -F | grep fifo
		prw-r--r--  1 wgg      wgg            0 12月 25 11:11 fifo.1|
		prw-r--r--  1 wgg      wgg            0 12月 25 11:11 fifo.2|
	*/

	if ( (childpid = Fork()) == 0) {		/* child */
		readfd = Open(FIFO1, O_RDONLY, 0);
		writefd = Open(FIFO2, O_WRONLY, 0);

		server(readfd, writefd);
		exit(0);
	}
		/* parent */
		/*: if we swap the order of the two calls to open in the parent, the program does not work */
	writefd = Open(FIFO1, O_WRONLY, 0);
	readfd = Open(FIFO2, O_RDONLY, 0);

	client(readfd, writefd);

	Waitpid(childpid, NULL, 0);		/* wait for child to terminate */

	Close(readfd);
	Close(writefd);

	Unlink(FIFO1);
	Unlink(FIFO2);
	exit(0);
}
