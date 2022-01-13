all: client server

chat: cserver cclient


client: basic_client.o pipe_networking.o
	gcc -o client basic_client.o pipe_networking.o

server: basic_server.o pipe_networking.o
	gcc -o server basic_server.o pipe_networking.o

basic_client.o: basic_client.c pipe_networking.h
	gcc -c basic_client.c

basic_server.o: basic_server.c pipe_networking.h
	gcc -c basic_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

cserver: chatserver.c pipe_networking.o
	gcc -o cserver chatserver.c pipe_networking.o

cclient: chatclient.c pipe_networking.o
	gcc -o cclient chatclient.c pipe_networking.o

clean:
	rm *.o
	rm *~
