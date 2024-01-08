#include	"unpipc.h"

int
main(int argc, char **argv)
{
	printf("SEM_VALUE_MAX = %ld\n", Sysconf(_SC_SEM_VALUE_MAX));
	// printf("SEM_NSEMS_MAX = %ld\n", Sysconf(_SC_SEM_NSEMS_MAX));
	exit(0);
}
