/*=============================================================================
|   Title: lqueue.h
|
|       Author:  Grace Miller
|     Language:  C
|   To Compile:  Run the Makefile
|
|    Class:  CS 63 Programming Parallel Systems
|    Date:  10/15/2016
|
+-----------------------------------------------------------------------------
|
|  Description:  implements a generic locked queue that is singly linked with a head
|  and a tail
|  items are inserted into the tail of the queue, and popped from the head of the queue
|  The queue also contains a mutex that is locked whenever a function is performed
|  on the queue, and unlocked once that function is complete
|
|  each node item has a next pointer, next points backwards to the next item to be popped,
|  the tail's next value is NULL
|
*===========================================================================*/

#define TRUE 1
#define FALSE 0


/* the queue representation is hidden from users of the module */
typedef void lqueue_t;

/*
 * Function:  lqopen()
 * --------------------
 * a dummy function for opening a queue; returns NULL
 *
 * paramaters: null
 *
 *  returns: queue_t*, an open queue
 */
 queue_t* lqopen(void);

/*
 * Function:  lqclose()
 * --------------------
 * deallocate a queue, frees everything in it
 *
 * paramaters:
 *  queue_t *qp: queue to close
 *
 *  returns: NULL
 */
 void lqclose(queue_t *qp);

/*
 * Function:  lqput()
 * --------------------
 * put element at end of queue, queue elements insert into the tail of the queue
 *
 * paramaters:
 *  queue_t *qp: queue to insert into
 *  void *elementp: element to insert in
 *
 *  returns: 0 if successful, -1 if not successful
 */
 int lqput(queue_t *qp, void *elementp);

/*
 * Function:  lqget()
 * --------------------
 * get first element from queue, queue items are taken from the head of the queue
 *
 * paramaters:
 *  queue_t *qp: queue to get from
 *
 *  returns: void*, item from the queue (value inside the node)
 */
 void* lqget(queue_t *qp);

/*
 * Function:  lqapply()
 * --------------------
 *  apply a void function to every element of a queue
 *
 * paramaters:
 *  queue_t *qp: queue of items to apply the function on
 *  void (*fn)(void* elementp): a function to apply to each node
 *				which takes in the data inside each node, and returns void*
 *
 *  returns: NULL
 */
 void lqapply(queue_t *qp, void (*fn)(void* elementp));

/*
 * Function:  lqsearch()
 * --------------------
 *  search a queue using a supplied boolean function to compare elements,
 *  returns an element if it is found
 *
 * paramaters:
 *  queue_t *qp: queue of items to apply the function on
 *  void (*fn)(void* elementp,const void* keyp): a function which takes in and
 *        compares two nodes and returns 1 if they are the same and 0 if they
 * 				are not the same, const void* keyp is the node to find
 *  const void* skeyp: a data value to search the queue for
 *
 *  returns: the void* item if the value if found, NULL if not found
 */
 void* lqsearch(queue_t *qp,
	      int (*searchfn)(void* elementp,const void* keyp),
	      const void* skeyp);

/*
 * Function:  lqremove()
 * --------------------
 *  search a queue using a supplied boolean function, removes and
 * returns the element
 *
 * paramaters:
 *  queue_t *qp: queue of items to apply the function on
 *  void (*fn)(void* elementp,const void* keyp): a function which takes in and
 *        compares two nodes and returns 1 if they are the same and 0 if they
 * 				are not the same, const void* keyp is the node to find
 *  const void* skeyp: a data value to search the queue for
 *
 *  returns: the void* item if the value if found and removed, NULL if not found
 */
void* lqremove(queue_t *qp,
	      int (*searchfn)(void* elementp,const void* keyp),
	      const void* skeyp);

/*
 * Function:  llqconcat()
 * --------------------
 *  oncatenatenates elements of q2 into q1, q2 is dealocated upon completion
 *
 * paramaters:
 *  queue_t *q1p: queue to add the other queue to
 *  queue_t *q2p: queue to be added to the other queue
 *
 *  returns: NULL, the original q1p will now contain q2p on it's end
 */
 void lqconcat(queue_t *q1p, queue_t *q2p);
