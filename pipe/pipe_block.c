#include "unpipc.h"

int main (int argc, char **argv)
{
    pid_t childPid;
    int readFifo;
    #define FIFO_NAME "/tmp/block"
    if ((mkfifo(FIFO_NAME, FILE_MODE) < 0) && (errno != EEXIST))
		err_sys("can't create %s", FIFO_NAME);
    
    if ( (childPid = Fork()) == 0) {		/* child */
        readFifo = Open(FIFO_NAME, O_RDONLY, 0);
        
		exit(0);
	}
}