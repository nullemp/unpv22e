/* include init */
#include	"unpipc.h"
#include	"pthread_rwlock.h"


// pthread_mutex_t *mptr; / * pointer to the mutex in shared memory * /
// pthread_mutexattr_t *mattr; / * mutex attribute datatype */

// mptr = / * some value that points to shared memory * /
// Pthread_mutexattr_init(&mattr);
// #ifdef -POSIX-THREAD-PROCESS-SHARED
// 	Pthread_mutexattr_setpshared(&mattr, PTHREAD-PROCESS-SHARED); 
// #else 
// 	#error this implementation does not support -POSIX-THREAD-PROCESS-SHARED #endif
// Pthread-mutex-inithptr, &rnattr);


int
my_pthread_rwlock_init(my_pthread_rwlock_t *rw, my_pthread_rwlockattr_t *attr)
{
	int		result;

	if (attr != NULL)
		return(EINVAL);		/* not supported */

	if ( (result = pthread_mutex_init(&rw->rw_mutex, NULL)) != 0)
		goto err1;
	if ( (result = pthread_cond_init(&rw->rw_condreaders, NULL)) != 0)
		goto err2;
	if ( (result = pthread_cond_init(&rw->rw_condwriters, NULL)) != 0)
		goto err3;
	rw->rw_nwaitreaders = 0;
	rw->rw_nwaitwriters = 0;
	rw->rw_refcount = 0;
	rw->rw_magic = RW_MAGIC;

	return(0);

err3:
	pthread_cond_destroy(&rw->rw_condreaders);
err2:
	pthread_mutex_destroy(&rw->rw_mutex);
err1:
	return(result);
}
/* end init */

void
Pthread_rwlock_init(my_pthread_rwlock_t *rw, my_pthread_rwlockattr_t *attr)
{
	int		n;

	if ( (n = my_pthread_rwlock_init(rw, attr)) == 0)
		return;
	errno = n;
	err_sys("pthread_rwlock_init error");
}
