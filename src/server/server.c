/*=============================================================================
 |   Title:  server.c
 |
 |       Author:  Grace Miller
 |     Language:  C
 |   To Compile:  Run the Makefile in the server folder
 |
 |        Class:  CS 63 Programming Parallel Systems
 |     Due Date:  10/15/2016
 |
 +-----------------------------------------------------------------------------
 |
 |  Description:  A TCP server which takes in multiple clients and allows them
 |              to talk like a chat room.  Every client that it accepts becomes
 |              a new thread within the server.  This can be run on a different
 |              ip address than the clients, you can discover the ip address of
 |            the server by logging into it and then cat’ing the
 |            file /etc/network/interfaces
 |
 |        Input:  ./server [PORT_NUM]
 |              Takes in a port number to run the server
 |
 |       Output:  prints information on the server running, to end the server just control C
 |
 *===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "queue.h"
#include "lqueue.h"



#define NAMELENGTH 100
#define BUFFERSIZE 2048
#define ADDRLENGTH 50
#define LISTENQ 8
#define MAXPEOPLE 1000
#define SWITCHCOUNT 4
#define PING 0
#define JOIN 1
#define LEAVE 2
#define WHO 3

/* message passed to server with the user name inside */
typedef struct Message{
   char buffer[BUFFERSIZE];
   char user_id[NAMELENGTH];
}Message;

/* Struct to pass the socket information to the server thread*/
typedef struct Connection{
  int csocket;
}Connection;

/* ChatUser struct which contains information to send messages back
this information stored in the queue of users*/
typedef struct ChatUser{
  char name[NAMELENGTH];
  int usocket;
} ChatUser;

/* queue of users currently "joined" in the chatroom */
lqueue_t *myqueue;
/* message to send to all users */
char public_message_tosend[BUFFERSIZE];
int sockfd;
/* person sending the current message */
char curr_sender[NAMELENGTH];
/* socket of the person sending the current message */
int curr_sender_socket;




void print(void* elementp){
  ChatUser *c = (ChatUser*) elementp;
  printf("printq: %s\n", c->name);
}

/*
 * Function:  find_user()
 * --------------------
 * comparator method to be passed into lqsearch() and lqremove() to
 * compare if two chatusers are the same person, since each username is
 * unique they can compare names
 *
 * paramaters:
 *  void* elementp: the element to see if it is the same element
 *  const void* id: the element to find
 *
 *  returns: 0 if user not found, 1 if user found
 */
int find_user(void* elementp, const void* id){
  ChatUser *curr_user = (ChatUser *)elementp;
  ChatUser *search_user = (ChatUser *) id;

  if(strcmp(curr_user->name, search_user->name) == 0){
    return TRUE;
  }else{
    return FALSE;
  }
}

/*
 * Function:  send_message_toall()
 * --------------------
 * method to be applied to each every in the queue using lqapply() which
 * sends the current message out to all users, unless it is the user sending
 * the message
 *
 * paramaters:
 *  void* elementp: the element containing the user to send the message to
 *
 *  returns: NULL
 */
void send_message_toall(void* elementp){
  int reclen;
  ChatUser *curr_user = (ChatUser*) elementp;

  if(strcmp(curr_user->name, curr_sender) != 0){
    reclen = send(curr_user->usocket, public_message_tosend, BUFFERSIZE, 0);
  }
  if (reclen < 0) {
    perror("ERROR in sendto");
  }
}

/*
 * Function:  send_user_in_room()
 * --------------------
 * method to be applied to each every in the queue using lqapply() which
 * sends the current user's name back to the user who requested it using the
 * '/who' request.  It only sends the list of users to the user who requested it
 *
 * paramaters:
 *  void* elementp: the element containing the user name to send
 *
 *  returns: NULL
 */
void send_user_in_room(void *elementp){
  int reclen;
  char user_tosend[BUFFERSIZE];
  ChatUser *curr_user = (ChatUser*) elementp;

  if(strcmp(curr_user->name, curr_sender) != 0){
    strcpy(user_tosend, curr_user->name);
    strcat(user_tosend, "\n");
    reclen = send(curr_sender_socket, user_tosend, BUFFERSIZE, 0);
  }
  if (reclen < 0) {
    perror("ERROR in sendto");
  }

}


/*
 * Function:  combine_return_message()
 * --------------------
 * helper method to synthesize the user's message with their user name
 *
 * paramaters:
 *  Message *message: message struct containing the user name and the message
 *
 *  returns: char *, the message with the username concatenated to the front
 */
char *combine_return_message(Message *message){
  char *src = malloc(sizeof(char)*BUFFERSIZE);
  char *dest = malloc(sizeof(char)*BUFFERSIZE);

  strcpy(src,  message->user_id);
  strcat(src,  ": ");
  strcpy(dest, message->buffer);
  strcat(src, dest);

  return src;
}

/*
 * Function:  add_user()
 * --------------------
 * helper method to add a user to the queue of users if they are not already in it
 *
 * paramaters:
 *   ChatUser *chat_user: the user to add into the queue
 *
 *  returns: char *, the message to return to the user about whether they were
 *          successfully added or not
 */
char *add_user(ChatUser *chat_user){
  void *returned_user;
  char *add_status_message = malloc(sizeof(char)*BUFFERSIZE);

  returned_user = lqsearch(myqueue, find_user, chat_user);

  if(!returned_user){
     lqput(myqueue, chat_user);
     add_status_message = "SERVER: successfully joined the chatroom, start typing!\n";
  }else{
    add_status_message = "SERVER ERROR: A user with this username already exists!\n";
  }
  return add_status_message;
}

/*
 * Function:  check_switches()
 * --------------------
 * helper method to check whether a user's message is one of the installed messages
 * for the server client, which then responds accordingly:
 |          /ping – tells the user that the server is running
 |           /join – adds user to the chat room, messages not displayed otherwise
 |           /leave – removes user from the chat room
 |          /who – obtains the current list of ID’s in the chat room, return to the server
 *
 * paramaters:
 *   ChatUser *chat_user: the user to add into the queue
 *   Message *message: the user's message
 *
 *  returns: int, 1 if the user's message was a switch case, 0 if it was not
 */
int check_switches(Message *message, ChatUser *chat_user){
  int i;
  char sendback[BUFFERSIZE];
  const char *switches[SWITCHCOUNT] = {"/ping", "/join", "/leave", "/who"};

  for(i = 0; i < SWITCHCOUNT; i++){
    if(strncmp(message->buffer, switches[i], (strlen(switches[i]) -1)) == 0){
      strcpy(sendback,"\n");
      if(i == WHO){
        lqapply(myqueue, print);
        printf("IN WHO\n");
        strcpy(curr_sender, message->user_id);
        curr_sender_socket = chat_user->usocket;
        lqapply(myqueue, send_user_in_room);
      }
      else if(i == PING){
        strcpy(sendback,"SERVER: server is currently running...\n");
      }else if(i == JOIN){
        strcpy(sendback, add_user(chat_user));
      }else if(i == LEAVE){
        lqremove(myqueue, find_user, chat_user);
        lqapply(myqueue, print);

        strcpy(sendback,"SERVER: leaving the chat room..\n");
      }

      /* send message back */
      send(chat_user->usocket, sendback, BUFFERSIZE, 0);
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * Function:  send_out_message()
 * --------------------
 * helper method to send a user's message to every other user, this checks
 * that the user is in the queue, concatinates the message, and calls lqapply()
 * to send that message to all other users
 *
 * paramaters:
 *   ChatUser *chat_user: the user to add into the queue
 *   Message *message: the user's message
 *
 *  returns: NULL
 */
void send_out_message(Message *message, ChatUser *chat_user){
  ChatUser *returned_user = (ChatUser *)malloc(sizeof(ChatUser));
  returned_user = (ChatUser *)lqsearch(myqueue, find_user, chat_user);

  if(returned_user != NULL){
    strcpy(public_message_tosend, combine_return_message(message));
    strcpy(curr_sender, message->user_id);
    lqapply(myqueue, send_message_toall);
  }

}

/*
 * Function:  new_connection()
 * --------------------
 * A method to be called by a new thread to run asynchronously for every client
 * that requests a connection with the server, it continuously takes in and responds
 * to that client's messages
 *
 * paramaters:
 *   void *newsocket: a Connection struct, which provides the socket have messages sent ot
 *
 *  returns: 0 when the thread is finished
 */
void *new_connection(void *newsocket){
  Connection *currsocket = (Connection *)newsocket;
  Message *message;
  int reclen;
  ChatUser *chat_user = (ChatUser *)malloc(sizeof(ChatUser));

  message = (Message *)malloc(sizeof(Message));
  while(1){
    while ( (reclen = recv(currsocket->csocket, (struct Message *)message, sizeof(Message),0)) > 0)  {
        printf("recieved message from %s: %s", message->user_id, message->buffer);

        strcpy(chat_user->name, message->user_id);
        chat_user->usocket = currsocket->csocket;


        if(check_switches(message, chat_user) == FALSE){
          send_out_message(message, chat_user);
        }

        if (reclen < 0) {
             perror("Read error");
             exit(1);
        }
   }
 }
 close(currsocket->csocket);
 return 0;
}


int main(int argc, char* argv[]){
  int SERV_PORT = 0;
  struct sockaddr_in servaddr;
  struct sockaddr_in clientaddr;
  socklen_t addrlen = sizeof(servaddr);

  int newsocket, i, err;
  Connection *n_connection;

  /* user threads */
  pthread_t tid[MAXPEOPLE];
  /* current thread number */
  int curr_conn_num = 0;
  myqueue = lqopen();


  if(argc != 2){
    printf("incorrect number of arguments.");
    return(0);
  }
  SERV_PORT = atoi(argv[1]);

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
       perror("Problem in creating the socket");
       exit(2);
  }

  /* create the socket */
  memset((char *) &servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  /* bind the socket to the address */
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    printf("bind failed");
  	perror("bind failed");
  	return 0;
  }

  /* listen for incoming connections */
  if(listen (sockfd, LISTENQ) == 0){
    printf("server listening for clients...\n");
  }else{
    perror("listening failed...\n");
  }

  /* accept all new connections from different clients, send them to a seperate thread */
  while(1){
    newsocket = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
    printf("Received new connection request, number %d...\n", curr_conn_num);

    n_connection = (Connection *)malloc(sizeof(Connection));
    n_connection->csocket = newsocket;

    err = pthread_create(&tid[curr_conn_num], NULL, new_connection, n_connection);
    if (err != 0){
        printf("\ncan't create thread");
        return(1);
    }
    curr_conn_num++;
  }

  for(i = 0; i <= curr_conn_num; i++){
    pthread_join(tid[i], NULL);
  }

  return(1);

}
