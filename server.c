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
int clients[BUFSIZE];
int number_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void send_message(char *data,int curr){
	//int i;
	pthread_mutex_lock(&mutex);
	for(int i = 0; i < number_clients; i++){
		if(clients[i] != curr){
			if(send(clients[i], data, strlen(data), 0) < 0) {
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}
void *handle_client(void *arg){

	struct client_info cl = *((struct client_info *)arg);
	char message[BUFSIZE];
	int datalen;
	//int i;
	//int j;
	
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
	//printf("%s \n",cl.ip);
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
	//int portno;
	pthread_t sendt, recvt;

	/*Create message */
	char message[BUFSIZE];
	int size;
	struct client_info cl;
	char ip[INET_ADDRSTRLEN];//;
	//;
	//if(argc > 2) {
	//	printf("too many arguments");
	//	exit(1);
	//}
	//portno = atoi(argv[1]);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Socket");
		exit(1);
	}

	/*Clearing sin_zero, to be bug free*/
	memset(serv_addr.sin_zero, '\0', sizeof(serv_addr.sin_zero));
	
	/*type of socket created*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	//my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
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

	/*Main loop for connection*/
	while(1){

		/*if connection from a client has been unsuccessfully not established.*/
		if((sockcl = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addr_size)) < 0){
			perror("ERROR on accept");
			exit(1);
		}
		pthread_mutex_lock(&mutex);
		//inet_ntop(AF_INET, (struct sockaddr *)&cli_addr, ip, INET_ADDRSTRLEN);
		//printf("%s connected\n",ip);
		printf("CONNECTED...\n");
		cl.sockno = sockcl;
		strcpy(cl.ip, ip);
		clients[number_clients] = sockcl;
		number_clients++;
		pthread_create(&recvt, NULL, handle_client, &cl);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
