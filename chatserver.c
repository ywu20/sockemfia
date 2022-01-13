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
    char input[5];
    int max_fd = sd;
    int clients[8] = {0}; // 8 max clients can connect
    int c = 0;

    while (seconds) {
        FD_ZERO(&read_fds); // clears set
        FD_SET(sd, &read_fds);  // adds normal socket to set

        int client = server_connect(sd); // checks for who's connecting
        if (clients[c]==0) { // keeps track of clients fds
            clients[c] = client;
            c++;
        } else {
            printf("no more clients can connect\n");
        }

        if (max_fd < client) {
            max_fd = client;
        }

        if (FD_ISSET(client, &read_fds)) { // if already in set
            printf("client %d was set\n", client);
        } else { // if not in set
            FD_SET(client, &read_fds); // add to set
            printf("added fd %d to set\n", client);
        }
        // printf(">0 if 1 in set: %d\n", FD_ISSET(1,&read_fds)); // indeed its not

        int sel = select(max_fd+1, &read_fds, NULL, NULL, NULL);
        printf("sel: %d\n", sel);

        if (sel) { // if there is only one file left in set
            for (int i = 0; i < c; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &read_fds)) { // if the client is in remaining one
                    printf("going to read from %d\n", clients[i]);
                    read(clients[i], input, 5);
                    printf("got data: %s\n",input);
                }
            }
        }
        printf("loop complete\n");
    }

    return 0;
}