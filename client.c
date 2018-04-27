#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 1024

char nickname[1024];
void *receive_message(void *sockfd);
int main(int argc, char *argv[]){
	struct sockaddr_in serv_addr;

	/*Creating sockets, size for data, and port number*/
	int sockfd, size, sockserv, serv_addr_size, port;

	/*Create pthread to send and receive*/
	pthread_t sendt, recvt;
	char message[BUFSIZE];
	char res[BUFSIZE];
	char *host;
	
	/*If user doesnt provided ip and port number to
	 * connect to the server*/
	if(argc != 4){
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}
	
	/*ip address, as first argument*/
	host = argv[1];

	/*port number, as second argument*/
	port = atoi(argv[2]);
	strcpy(nickname, argv[3]);

	/*The socket() system call creates a new socket. It takes three arguments. The first is the address domain
	*of the socket. Recall that there are two possible address domains, the unix domain for two processes which share 
	*a common file system, and the Internet domain for any two hosts on the Internet. */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/*Clearing sin_zero, to be bug free*/
	memset(serv_addr.sin_zero,'\0', sizeof(serv_addr.sin_zero));

	/*The variable serv_addr is a structure of type struct sockaddr_in. This structure has four fields. 
	 * The first field is short sin_family, which contains a code for the address family. 
	 * It should always be set to the symbolic constant AF_INET.*/
	serv_addr.sin_family = AF_INET;

	/*The second field of serv_addr is unsigned short sin_port , which contain the port number. 
	 * However, instead of simply copying the port number to this field, it is necessary to convert this to 
	 * network byte order using the function htons() which converts a port number in host byte order to a port 
	 * number in network byte order.*/
	serv_addr.sin_port = htons(port);

	/*The third field of sockaddr_in is a structure of type struct in_addr which contains only a single 
	 * field unsigned long s_addr. This field contains the IP address of the host. For server code, this 
	 * will always be the IP address of the machine on which the server is running, and there is a symbolic 
	 * constant INADDR_ANY which gets this address.*/
	//serv_addr.sin_addr.s_addr = inet_addr("2.0.31.144");
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/*The connect function is called by the client to establish a connection to the server. It takes three 
	 * arguments, the socket file descriptor, the address of the host to which it wants to connect 
	 * (including the port number), and the size of this address. This function returns 0 on success and -1 if it fails*/
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("connection not esatablished");
		exit(1);
	}

	/*The inet_ntop() function shall convert a numeric address into a text string suitable for presentation.*/
	inet_ntop(AF_INET, (struct sockaddr *)&serv_addr, host, INET_ADDRSTRLEN);
	printf("connected to %s, start chatting\n", host);

	/*The pthread_create() function starts a new thread in the calling process.*/
	pthread_create(&recvt, NULL, receive_message, &sockfd);
	
	/*Reads a line from the specified standard input and stores it into the string pointed to by message.*/
	while(fgets(message, BUFSIZE, stdin) > 0) {
		
		/*It copies contents of one string into another string.
		from the client who is sending the message*/
		strcpy(res,nickname);

		/*Concatenates two given strings. It concatenates source string at the 
		end of destination string.*/
		strcat(res, "-->");
		strcat(res, message);

		/*writes up to res bytes from the strlen(res) starting at buf to
       		the file referred to by the file descriptor sockfd.*/
		size = write(sockfd, res, strlen(res));
		if(size < 0) {
			perror("message not sent");
			exit(1);
		}

		/*The memset() function fills the first sizeof(message) bytes of the memory area
		 * pointed to by message with the constant byte '\0'.*/
		memset(message, '\0', sizeof(message));

		/*The memset() function fills the first sizeof(res) bytes of the memory area
		 * pointed to by res with the constant byte '\0'.*/
		memset(res, '\0', sizeof(res));
	}

	/*The pthread_join() function shall suspend execution of the calling thread until the target 
	 * thread terminates, unless the target thread has already terminated.*/
	pthread_join(recvt, NULL);
	close(sockfd);

}

/*Function will receive message data from other clients*/
void *receive_message(void *sockfd){
	int sockserv = *((int *)sockfd);
	char message[BUFSIZE];
	int size;

	/*function shall receive a message from a connection-mode or connectionless-mode socket. 
	It is normally used with connected sockets because it does not permit the application 
	to retrieve the source address of received data.*/
	while((size = recv(sockserv, message, BUFSIZE, 0)) > 0){
		message[size] = '\0';

		/* writes an array of characters to a given file stream. */
		fputs(message, stdout);

		/*The memset() function fills the first sizeof(message) bytes of the memory area
		 * pointed to by message with the constant byte '\0'.*/
		memset(message, '\0', sizeof(message));
	}
}
