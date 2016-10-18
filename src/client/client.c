/*=============================================================================
 |   Title:  client.c
 |
 |       Author:  Grace Miller
 |     Language:  C
 |   To Compile:  Run the Makefile in the client folder
 |
 |        Class:  CS 63 Programming Parallel Systems
 |     Due Date:  10/15/2016
 |
 +-----------------------------------------------------------------------------
 |
 |  Description:  A TCP client which connects to a running server and can call to other
 |              clients like a chat room. This can be run on a different
 |              ip address than the server, you can discover the ip address of
 |            the server by logging into it and then cat’ing the
 |            file /etc/network/interfaces
 |
 |        Input:  ./client [SCREEN_NAME] [IP_ADDRESS] [PORT_NUM]
 |              SCREEN_NAME- name to be presented to other users next to your messages
 |              IP_ADDRESS- the ip address of the server
 |              PORT_NUM- the port number the server is running on
 |
 |              once running the client takes these commands:
 |                 /ping – queries the server to determine if it is up and prints the result
 |                  /join – joins the chat room, messages not displayed otherwise
 |                  /leave – leaves the chat room
 |                  /who – obtains the current list of ID’s in the chat room.
 |
 |              otherwise, the server sends the user's message to all other clients
 |
 |       Output:  messages from other users, or messages from the server
 |
 *===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>


#define NAMELENGTH 100
#define BUFFERSIZE 2048

volatile sig_atomic_t print_flag = false;
int m_recieved = 0;
int num_tries = 1;


/* message passed to server with the user name inside */
typedef struct Message {
   char buffer[BUFFERSIZE];
   char user_id[NAMELENGTH];
} Message;

/* structure to pass all server params to threads */
typedef struct ServerParams{
  struct sockaddr_in servaddr;
  socklen_t serverlength;
  char *name;
  int sockfd;
} ServerParams;

/*
 * Function:  receive_message()
 * --------------------
 *  a function to run on a seperate thread than the send message for the client
 *  to recieve messages from the server as well as other users
 *
 * paramaters:
 *  void *i_params: a ServerParams structure
 *
 *  returns: 0 when the thread is closed
 */
void *receive_message(void *i_params){
  ServerParams *params = (ServerParams*)i_params;
  char recvline[BUFFERSIZE];

  /* print the server's reply */
  while(1){
    memset(&recvline[0], 0, sizeof(recvline));
    m_recieved = recv(params->sockfd, recvline, BUFFERSIZE, 0);
    if (m_recieved < 0){
      perror("ERROR in recvfrom");
      return 0;
    }
    printf("%s", recvline);

  }
  return 0;
}


/*
 * Function:  send_message()
 * --------------------
 *  a function to run on a seperate thread than the receive message for the client
 *  to send messages from the client to the server and other users
 *
 * paramaters:
 *  void *i_params: a ServerParams structure
 *
 *  returns: 0 when the thread is closed
 */
void *send_message(void *i_params){
  Message *message;
  char *sendline = malloc(sizeof(char));
  int n;
  ServerParams *params = (ServerParams*)i_params;


  message  = (Message *)malloc(sizeof(Message));
  while(1){
    while (fgets(sendline, BUFFERSIZE, stdin) != NULL) {

      strcpy(message->user_id, params->name);
      strcpy(message->buffer, sendline);
      n = send(params->sockfd, (struct Message *)message, sizeof(Message), 0);

      if (n < 0) {
          perror("ERROR in sendto");
        	return 0;
      }
    }
  }
  return 0;
}



int main(int argc, char* argv[]){
  char *name = malloc(sizeof(char));
  char *host_id;
  /*char sendline[800]; */
  int port_num, sockfd, err;
  socklen_t serverlength;
  struct sockaddr_in servaddr;
  /* threads */
  pthread_t send_thread;
  pthread_t recv_thread;


  ServerParams *sparams;

  if(argc != 4){
    printf("incorrect number of arguments.");
    return(0);
  }
  name = argv[1];
  host_id = argv[2];
  port_num = atoi(argv[3]);

  /* create the socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("NO SOCKET");
  	perror("cannot create socket");
  	return 0;
  }

  /* create the socket */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(host_id);
  servaddr.sin_port = htons(port_num);

  /* send the message to the server */
  serverlength = sizeof(servaddr);
  /* connection of the client to the socket */
  if (connect(sockfd, (struct sockaddr *) &servaddr, serverlength)<0) {
          perror("Problem in connecting to the server");
          exit(3);
  }

  printf("Please enter your fist message: ");

  sparams = malloc(sizeof(ServerParams));
  sparams->name = name;
  sparams->servaddr = servaddr;
  sparams->serverlength = serverlength;
  sparams->sockfd = sockfd;

  /* create the thread to send messages from */
  err = pthread_create(&send_thread, NULL, send_message, sparams);
  if (err != 0){
      printf("\ncan't create thread");
      return(1);
  }

  /* create the thread to receive messages from */
  err = pthread_create(&recv_thread, NULL, receive_message, sparams);
  if (err != 0){
      printf("\ncan't create thread");
      return(1);
  }

  pthread_join(send_thread, NULL);
  pthread_join(recv_thread, NULL);

  return 0;



}
