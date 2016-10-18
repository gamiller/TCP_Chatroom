# TCP_Chatroom
##Overview
A simple TCP connection chat client and server that provides a single chat room, written in C.  The server and each client can be run on a seperate server and talk to eachother.  Each client only sees messages sent from other clients who have joined the chatroom and the server; the client can send specific messages to get responses:

* /ping – queries the server to determine if it is up and prints the result 
* /join – joins the chat room (users cannot communicate until they join)
* /leave – leaves the chat room
* /who – obtains the current list of ID’s in the chat room. (only the current use sees the list)

When the server receives a connection from a client it opens a new thread to run that client's message receival and deliverance asynchronously.  The server stores a list of all clients in a locked queue, so that only one client can alter the queue at a time.

##How to run the code:
####Server:
1. run make in the server folder
2. start the server:
```
./server [PORT_NUM]
```
   Takes in a port number to run the server
   
####Client:
1. run make in the client folder, open the client on a different ip address
2. start the client:
```
./client [SCREEN_NAME] [IP_ADDRESS] [PORT_NUM]
```
    SCREEN_NAME- name to be presented to other users next to your messages, must be unique
    IP_ADDRESS- the ip address of the server, you can discover the ip address of the server by logging into it and then cat’ing the file  `/etc/network/interface`
    PORT_NUM- the port number the server is running on
