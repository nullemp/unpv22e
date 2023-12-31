/* include main */
#include	"unpipc.h"

// We do not start the consumer thread until all the producers are done.

#define	MAX_N_ITEMS 		1000000   	// 池子有多少个槽
#define	MAX_N_THREADS			100		// 线程数量

int		nitems;			/* read-only by producer and consumer */
struct {
  pthread_mutex_t	mutex;
  int	buff[MAX_N_ITEMS];
  int	nput; // 键
  int	nval; // 值
} shared = { PTHREAD_MUTEX_INITIALIZER };

void	*produce(void *), *consume(void *);

int
main(int argc, char **argv)
{
	int			i, nthreads, count[MAX_N_ITEMS];
	// 多个生产者，1个消费者
	pthread_t	tid_produce[MAX_N_THREADS], tid_consume;

	if (argc != 3)
		err_quit("usage: prodcons1 <#items> <#threads>");
	nitems = min(atoi(argv[1]), MAX_N_ITEMS);
	nthreads = min(atoi(argv[2]), MAX_N_THREADS);

	Set_concurrency(nthreads);
	for (i = 0; i < nthreads; i++) {
		count[i] = 0;
		Pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}

	for (i = 0; i < nthreads; i++) {
		Pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);	
	}

	Pthread_create(&tid_consume, NULL, consume, NULL);
	Pthread_join(tid_consume, NULL);

	exit(0);
}
/* end main */

/* include produce */
void *
produce(void *arg)
{

	for ( ; ; ) {
		if (shared.nput >= nitems) {
			return(NULL);		/* array is full, we're done */
		}
		shared.buff[shared.nput] = shared.nval;
		shared.nput++;
		shared.nval++;
		*((int *) arg) += 1;
	}
}

void *
consume(void *arg)
{
	int		i;

	for (i = 0; i < nitems; i++) {
		if (shared.buff[i] != i)
			printf("buff[%d] = %d\n", i, shared.buff[i]);
	}
	return(NULL);
}
/* end produce */
