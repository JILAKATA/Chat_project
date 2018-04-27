# A basic Makefile
all: server client

server: server.c
	gcc -pthread -o server server.c

client: client.c
	gcc -pthread -o client client.c

clean:
	-rm *.o $(objects) server client
