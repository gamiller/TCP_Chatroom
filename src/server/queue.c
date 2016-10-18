 /*=============================================================================
 |   Title: Queue.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct QueueNode {
   void *data;
	 /* next points backwards to the next item to be popped,
	 the tail's next value is NULL */
   struct QueueNode *next;
} QueueNode;

typedef struct {
  QueueNode *head;
  QueueNode *tail;
} queue_t;

/*
 * Function:  qopen()
 * --------------------
 * a dummy function for opening a queue; returns NULL
 *
 * paramaters: null
 *
 *  returns: queue_t*, an open queue
 */
queue_t* qopen(void){
  queue_t *myqueue;
  myqueue = (queue_t *)malloc(sizeof(queue_t));
  /* set the head and tail to point to nothing */
  myqueue->head = NULL;
  myqueue->tail = NULL;
  return myqueue;
}

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
void qclose(queue_t *qp){
/* want to go from head, if it has next free it and then go to next and keep going, then free self */
QueueNode *curr_node;
QueueNode *temp_node;

if(qp->head){
    curr_node = qp->head;
    while(curr_node){
      QueueNode *next_node;
      next_node = curr_node->next;
      temp_node = curr_node;
      free(temp_node);
      curr_node = next_node;
    }
	}
    free(qp);
}

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
int qput(queue_t *qp, void *elementp){
  QueueNode *node;
  node = (QueueNode*)malloc(sizeof(QueueNode));


	if (!elementp || !qp){
		return -1;
	}
	node->next = NULL;
	node->data = elementp;

  /* if the queue is empty */
  if(qp->head == NULL){
    qp->head = node;
		qp->tail = node;
		return 0;
  }
  /* if the queue is not empty, place the item in the rear of the queue */
  else{
		printf("here\n");
    qp->tail->next = node;
		qp->tail = node;
  }
  return 0;
}


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
void* qget(queue_t *qp){
	QueueNode *temp_node;
  /* if the list is empty return null */
  if(!qp->head ){
    return NULL;
  }
  else{
    /*create another node to hold onto the old head */
    temp_node = qp->head;
		qp->head = temp_node->next;

    return temp_node->data;
  }
}

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
void qapply(queue_t *qp, void (*fn)(void* elementp)){
    QueueNode *curr_node;

		if(!qp || !qp->head){
			return;
		}
    curr_node = qp->head;
		/* apply the function to each node */
    while(curr_node){
			fn(curr_node->data);
      curr_node = curr_node->next;
    }

		return;
}

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
	      const void* skeyp){

		QueueNode *curr_node;
		if(!qp || !qp->head){
			return NULL;
		}

    curr_node = qp->head;
    while(curr_node){
      if(searchfn(curr_node->data, skeyp)){
        return curr_node->data;
      }
			curr_node = curr_node->next;

    }
    return NULL;

}

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
	      const void* skeyp){
    QueueNode *prev_node;
    QueueNode *curr_node;

		if(!qp || !qp->head){
			return NULL;
		}

		curr_node = qp->head;
		prev_node = NULL;

    while(curr_node){

			if(searchfn(curr_node->data, skeyp)){
				if(prev_node != NULL){
					/* if this is the tail node */
					if(curr_node->next == NULL){
						qp->tail = prev_node;
						prev_node->next = NULL;
					}else{
						/* if this is a middle node */
						prev_node->next = curr_node->next;
					}
				}else{
					/* if this is the head node */
					qp->head = curr_node->next;
				}
				return curr_node->data;
			}

			prev_node = curr_node;
			curr_node = curr_node->next;

		}
    return NULL;

}

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
void qconcat(queue_t *q1p, queue_t *q2p){
  if((q1p->tail == NULL) && (q2p->head == NULL)){
      q1p->head = q2p->head;
      q1p->tail = q2p->tail;
      free(q2p);
    }
  else{
    q1p->tail->next = q2p->head;
    q2p->head = q2p->tail = NULL;
    free(q2p);
  }
}
