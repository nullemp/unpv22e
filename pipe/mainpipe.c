#include	"unpipc.h"

void	client(int, int), server(int, int);

int
main(int argc, char **argv)
{
	int		pipe1[2], pipe2[2];
	pid_t	childpid;

	Pipe(pipe1);	/* create two pipes */
	Pipe(pipe2);

	if ( (childpid = Fork()) == 0) {		/* child */
		Close(pipe1[1]); // close write
		Close(pipe2[0]); // close read
		
		// 服务器从pipe1[0]读取要打开的路径名，打开文件，读取文件内容，写入pipe2[1]
		server(pipe1[0], pipe2[1]);
		exit(0);
	}
		/* parent */
	Close(pipe1[0]);
	Close(pipe2[1]);

	// 客户端段从stdin读取要查询的文件名，将该文件名写入到pipe1[1]中，然后从pipe2[0]中读取server端传过来的文件内容
	client(pipe2[0], pipe1[1]);
	// 到这里，server端先结束
	Waitpid(childpid, NULL, 0);		/* wait for child to terminate */
	exit(0);
}
