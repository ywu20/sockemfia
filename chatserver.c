#include "chat.h"

int main() {
    // only for setting up separate chatrooms
    int sd;

    sd = server_setup();
    printf("sd: %d\n", sd);

    chatroom(1,sd,2);
}

int chatroom(int seconds, int sd, int max_clients) { // seconds will but rn doesn't limit chat time
    fd_set read_fds, write_fds, clients_fds;
    int max_fd = sd;
    int r;

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

        FD_SET(sd, &clients_fds); // add server socket to set
        if (FD_ISSET(clients[i], &clients_fds)) { // if already in client set
            printf("client %d was read set\n", clients[i]);
        } else { // if not in set
            FD_SET(clients[i], &clients_fds); // add to client set
            printf("added fd %d to read set\n", clients[i]);
        }

        i++;
    }
    printf("new max_fd: %d\n", max_fd);

    // start the chatroom
    while (seconds) {
        char input[100];
        FD_ZERO(&read_fds); // clears set
        FD_ZERO(&write_fds);

        // copy client set
        read_fds = clients_fds;
        write_fds = clients_fds;

        int sel = select(max_fd+1, &read_fds, NULL, NULL, NULL);
        printf("sel: %d\n", sel);

        if (sel) { // if there is stuff left in read set
            for (int i = 0; i < max_clients; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &read_fds)) { // if the client is in remaining one
                    printf("going to read from %d\n", clients[i]);
                    r = read(clients[i], input, 100);
                    for (int j = 0; j < 100; j++) {
                        if (input[j] == '\n') {
                            input[j] = '\0';
                            j = 100;
                        }
                    }
                    printf("got data: %s\n",input);
                    FD_CLR(clients[i], &write_fds);
                }
            }

             // if there is stuff left in write set
            for (int i = 0; i < max_clients && r; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &write_fds)) { // if the client is in remaining one
                    printf("going to write to %d: %s\n", clients[i], input);
                    write(clients[i], input, 100);
                }
            }
        }
        printf("loop complete\n\n");
    }

    return 0;
}