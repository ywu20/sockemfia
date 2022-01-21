all: client server

chat: cserver cclient

client: basic_client.o pipe_networking.o parse.o
	gcc -o client basic_client.o pipe_networking.o parse.o

server: basic_server.o pipe_networking.o
	gcc -o server basic_server.o pipe_networking.o

basic_client.o: basic_client.c pipe_networking.h parse.h constants.h
	gcc -c basic_client.c

basic_server.o: basic_server.c pipe_networking.h constants.h
	gcc -c basic_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

cserver: chatserver.c pipe_networking.o
	gcc -o cserver chatserver.c pipe_networking.o

cclient: chatclient.c pipe_networking.o
	gcc -o cclient chatclient.c pipe_networking.o

parse.o: parse.c parse.h
	gcc -c parse.c

clean:
	rm *.o
	rm *~


