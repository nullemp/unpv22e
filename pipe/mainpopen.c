#include	"unpipc.h"

// int
// main(int argc, char **argv)
// {
// 	size_t	n;
// 	char	buff[MAXLINE], command[MAXLINE];
// 	FILE	*fp;

// 		/* read pathname */
// 	Fgets(buff, MAXLINE, stdin);
// 	n = strlen(buff);		/* fgets() guarantees null byte at end */
// 	if (buff[n-1] == '\n')
// 		n--;				/* delete newline from fgets() */

// 	snprintf(command, sizeof(command), "cat %s", buff);
// 	fp = Popen(command, "r");

// 		/* copy from pipe to standard output */
// 	while (Fgets(buff, MAXLINE, fp) != NULL)
// 		Fputs(buff, stdout);

// 	Pclose(fp);
// 	exit(0);
// }


// #include <stdio.h>
/* Also realize that the call to popen succeeds in such a case,
    but fgets just returns an end-of-file the first time it is called.
*/
// 下面这段程序并没有验证成功，因为fgets会堵塞
int main() {
    FILE *fp;
    char buffer[1024];

    fp = popen("sleep 10;echo Hello, World!", "r");

    if (fp == NULL) {
        perror("popen");
        return -1;
    }

    // 读取子进程的输出
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("Output from command: %s", buffer);
    } else {
        printf("fgets encountered EOF or an error.\n");
    }

    pclose(fp);

    return 0;
}

