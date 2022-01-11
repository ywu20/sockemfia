#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<signal.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define ACK "HOLA"
#define WKP "mario"
#define SECMSG "Damnit colleges why don't y'all want to accept me!?"

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000
#define ROLE_NUM_MEM 11111



//for forking server
int server_setup();
int server_connect(int from_client);


int server_handshake(int *to_client);
int client_handshake();

#endif
