#include	"unpipc.h"

int main(int argc, char **argv)
{
	int		semid;

	if (argc != 2)
		err_quit("usage: semrmid <pathname>");

	semid = Semget(Ftok(argv[1], 0), 0, 0);
	/*
	 IPC_RMID
            Immediately remove the semaphore set, 
			awakening all processes blocked in semop(2) calls on the set (with an error return  and  errno
            set  to  EIDRM).   
			The  effective user ID of the calling process must match the creator or owner of the semaphore set, or the caller
            must be privileged.  
			The argument semnum is ignored.
	*/
	// 注意权限问题
	Semctl(semid, 0, IPC_RMID);

	exit(0);
}
