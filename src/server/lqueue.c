#include "queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


/* the lqueue representation is hidden from users of the module */
typedef struct {
	void* node;
	void* prev;
} Node;

typedef struct lqueue_t{
   queue_t *q;
   pthread_mutex_t mutex;
} lqueue_t;

/* create an empty lqueue */
lqueue_t* lqopen(void){
  lqueue_t *lq = (lqueue_t*) malloc(sizeof(lqueue_t));
  pthread_mutex_t lock;
  queue_t *myqueue;
  myqueue = qopen();
  pthread_mutex_init(&lock, NULL);


  lq->q = myqueue;
  lq->mutex = lock;

  return lq;
}

/* deallocate a lqueue, frees everything in it */
void lqclose(lqueue_t *lqp){
  pthread_mutex_lock(&lqp->mutex);
  qclose(lqp->q);
  pthread_mutex_unlock(&lqp->mutex);
}

/* put element at end of lqueue */
int lqput(lqueue_t *lqp, void *elementp){
  int n;
  pthread_mutex_lock(&lqp->mutex);
  n = qput(lqp->q, elementp);
  pthread_mutex_unlock(&lqp->mutex);
  return n;
}

/* get first element from lqueue */
void* lqget(lqueue_t *lqp){
  Node *temp;
  pthread_mutex_lock(&lqp->mutex);
  temp = qget(lqp->q);
  pthread_mutex_unlock(&lqp->mutex);
  return temp;

}

/* apply a void function to every element of a lqueue */
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)){
  pthread_mutex_lock(&lqp->mutex);
  qapply(lqp->q, fn);
  pthread_mutex_unlock(&lqp->mutex);
}

/* search a lqueue using a supplied boolean function, returns an element */
void* lqsearch(lqueue_t *lqp, int (*searchfn)(void* elementp,const void* keyp), const void* skeyp){
    Node *temp;
    pthread_mutex_lock(&lqp->mutex);
    temp = qsearch(lqp->q, searchfn, skeyp);
    pthread_mutex_unlock(&lqp->mutex);
    return temp;
}

/* search a lqueue using a supplied boolean function, removes and
 * returns the element
 */
void* lqremove(lqueue_t *lqp,
	      int (*searchfn)(void* elementp,const void* keyp),
	      const void* skeyp){
      Node *temp;

      pthread_mutex_lock(&lqp->mutex);
      temp = qremove(lqp->q, searchfn, skeyp);
      pthread_mutex_unlock(&lqp->mutex);
      return temp;
    }

/* concatenatenates elements of q2 into q1, q2 is dealocated upon completion */
void lqconcat(lqueue_t *q1p, lqueue_t *q2p){
    pthread_mutex_lock(&q1p->mutex);
    qconcat(q1p->q, q2p->q);
    pthread_mutex_unlock(&q1p->mutex);

}
