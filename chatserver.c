#include "pipe_networking.h"

int main() {
    // only for setting up separate chatrooms
    int sd;

    sd = server_setup();

    chatroom(1,sd);
}

int chatroom(int seconds, int sd) { // seconds will but rn doesn't limit chat time
    // preparing set of socket descriptors
    fd_set read_fds;

    printf("waiting for connection...\n");

    // int to_client = select(5, &read_fds, NULL, NULL, NULL);
    while (seconds) {
        FD_ZERO(&read_fds); // clears set    
        FD_SET(sd, &read_fds); // adds server socket to set
        int client = server_connect(sd);
        FD_SET(client, &read_fds);
        printf("sd: %d\tclient: %d\n",sd,client);

        if (client > sd) {
            sd = client;
        }

        int to_client = select(sd+1, &read_fds, NULL, NULL, NULL);
        printf("to_client: %d\n",to_client);
        if (to_client > 2) {
            char input[BUFFER_SIZE] = {0};
            read(to_client, input, sizeof(input));
            printf("from client: %s\n",input);
            // write(to_client, "[client]: ", 10);
            // write(to_client, input, sizeof(input));
        }
    }
}