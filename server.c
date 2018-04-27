#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

/*Data buffer*/
#define BUFSIZE 1024

#define PORT 8080

struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN];
};

/*Global variables to keep the number of clients
 * online and the data*/
int clients[BUFSIZE];
int number_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*This function will send the message to every client, who is
 online, there is a for loop, to count for every the number of 
clients online to send the data to each */
void send_message(char *data,int curr){
	pthread_mutex_lock(&mutex);

	/*As long as there is one client online*/
	for(int i = 0; i < number_clients; i++){
		
		/*As long as the same client is not sending itself message */
		if(clients[i] != curr){
			if(send(clients[i], data, strlen(data), 0) < 0) {
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}

/*This function will know if users are dissconected from server,
everytime a client disconects from the server, it will display
a message of "DISCONECTED..."*/
void *handle_client(void *arg){

	struct client_info cl = *((struct client_info *)arg);
	char message[BUFSIZE];
	int datalen;

	/*It is a socket with data ready to be read, while data is 
	 greater than zero */
	while((datalen = recv(cl.sockno, message, BUFSIZE, 0)) > 0){
		message[datalen] = '\0';
		/*Send data or message, by calling the function*/
		send_message(message, cl.sockno);

		/*Clearing sin_zero, to be bug free*/
		memset(message, '\0', sizeof(message));
	}
	pthread_mutex_lock(&mutex);
	printf("DISCONNECTED...\n");
	
	for(int i = 0; i < number_clients; i++){
		int j;
		if(clients[i] == cl.sockno){
			j = i;
			while(j < number_clients-1){
				clients[j] = clients[j+1];
				j++;
			}
		}
	}

	/*Decreasing number of clients online*/
	number_clients--;
	pthread_mutex_unlock(&mutex);
}

int main(int argc,char *argv[]){
	struct sockaddr_in serv_addr, cli_addr;

	/*Creating socket for server and client*/
	int sockfd, sockcl;

	socklen_t cli_addr_size;
	pthread_t sendt, recvt;

	/*Create message */
	char message[BUFSIZE];
	char *welcome_message = "WELCOME TO OUR GROUP CHAT!! \r\n";
	int size;
	struct client_info cl;
	char ip[INET_ADDRSTRLEN];//;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Socket");
		exit(1);
	}

	/*Clearing sin_zero, to be bug free*/
	memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));
	
	/*type of socket created*/

	/*The variable serv_addr is a structure of type struct sockaddr_in. This structure has four fields. 
	 * The first field is short sin_family, which contains a code for the address family. 
	 * It should always be set to the symbolic constant AF_INET.*/
	serv_addr.sin_family = AF_INET;

	/*The second field of serv_addr is unsigned short sin_port , which contain the port number. However, 
	 * instead of simply copying the port number to this field, it is necessary to convert this to network 
	 * byte order using the function htons() which converts a port number in host byte order to a port 
	 * number in network byte order.*/
	serv_addr.sin_port = htons(8080);
	//my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/*The third field of sockaddr_in is a structure of type struct in_addr which contains only a single 
	 * field unsigned long s_addr. This field contains the IP address of the host. For server code, 
	 * this will always be the IP address of the machine on which the server is running, and there is a 
	 * symbolic constant INADDR_ANY which gets this address.*/
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	cli_addr_size = sizeof(cli_addr);
	
	/*Bind our socket to the port*/
	if(bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
		perror("ERROR on bind");
		exit(1);
	}

	/*Start listening for incoming connections*/
	if(listen(sockfd, 10) == -1){
		perror("ERROR on listened");
		exit(1);
	}

	/*Main loop for connection until CRL-C to discconect the program*/
	while(1){

		/*if connection from a client has been unsuccessfully not established.*/
		if((sockcl = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addr_size)) < 0){
			perror("ERROR on accept");
			exit(1);
		}
		pthread_mutex_lock(&mutex);
		printf("CONNECTED...\n");

		/*Send a welcome message to a new client who gets online*/
		if( send(sockcl, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) ){
			perror("ERROR");
		}
		cl.sockno = sockcl;
		strcpy(cl.ip, ip);
		clients[number_clients] = sockcl;

		/*Increasing the number of clients online*/
		number_clients++;

		/*Function starts a new thread in the calling process.*/
		pthread_create(&recvt, NULL, handle_client, &cl);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
