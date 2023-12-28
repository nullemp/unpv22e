#include	"unpipc.h"

void
client(int readfd, int writefd)
{
	size_t	len;
	ssize_t	n;
	char	buff[MAXLINE];

		/* read pathname , block */
	Fgets(buff, MAXLINE, stdin);
	len = strlen(buff);		/* fgets() guarantees null byte at end */
	if (buff[len-1] == '\n')
		len--;				/* delete newline from fgets() */

		/* write pathname to IPC channel */
	Write(writefd, buff, len);
	
	#if 0
	// test SIGPIPE
	struct sigaction action;
	action.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &action , NULL); 

	sleep(3);
	Write(writefd, buff, len);
	// 执行不到这, Fatal error
	//sigaction(SIGPIPE, &action, NULL);
	#endif

		/* read from IPC, write to standard output */
	while ( (n = Read(readfd, buff, MAXLINE)) > 0)
		Write(STDOUT_FILENO, buff, n);
}
