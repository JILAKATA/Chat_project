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
void *recvmg(void *sock){
	int sockserv = *((int *)sock);
	char message[BUFSIZE];
	int size;
	while((size = recv(sockserv, message, BUFSIZE, 0)) > 0){
		message[size] = '\0';
		fputs(message, stdout);
		memset(message, '\0', sizeof(message));
	}
}
int main(int argc, char *argv[]){
	struct sockaddr_in serv_addr;
	int sockfd, size, sockserv, serv_addr_size, port;
	pthread_t sendt, recvt;
	char message[BUFSIZE];
	//char username[100];
	char res[BUFSIZE];
	//char ip[INET_ADDRSTRLEN];
	char *host;
	if(argc != 3){
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}

	/*if(argc > 2) {
		printf("too many arguments");
		exit(1);
	}*/
	host = argv[1];
	port = atoi(argv[2]);
	//strcpy(username,argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(serv_addr.sin_zero,'\0', sizeof(serv_addr.sin_zero));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("connection not esatablished");
		exit(1);
	}
	inet_ntop(AF_INET, (struct sockaddr *)&serv_addr, host, INET_ADDRSTRLEN);
	printf("connected to %s, start chatting\n", host);
	pthread_create(&recvt, NULL, recvmg, &sockfd);
	while(fgets(message, BUFSIZE, stdin) > 0) {
		//strcpy(res,username);
		strcat(res, "-->");
		strcat(res, message);
		size = write(sockfd, res, strlen(res));
		if(size < 0) {
			perror("message not sent");
			exit(1);
		}
		memset(message, '\0', sizeof(message));
		memset(res, '\0', sizeof(res));
	}
	pthread_join(recvt, NULL);
	close(sockfd);

}
