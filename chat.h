#include "pipe_networking.h"
#include <sys/select.h>
#include <string.h>

int chatroom(int seconds, int sd, int max_clients);
int chat(int server);

// simulating game assignment
struct player{
    char name [50];
    char role[15];
    int alive; // 1 alive, 0 dead
    int socket;
    int votes;
};

struct player * player_setup(char name[50], int socket);