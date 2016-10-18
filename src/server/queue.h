/*=============================================================================
|   Title: Queue.h
|
|       Author:  Grace Miller
|     Language:  C
|   To Compile:  Run the Makefile
|
|    Class:  CS 63 Programming Parallel Systems
|    Date:  9/27/2016
|
+-----------------------------------------------------------------------------
|
|  Description:  implements a generic queue that is singly linked with a head
|  and a tail
|  items are inserted into the tail of the queue, and popped from the head of the queue
|
|  each node item has a next pointer, next points backwards to the next item to be popped,
|  the tail's next value is NULL
|
*===========================================================================*/

#pragma once
/*
* queue.h -- public interface to the queue module
*/

#define TRUE 1
#define FALSE 0

/* the queue representation is hidden from users of the module */
typedef void queue_t;

/*
* Function:  qopen()
* --------------------
* a dummy function for opening a queue; returns NULL
*
* paramaters: null
*
*  returns: queue_t*, an open queue
*/
queue_t* qopen(void);

/*
* Function:  qclose()
* --------------------
* deallocate a queue, frees everything in it
*
* paramaters:
*  queue_t *qp: queue to close
*
*  returns: NULL
*/
void qclose(queue_t *qp);

/*
* Function:  qput()
* --------------------
* put element at end of queue, queue elements insert into the tail of the queue
*
* paramaters:
*  queue_t *qp: queue to insert into
*  void *elementp: element to insert in
*
*  returns: 0 if successful, -1 if not successful
*/
int qput(queue_t *qp, void *elementp);

/*
* Function:  qget()
* --------------------
* get first element from queue, queue items are taken from the head of the queue
*
* paramaters:
*  queue_t *qp: queue to get from
*
*  returns: void*, item from the queue (value inside the node)
*/
void* qget(queue_t *qp);

/*
* Function:  qapply()
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
void qapply(queue_t *qp, void (*fn)(void* elementp));

/*
* Function:  qsearch()
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
void* qsearch(queue_t *qp,
int (*searchfn)(void* elementp,const void* keyp),
const void* skeyp);

/*
 * Function:  qremove()
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
void* qremove(queue_t *qp,
	      int (*searchfn)(void* elementp,const void* keyp),
	      const void* skeyp);

/*
 * Function:  qconcat()
 * --------------------
 *  oncatenatenates elements of q2 into q1, q2 is dealocated upon completion
 *
 * paramaters:
 *  queue_t *q1p: queue to add the other queue to
 *  queue_t *q2p: queue to be added to the other queue
 *
 *  returns: NULL, the original q1p will now contain q2p on it's end
 */
 void qconcat(queue_t *q1p, queue_t *q2p);
