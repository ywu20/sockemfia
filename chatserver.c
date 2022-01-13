#include "pipe_networking.h"

#include <sys/select.h>

int main() {
    // only for setting up separate chatrooms
    int sd;

    sd = server_setup();
    printf("sd: %d\n", sd);

    chatroom(1,sd);
}

int chatroom(int seconds, int sd) { // seconds will but rn doesn't limit chat time
    fd_set read_fds;
    FD_ZERO(&read_fds); // clears set
    // FD_SET(sd, &read_fds);  // adds normal socket to set

    while (seconds) {
        int client = server_connect(sd);
        if (FD_ISSET(client, &read_fds)) { // if already in set
            printf("client %d was set\n", client);
        } else { // if not in set
            FD_SET(client, &read_fds); // add to set
            printf("added fd %d to set\n", client);
        }
    }

    return 0;
}