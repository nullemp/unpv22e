/* include wrlock */
#include	"unpipc.h"
#include	"pthread_rwlock.h"
#include <pthread.h>

static void rwlock_cancelwrwait(void *arg)
{
	my_pthread_rwlock_t *rw = (my_pthread_rwlock_t *)arg;
	rw->rw_nwaitwriters--;
	pthread_mutex_unlock(&rw->rw_mutex);
}

int my_pthread_rwlock_wrlock(my_pthread_rwlock_t *rw)
{
	int		result;

	if (rw->rw_magic != RW_MAGIC)
		return(EINVAL);

	if ( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return(result);

	while (rw->rw_refcount != 0) {
		rw->rw_nwaitwriters++;

		#if 1
			pthread_cleanup_push(rwlock_cancelwrwait, (void *)rw);
		#endif 
		result = pthread_cond_wait(&rw->rw_condwriters, &rw->rw_mutex);
		#if 1
			pthread_cleanup_pop(0);
		#endif
		rw->rw_nwaitwriters--;
		if (result != 0)
			break;
	}
	if (result == 0)
		rw->rw_refcount = -1;

	pthread_mutex_unlock(&rw->rw_mutex);
	return(result);
}
/* end wrlock */

void
Pthread_rwlock_wrlock(my_pthread_rwlock_t *rw)
{
	int		n;

	if ( (n = my_pthread_rwlock_wrlock(rw)) == 0)
		return;
	errno = n;
	err_sys("pthread_rwlock_wrlock error");
}
