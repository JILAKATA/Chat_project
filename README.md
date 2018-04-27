# Chat_project
This is a Program Project for CS371, Introduction to Computer Networking

Online chats are very common software that allow several users to discuss in real time through the Internet. 
Common examples are Skype, Microsoft Messenger, etc. 
In this project you will design a small chat system. The system includes a Chat Server, that is responsible for 
accepting connections from Clients and receiving/forwarding messages, and Clients, that connect to the server and 
send/receive messages. 
The project will be developed in C using socket programming and thread. 
We will use the library <sys/socket.h> for socket programming and a description can be found here: 
  	http://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html . 
We can use <pthread.h> for thread and a description can be found here: 
  	http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html . 

To Compile the program just type:
	 make
