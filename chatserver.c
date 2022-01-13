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
    char input[100];
    int max_fd = sd;
    int max_clients = 2;
    // int clients[8] = {0}; // 8 max clients can connect

    // gather the clients
    int clients[max_clients];
    printf("waiting for people to connect\n");
    int i = 0;

    while (i<max_clients) {
        clients[i] = server_connect(sd);
        if (clients[i] > max_fd) {
            max_fd = clients[i];
            printf("clients[i] %d joined\n", clients[i]);
        }
        i++;
    }
    printf("new max_fd: %d\n", max_fd);

    // int c = 0;
    // int i;

    while (seconds) {
        FD_ZERO(&read_fds); // clears set
        FD_SET(sd, &read_fds);  // adds server socket to set

        // int client = server_connect(sd); // checks for who's connecting
        // if (clients[c]==0) { // keeps track of clients fds
        //     clients[c] = client;
        //     c++;
        // } else {
        //     printf("no more clients can connect\n");
        // }

        // if (max_fd < client) {
        //     max_fd = client;
        // }

        // add fds to set
        for (i = 0; i < max_clients; i++) { // adds all the fds to check up on
            if (FD_ISSET(clients[i], &read_fds)) { // if already in set
                printf("client %d was set\n", clients[i]);
            } else { // if not in set
                FD_SET(clients[i], &read_fds); // add to set
                printf("added fd %d to set\n", clients[i]);
            }
            if (max_fd < clients[i]) {
                max_fd = clients[i];
            }
        }
        // printf(">0 if 1 in set: %d\n", FD_ISSET(1,&read_fds)); // indeed its not

        int sel = select(max_fd+1, &read_fds, NULL, NULL, NULL);
        printf("sel: %d\n", sel);

        if (sel) { // if there is only one file left in set
            for (int i = 0; i < max_clients; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &read_fds)) { // if the client is in remaining one
                    printf("going to read from %d\n", clients[i]);
                    read(clients[i], input, 100);
                    printf("got data: %s\n",input);
            }
            }
        }
        printf("loop complete\n\n");
    }

    return 0;
}