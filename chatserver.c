#include "pipe_networking.h"

#include <sys/time.h>

int main() {
    // only for setting up separate chatrooms
    int sd;

    sd = server_setup();

    chatroom(1,sd);
}

int chatroom(int seconds, int sd) { // seconds will but rn doesn't limit chat time
    fd_set read_fds;
    int client_socket[8]; // file descriptors of each client; 8 max
    int client, new_socket;

    while (seconds) {
        FD_ZERO(&read_fds); // clears socket set
        FD_SET(sd, &read_fds); // adds main socket to set

        // add child sockets to set
        int i, max_sd;
        for (i = 0; i < 8; i++) {
            sd = client_socket[i];
            
            // only add valid socket descriptors to set
            if (sd>0) {
                FD_SET(sd, &read_fds);
            }

            // sets max socket descriptor
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // wait indefinitely for a new connection to read
        client = select(max_sd+1, &read_fds, NULL, NULL, NULL);

        if (client < 0 && errno!=EINTR) {
            printf("select error\n");
        }

        // accept new connection
        if (FD_ISSET(sd, &read_fds)) { // checks if socket is in set
            if (new_socket = server_connect(sd)) {
                printf("accept error\n");
                exit(EXIT_FAILURE);
            }
            // server msg
            printf("New connection!\tsocket fd: %d\n", new_socket);
            // send new connection a msg
            write(new_socket, "You have connected!\n",20);
        }

        char input[5];
        // add new sockets to set
        for (i = 0; i < 8; i++) {
            if (client_socket[i] == 0) {
                client_socket[i] = new_socket;
                printf("adding new socket to set at index %d\n",i);
            } else {
                sd = client_socket[i];
                read(sd,input,5);
                printf("from socket %d: %s\n",sd, input);
            }     
        }
    }



    // // preparing set of socket descriptors
    // fd_set read_fds;

    // printf("waiting for connection...\n");

    // // int to_client = select(5, &read_fds, NULL, NULL, NULL);
    // while (seconds) {
    //     FD_ZERO(&read_fds); // clears set    
    //     FD_SET(sd, &read_fds); // adds server socket to set
    //     // if (errno) {
    //     //     printf("error %d: %s\n", errno, strerror(errno));
    //     // }
    //     int client = server_connect(sd);
    //     // FD_SET(client, &read_fds);
    //     printf("sd: %d\tclient: %d\n",sd,client);

    //     if (client > sd) {
    //         sd = client;
    //         printf("new sd: %d\n", sd);
    //     }

    //     int to_client = select(sd+1, &read_fds, NULL, NULL, NULL); // only returns 1 or -1???
    //     printf("to_client: %d\n",to_client);
    //     if (to_client > 2) {
    //         char input[BUFFER_SIZE] = {0};
    //         read(to_client, input, sizeof(input));
    //         printf("from client: %s\n",input);
    //         // write(to_client, "[client]: ", 10);
    //         // write(to_client, input, sizeof(input));
    //     }
    // }
}