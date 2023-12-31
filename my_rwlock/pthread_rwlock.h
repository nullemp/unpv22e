/* include rwlockh */
#ifndef	__pthread_rwlock_h
#define	__pthread_rwlock_h

// #include <bits/pthreadtypes.h>

typedef struct {
  pthread_mutex_t	rw_mutex;		/* basic lock on this struct */
  pthread_cond_t	rw_condreaders;	/* for reader threads waiting */
  pthread_cond_t	rw_condwriters;	/* for writer threads waiting */
  int				rw_magic;		/* for error checking */
  int				rw_nwaitreaders;/* the number waiting */
  int				rw_nwaitwriters;/* the number waiting */
  int				rw_refcount; /* -1 indicates a write lock (and only one of these can exist at a time), 0 indicates the lock is available, and a value greater than 0 means that many read locks are currently held. */
} my_pthread_rwlock_t;

#define	RW_MAGIC	0x19283746

	/* following must have same order as elements in struct above */
#define	MY_PTHREAD_RWLOCK_INITIALIZER	{ PTHREAD_MUTEX_INITIALIZER, \
			PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, \
			RW_MAGIC, 0, 0, 0 }

typedef	int		my_pthread_rwlockattr_t;	/* dummy; not supported */

	/* function prototypes */
int		my_pthread_rwlock_destroy   (my_pthread_rwlock_t *);
int		my_pthread_rwlock_init      (my_pthread_rwlock_t *, my_pthread_rwlockattr_t *);
int		my_pthread_rwlock_rdlock    (my_pthread_rwlock_t *);
int		my_pthread_rwlock_tryrdlock (my_pthread_rwlock_t *);
int		my_pthread_rwlock_trywrlock (my_pthread_rwlock_t *);
int		my_pthread_rwlock_unlock    (my_pthread_rwlock_t *);
int		my_pthread_rwlock_wrlock    (my_pthread_rwlock_t *);

/* and our wrapper functions */
void	Pthread_rwlock_destroy    (my_pthread_rwlock_t *);
void	Pthread_rwlock_init       (my_pthread_rwlock_t *, my_pthread_rwlockattr_t *);
void	Pthread_rwlock_rdlock     (my_pthread_rwlock_t *);
int		Pthread_rwlock_tryrdlock  (my_pthread_rwlock_t *);
int		Pthread_rwlock_trywrlock  (my_pthread_rwlock_t *);
void	Pthread_rwlock_unlock     (my_pthread_rwlock_t *);
void	Pthread_rwlock_wrlock     (my_pthread_rwlock_t *);


#endif	//__pthread_rwlock_h
/* end rwlockh */
