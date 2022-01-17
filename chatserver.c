#include "chat.h"

struct player * player_setup(char name[50], int socket){
  printf("hereyee\n");
  struct player * a = malloc(sizeof(struct player));
  a->name[strlen(name) - 1] = '\0';
  printf("herenah\n");
  strcpy(a->name, name);
  a->alive = 1;
  a->socket = socket;
  return a;
}

void print_struct(struct player * s [20], int num_player){
  int i;
  for(i=0;i<=num_player;i++){
    // if (strlen(s[i]->role) == 0){
    //   printf("Player name: %s Player alive status: %d\n", s[i]->name, s[i]->alive);
    // }else{
     printf("Player name: %s\n Player role: %s\n Player alive status: %d\n", s[i]->name, s[i]->role, s[i]->alive);
    // }
  }
}

int main() {
    // only for setting up separate chatrooms
    int sd;

    struct player ** players = malloc(2*sizeof(struct player));
    printf("here\n");
    players[0] = player_setup("steve",4);
    players[1] = player_setup("tony",5);
    printf("here2\n");

    print_struct(players,1);

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