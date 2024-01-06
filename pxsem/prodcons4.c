/* include globals */
#include	"unpipc.h"

#define	NBUFF	 	 10
#define	MAX_N_THREADS	100
#define DEBUG

int		nitems, nproducers, nconsumers;		/* read-only */

struct {							/* data shared by producers and consumers */
  int	buff[NBUFF];
  int	nput;						/* item index: 0, 1, 2, ... */
  int	nputval;					/* value to store in buff[] */
  int	nget;						/* item index: 0, 1, 2, ... */
  int	ngetval;					/* value fetched from buff[] */
  sem_t	mutex, nempty, nstored;		/* semaphores, not pointers */
} shared;

void	*produce(void *), *consume(void *);
/* end globals */

/* include main */
int
main(int argc, char **argv)
{
	int		i, prodcount[MAX_N_THREADS], conscount[MAX_N_THREADS];
	pthread_t	tid_produce[MAX_N_THREADS], tid_consume[MAX_N_THREADS];
	int val;
	if (argc != 4)
		err_quit("usage: prodcons4 <#items> <#producers> <#consumers>");
	nitems = atoi(argv[1]);
	nproducers = min(atoi(argv[2]), MAX_N_THREADS);
	nconsumers = min(atoi(argv[3]), MAX_N_THREADS);

		/* initialize three semaphores */
	Sem_init(&shared.mutex, 0, 1);
	Sem_init(&shared.nempty, 0, NBUFF);
	Sem_init(&shared.nstored, 0, 0);

#ifdef DEBUG
	
	sem_getvalue(&shared.mutex, &val);
	printf("shared.mutex value = %d\n", val);
	sem_getvalue(&shared.nempty, &val);
	printf("shared.nempty value = %d\n", val);
	sem_getvalue(&shared.nstored, &val);
	printf("shared.nstored value = %d\n", val);
#endif 

		/* create all producers and all consumers */
	Set_concurrency(nproducers + nconsumers);
	for (i = 0; i < nproducers; i++) {
		prodcount[i] = 0;
		Pthread_create(&tid_produce[i], NULL, produce, &prodcount[i]);
	}
	for (i = 0; i < nconsumers; i++) {
		conscount[i] = 0;
		Pthread_create(&tid_consume[i], NULL, consume, &conscount[i]);
	}

		/* wait for all producers and all consumers */
	for (i = 0; i < nproducers; i++) {
		Pthread_join(tid_produce[i], NULL);
		printf("producer count[%d] = %d\n", i, prodcount[i]);	
	}
	for (i = 0; i < nconsumers; i++) {
		Pthread_join(tid_consume[i], NULL);
		printf("consumer count[%d] = %d\n", i, conscount[i]);	
	}

#ifdef DEBUG
	
	sem_getvalue(&shared.mutex, &val);
	printf("shared.mutex value = %d\n", val);
	sem_getvalue(&shared.nempty, &val);
	printf("shared.nempty value = %d\n", val);
	sem_getvalue(&shared.nstored, &val);
	printf("shared.nstored value = %d\n", val);
#endif 


	Sem_destroy(&shared.mutex);
	Sem_destroy(&shared.nempty);
	Sem_destroy(&shared.nstored);
	exit(0);
}
/* end main */

/* include produce */
void *produce(void *arg)
{
	for ( ; ; ) {
		Sem_wait(&shared.nempty);	/* wait for at least 1 empty slot */
		Sem_wait(&shared.mutex);

		if (shared.nput >= nitems) {
			Sem_post(&shared.nstored);	/* let consumers terminate */
			Sem_post(&shared.nempty);
			Sem_post(&shared.mutex);
			return(NULL);			/* all done */
		}

		shared.buff[shared.nput % NBUFF] = shared.nputval;
		shared.nput++;
		shared.nputval++;

		Sem_post(&shared.mutex);
		Sem_post(&shared.nstored);	/* 1 more stored item */
		*((int *) arg) += 1;
	}
}
/* end produce */

/* include consume */
void *consume(void *arg)
{
	int		i;

	for ( ; ; ) {
		Sem_wait(&shared.nstored);	/* wait for at least 1 stored item */
		Sem_wait(&shared.mutex);

		if (shared.nget >= nitems) {
			Sem_post(&shared.nstored);
			Sem_post(&shared.mutex);
			return(NULL);			/* all done */
		}

		i = shared.nget % NBUFF;
		if (shared.buff[i] != shared.ngetval)
			printf("error: buff[%d] = %d\n", i, shared.buff[i]);
		shared.nget++;
		shared.ngetval++;

		Sem_post(&shared.mutex);
		Sem_post(&shared.nempty);	/* 1 more empty slot */
		*((int *) arg) += 1;
	}
}
/* end consume */
