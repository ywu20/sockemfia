#include "pipe_networking.h"
#include <sys/select.h>
#include <string.h>
#include <time.h>

#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[0m"
#define STOP_TALKING "STOPTALKING"

int chatroom(int seconds, int max_clients, struct player * players[20]);
int chat(int server, char living);
/*
// simulating game assignment
struct player{
    char name [50];
    char role[15];
    int alive; // 1 alive, 0 dead
    int socket;
    int votes;
};
*/

struct player * player_setup(char name[50], int socket);
