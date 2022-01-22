#include "chat.h"

struct player * player_setup(char name[50], int socket){
  struct player * a = malloc(sizeof(struct player));
  a->name[strlen(name) - 1] = '\0';
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
     printf("Player name: %s\n Player role: %s\n Player alive status: %d\nsocket: %d\n", 
     s[i]->name, 
     s[i]->role, 
     s[i]->alive,
     s[i]->socket);
    // }
  }
}

int main() {
    // only for setting up separate chatrooms
    int sd;

    struct player * players[20];
    players[0] = player_setup("steve",4);
    players[1] = player_setup("tony",5);
    // players[2] = player_setup("polly",6);
    // players[3] = player_setup("santonio",7);

    print_struct(players,1);

    sd = server_setup();
    int i = 2;
    while (i) {
        int s = server_connect(sd);
        i--;
    }

    printf("sd: %d\n", sd);

    // printf("socket #: %d\n",players[2]->socket);
    chatroom(30,2,players);
}

int chatroom(int seconds, int max_clients, struct player * players[20]) { // seconds will but rn doesn't limit chat time
    fd_set read_fds, write_fds, clients_fds;
    int max_fd = 0;
    int r;

    struct timeval t = {seconds, 0};

    // gather the clients
    int clients[max_clients];
    printf("waiting for people to connect\n"); 
    int i = 0;

    // tell clients to connect
    for (i=0;players[i];i++){
      write(players[i]->socket, "CHAT", 5);
      printf("told player %s to connect\n", players[i]->name);
      if (!(players[i]->alive)) {
          write(players[i]->socket, "DEAD", 4);
          printf("told player %s to be view only", players[i]->name);
      }
    }

    i = 0;
    while (i<max_clients) {
        clients[i] = (players[i] -> socket);
        if (clients[i] > max_fd) {
            max_fd = clients[i];
            printf("clients[%d] joined\n", clients[i]);
        }
        // FD_SET(sd, &clients_fds); // add server socket to set
        if (FD_ISSET(clients[i], &clients_fds)) { // if already in client set
            printf("client %d was read set\n", clients[i]);
        } else { // if not in set
            FD_SET(clients[i], &clients_fds); // add to client set
            printf("added fd %d to read set\n", clients[i]);
        }
        i++;
    }
    printf("new max_fd: %d\n", max_fd);

    // preparing the timings
    time_t startTime = time(NULL);

    // start the chatroom
    while (time(NULL)-startTime < seconds) {

        // 10 second warning will be moved to a fork
        if (time(NULL)-startTime <= 10.5 && time(NULL)-startTime >= 9.5) {
            write_fds = clients_fds;
            for (int i = 0; i < max_clients && r; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &write_fds)) { // sends 10 second warning
                    printf("going to write to %d: [server] 10 SECONDS LEFT TO CHAT!\n", clients[i]);
                    write(clients[i], "[server] 10 SECONDS LEFT TO CHAT!\n", 34);
                }
            }
        }

        char input[100] = {0};
        char chatter[50] = "";
        char final_message[150] = "";
        FD_ZERO(&read_fds); // clears set
        FD_ZERO(&write_fds);

        // copy client set
        read_fds = clients_fds;
        write_fds = clients_fds;

        int sel = select(max_fd+1, &read_fds, NULL, NULL, &t);
        printf("sel: %d\n", sel);

        if (sel) { // if there is stuff left in read set
            for (int i = 0; i < max_clients; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &read_fds)) { // if the client is in remaining one
                    printf("going to read from %d\n", clients[i]);
                    r = read(clients[i], input, sizeof(input));
                    strncpy(chatter, players[i]->name,50);
                    printf("got data: %s\n",input);
                    printf("chatter: %s\n", chatter);
                    FD_CLR(clients[i], &write_fds);
                }
            }

            // preparing the final message
            int len = 0;
            for (int i = 0; i < 50; i++) {
                if (chatter[i]!='\0') {
                    final_message[i] = chatter[i];
                    printf("copying %c into final msg\n", chatter[i]);
                } else {
                    len = i+1;
                    final_message[i] = ':';
                    final_message[i+1] = ' ';
                    i = 50;
                }
            }
            for (int i = 0; i < 100;i++) {
                if (input[i]!='\n') {
                    final_message[i+len] = input[i];
                    printf("copying %c into final msg\n", final_message[i+len]);
                } else {
                    final_message[i+len] = '\n';
                    i = 100;
                }
            }

             // if there is stuff left in write set
            for (int i = 0; i < max_clients && r; i++) { // loops to find the active client
                if (FD_ISSET(clients[i], &write_fds)) { // if the client is in remaining one
                    printf("going to write to %d: %s\n", clients[i], final_message);
                    write(clients[i], final_message, 150);
                }
            }
        }
        printf("loop complete\n\n");
    }
    
    
    for (i=0;players[i];i++){
      write(players[i]->socket, "STOPTALKING", 11);
      printf("told player %s to stop talking\n", players[i]->name);
    }

    return 0;
}