#include "pipe_networking.h"
#include <sys/select.h>
#include <string.h>

int chatroom(int seconds, int sd, int max_clients);
int chat(int server);