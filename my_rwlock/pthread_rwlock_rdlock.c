/* include rdlock */
#include	"unpipc.h"
#include	"pthread_rwlock.h"
#include <pthread.h>

static void rwlock_cancelrdwait(void *arg)
{
	my_pthread_rwlock_t *rw = (my_pthread_rwlock_t *)arg;
	rw->rw_nwaitreaders--;
	pthread_mutex_unlock(&rw->rw_mutex);
}

int my_pthread_rwlock_rdlock(my_pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return(result);

		/* give preference to waiting writers */
		// 写者优先
	while (rw->rw_refcount < 0 || rw->rw_nwaitwriters > 0) {
		rw->rw_nwaitreaders++;

		// Answer to Q2
		#if 1
			pthread_cleanup_push(rwlock_cancelrdwait, (void *)rw);
		#endif
		result = pthread_cond_wait(&rw->rw_condreaders, &rw->rw_mutex);
		#if 1
			pthread_cleanup_pop(0);
		#endif
		/* Q1: spurious awake? */ 
		/*
		Q2:
		A problem exists in this function: 
		if the calling thread blocks in the call to pthread-cond-wait and 
		the thread is then canceled, 
		the thread terminates while it holds the mutex lock, and 
		the counter rw-nwaitreaders is wrong. 
		The same problem exists in our implementation of pthread_rwlock_wrlock in Figure 8.6. 
		We correct these problems in Section 8.5.	
		*/
		rw->rw_nwaitreaders--;
		if (result != 0)
			break;
	}
	if (result == 0)
		rw->rw_refcount++;		/* another reader has a read lock */

	pthread_mutex_unlock(&rw->rw_mutex);
	return (result);
}


/* end rdlock */

void Pthread_rwlock_rdlock(my_pthread_rwlock_t *rw)
{
	int		n;

	if ( (n = my_pthread_rwlock_rdlock(rw)) == 0)
		return;
	errno = n;
	err_sys("pthread_rwlock_rdlock error");
}
