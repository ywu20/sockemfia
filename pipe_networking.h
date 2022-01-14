#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/wait.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000
#define ROLE_NUM_MEM 11111

#define DETECTIVE_PROMPT "Who do you want to know about?"
#define MAFIA_PROMPT "Who do you want to kill?"
#define DOCTOR_PROMPT "Who do you want to save?"
#define VOTE_PLAYER "Who do you want to accuse?"

#define TELL_ROLE "tellRole"
#define END_GAME "gameEnd"
#define RECEIVED_MSG "msgGot"
#define NOTIFY_PLAYER "notifyPlayer"
#define STRING_SEPERATOR '\\'
#define ALIVE 1
#define DEAD 0

//for forking server
int server_setup();
int server_connect(int from_client);

int server_handshake(int *to_client);
int client_handshake();

#endif
