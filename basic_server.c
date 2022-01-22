#include "pipe_networking.h"
#include "constants.h"
#include "chat.h"
struct player * players[20];

void hunterTakedown(int hunterPlayerNum, int playerCount);

int* role_setup(int civilian, int mafia, int doctor, int detective, int lead_mafia, int hunter){
  int* role_num = malloc(sizeof(int *) * 6);

  // set up number of each role
  role_num[0] = civilian;
  role_num[1] = mafia;
  role_num[2] = doctor;
  role_num[3] = detective;
  role_num[4] = lead_mafia;
  role_num[5] = hunter;

  return role_num;
}

struct player * player_setup(char name[50], int socket){
  struct player * a = malloc(sizeof(struct player));
  name[strlen(name) - 1] = '\0';
  strcpy(a->name, name);
  a->alive = 1;
  a->medicineCount = 1;
  a->poisonCount = 1;
  a->socket = socket;
  return a;
}

char* disclose_players_to_player(){
  int i;
  char *out = malloc(sizeof(char) * BUFFER_SIZE);
  for (i = 0; players[i]; i++)
  {
    if (players[i]->alive)
    {
      char cur[BUFFER_SIZE];
      sprintf(cur, "%d: %s\n", i, players[i]->name);
      strcat(out, cur);
    }
  }
  out[strlen(out) - 1] = '\0';
  return out;
}

void free_struct(struct player * s[20]){
  int i;
  for(i=0;players[i];i++){
    write(s[i]->socket, END_GAME, sizeof(END_GAME));
    free(s[i]);
  }
}

void role_assign(int num_player, int num_player_per_role[6]){
  char out[BUFFER_SIZE] = {0};
  int i;
  for (i = 0; i < num_player && players[i]; i++)
  {
    char* client_role = NULL;
    int r = rand()%6;

    while(num_player_per_role[r] == 0){
      r = rand()%6;
    }

    // role is takable, reduce role number left by 1
    client_role = roles[r];
    (num_player_per_role[r])--;

    // assign role in player struct
    strcpy(players[i]->role, client_role);

    // tell client its role
    char in[BUFFER_SIZE] = {0};
    strcat(in, TELL_ROLE);
    strcat(in, sep);
    strcat(in, client_role);
    write(players[i]->socket, in, sizeof(in));
  }
  free(num_player_per_role);
  //TODO: less hacky way of accounting for the lead mafia than add one after roles are assigned? And to deal with the special roles
  num_mafia++;
  num_special *= 3;
}

void reset_votes(int playerCount){
  int i;
  for (i = 0; i < playerCount; i++){
    players[i]->votes = 0;
  }
}

int eliminate_player(int playerCount, int specifiedPlayer){
  int votes = 0;
  int i;
  if (specifiedPlayer < 0 || specifiedPlayer >= num_player){
    for (i = 0; i < playerCount; i++)
    {
      if (votes < players[i]->votes){
        specifiedPlayer = i;
        votes = players[i]->votes;
      }else if (votes == players[i]->votes){
        specifiedPlayer = num_player;
      }
    }
  }
  if (specifiedPlayer >= 0 && specifiedPlayer < num_player){
    players[specifiedPlayer]->alive = false;
    if (strstr(players[specifiedPlayer]->role, "mafia")){
      num_mafia -= 1;
    }else if (strcmp(players[specifiedPlayer]->role, "civilian") == 0){
      num_civilian -= 1;
    }else{
      if (strcmp(players[specifiedPlayer]->role, "hunter") == 0){
        hunterTakedown(specifiedPlayer, playerCount);
      }
      num_special -= 1;
    }
  }
  reset_votes(playerCount);
  return specifiedPlayer;
}

void informAllPlayers(int dead_player, char * note){
  int i;
  char out[BUFFER_SIZE] = NOTIFY_PLAYER;
  strcat(out, sep);
  strcat(out, note);
  sprintf(out, out, players[dead_player]->name);
  for (i = 0; players[i]; i++)
  {
    write(players[i]->socket, out, sizeof(out));
  }
}

void sigint_handle(){
  free_struct(players);
  exit(SIGINT);
}

static void sighandler(int signo){
 if(signo == SIGINT){
   sigint_handle();
 }
}

void hunterTakedown(int hunterPlayerNum, int playerCount){
  char in[BUFFER_SIZE] = {0};

  if (strcmp(players[hunterPlayerNum]->role, "hunter") == 0)
  {
    int invalid_input = 1;
    int hunter_voted_player = playerCount;
    while (invalid_input)
    {
      write(players[hunterPlayerNum]->socket, "You can take a player down with you when you die. Do you want to bring a player with you [y/n]?", BUFFER_SIZE);
      read(players[hunterPlayerNum]->socket, in, sizeof(in));

      char a;
      sscanf(in, "%c", &a);
      if (a == 'y')
      {
        char out[BUFFER_SIZE] = HUNTER_PROMPT;
        strcat(out, disclose_players_to_player());
        while (hunter_voted_player < 0 || hunter_voted_player >= playerCount)
        {
          write(players[hunterPlayerNum]->socket, out, BUFFER_SIZE);
          read(players[hunterPlayerNum]->socket, in, sizeof(in));
          sscanf(in, "%d", &hunter_voted_player);
        }

        informAllPlayers(hunter_voted_player, "Player %s was killed by the ghost of a slain hunter.");
        eliminate_player(playerCount, hunter_voted_player);
        invalid_input = 0;
      }
      else if (a == 'n')
      {
        invalid_input = 0;
      }
    }
  }
}

int getPlayerNumInput(char * out, int playerNum, int playerCount){
  int votedPlayer = playerCount;
  char in[BUFFER_SIZE] = {0};

  while (votedPlayer < 0 || votedPlayer >= playerCount)
  {
    write(players[playerNum]->socket, out, BUFFER_SIZE);
    read(players[playerNum]->socket, in, sizeof(in));
    sscanf(in, "%d", &votedPlayer);
  }

  return votedPlayer;
}

int checkForGameEnd(int playerCount, int mafiaCount, int civilianCount, int specialCount){
  int i;
  char toPlayers[BUFFER_SIZE] = NOTIFY_PLAYER;
  if (mafiaCount == 0)
  {
    strcat(toPlayers, sep);
    strcat(toPlayers, INNOCENT_WIN);
    for (i = 0; i < playerCount; i++){
      write(players[i]->socket, toPlayers, sizeof(toPlayers));
    }
    return 1;
  }
  else if (specialCount == 0 || civilianCount == 0)
  {
    strcat(toPlayers, sep);
    strcat(toPlayers, MAFIA_WIN);
    for (i = 0; i < playerCount; i++){
      write(players[i]->socket, toPlayers, sizeof(toPlayers));
    }
    return 1;
  }
  return 0;
}

void nightCycle(int playerCount)
{
  int i;
  char in[BUFFER_SIZE] = {0};
  int dead_player = -1;
  int votedPlayer;
  char a;

  informAllPlayers(-1, "The sun has set. Night has fallen among us.");
  // detective
  for (i = 0; i < playerCount; i++)
  {
    if (strcmp("detective", players[i]->role) == 0 && players[i]->alive)
    {
      char out[BUFFER_SIZE] = DETECTIVE_PROMPT;
      strcat(out, disclose_players_to_player());
      votedPlayer = getPlayerNumInput(out, i, playerCount);
      char message[100];
      if ((strcmp(players[votedPlayer]->role, "mafia") == 0) || (strcmp(players[votedPlayer]->role, "lead mafia") == 0))
      {
        sprintf(message, "Player %s is a bad person. Hit enter to continue.", players[votedPlayer]->name);
      }
      else
      {
        sprintf(message, "Player %s is a good person. Hit enter to continue.", players[votedPlayer]->name);
      }
      write(players[i]->socket, message, sizeof(message));
      read(players[i]->socket, in, sizeof(in));
    }
  }

  // mafia
  for (i = 0; i < playerCount; i++)
  {
    if (strcmp("lead mafia", players[i]->role) == 0 && players[i]->alive)
    {
      char out[BUFFER_SIZE] = MAFIA_PROMPT;
      strcat(out, disclose_players_to_player());
      votedPlayer = getPlayerNumInput(out, i, playerCount);
      dead_player = votedPlayer;
    }
  }
  // doctor
  for (i = 0; i < playerCount; i++)
  {
    int invalid_input = 1;
    int votedPlayer = playerCount;
    if (strcmp("doctor", players[i]->role) == 0 && players[i]->alive && i != dead_player)
    {
      while (invalid_input)
      {
        if (players[i]->medicineCount > 0)
        {
          char save[BUFFER_SIZE] = {0};
          strcat(save, players[dead_player]->name);
          strcat(save, " was killed tonight, do you want to save this person? [y/n]");
          write(players[i]->socket, save, BUFFER_SIZE);
          read(players[i]->socket, in, sizeof(in));

          sscanf(in, "%c", &a);

          if (a == 'y')
          {
            players[i]->medicineCount--;
            invalid_input = 0;
          }
          else if (a == 'n')
          {
            informAllPlayers(dead_player, "Player %s was killed last night.");
            eliminate_player(playerCount, dead_player);
            invalid_input = 0;
          }
        }

        if (players[i]->poisonCount > 0 && players[i]->alive)
        {
          char kill[100] = "Do you want to poison anyone tonight? [y/n]";
          write(players[i]->socket, kill, BUFFER_SIZE);
          read(players[i]->socket, in, sizeof(in));

          sscanf(in, "%c", &a);
          char message[BUFFER_SIZE] = "";

          if (a == 'y')
          {
            char out[BUFFER_SIZE] = "Who do you want to posion?\n";
            strcat(out, disclose_players_to_player());
            votedPlayer = getPlayerNumInput(out, i, playerCount);

            informAllPlayers(votedPlayer, "Player %s was poisoned and died last night.");
            eliminate_player(playerCount, votedPlayer);
            players[i]->poisonCount--;
            invalid_input = 0;
          }
          else if (a == 'n')
          {
            invalid_input = 0;
          }
        }
      }
    }
  }
}

void dayCycle(int playerCount){
  informAllPlayers(-1, "The sun has risen. But there is animosity in the air.");
  chatroom(20, playerCount, players); // doesn't have access to its own socket but maybe that can be fixed later
  int i;
  int votedPlayer;
  char out[BUFFER_SIZE] = VOTE_PLAYER;
  strcat(out, disclose_players_to_player());
  for (i = 0; i < playerCount; i++)
  {
    if (players[i]->alive)
    {
      votedPlayer = getPlayerNumInput(out, i, playerCount);
      players[votedPlayer]->votes++;
    }
  }
  int playerKilled = eliminate_player(playerCount, -1);
  informAllPlayers(playerKilled, "Player %s was killed in the broad daylight.");
}

void gameCycle(int playerCount){
  while (1)
  {
    // night cycle
    nightCycle(playerCount);
    if (checkForGameEnd(num_player, num_mafia, num_civilian, num_special)){
      break;
    }
    // day cycle
    dayCycle(playerCount);
    if (checkForGameEnd(num_player, num_mafia, num_civilian, num_special)){
      break;
    }
  }
}

int chatroom(int seconds, int max_clients, struct player * players[20]) {
    fd_set read_fds, write_fds, clients_fds;
    int max_fd;
    int r;

    struct timeval t = {seconds, 0};

    // gather the clients
    int clients[max_clients];
    printf("waiting for people to connect\n");
    int i = 0;

    // tell clients to connect
    for (i=0;players[i];i++){
      write(players[i]->socket, "CHAT", 4);
      printf("told player %s to connect\n", players[i]->name);
    }

    i = 0;
    while (i<max_clients) {
        clients[i] = (players[i] -> socket);
        if (clients[i] > max_fd) {
            max_fd = clients[i];
            printf("clients[%d] joined\n", clients[i]);
        }
        // FD_SET(3, &clients_fds); // add server socket to set
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
        // if (time(NULL)-startTime <= 10.5 && time(NULL)-startTime >= 9.5) {
        //     write_fds = clients_fds;
        //     for (int i = 0; i < max_clients && r; i++) { // loops to find the active client
        //         if (FD_ISSET(clients[i], &write_fds)) { // sends 10 second warning
        //             printf("going to write to %d: [server] 10 SECONDS LEFT TO CHAT!\n", clients[i]);
        //             write(clients[i], "[server] 10 SECONDS LEFT TO CHAT!\n", 34);
        //         }
        //     }
        // }

        char input[100] = "";
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
                    r = read(clients[i], input, 100);
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

int main() {

  int to_client;
  int sd = server_setup();

  num_player = 0;
  gameCapacity = 0;
  char in[BUFFER_SIZE] = {0};

  srand(time (NULL));
  while (gameCapacity < MIN_PLAYERS){
    printf("How many players will be playing this game of Mafia?\nYou need at least %d people to play this game.\n", MIN_PLAYERS);
    read(STDIN_FILENO, in, sizeof(in));
    sscanf(in, "%d", &gameCapacity);
  }

  printf("Game is now open for players to log on!\n");

  signal(SIGINT, sighandler);
  while (num_player < gameCapacity)
  {
    to_client = server_connect(sd);
    printf("===============Player connected===============\n");
    char name[50];
    read(to_client, name, sizeof(name));
    printf("Player name: %s\n", name);

    players[num_player] = player_setup(name, to_client);
    num_player++;
  }
  num_special = 1;
  num_civilian = (num_player - 3 * num_special)/2;
  num_mafia = num_player- 3 * num_special-num_civilian-1;
  if(num_mafia > 5){
    num_mafia = 5;
    num_civilian = num_player-3 * num_special-num_mafia-1;
  }

  // set number of people per role
  int * num_player_per_role = role_setup(num_civilian,num_mafia,num_special,num_special,1,num_special);
  // here we assign roles
  role_assign(num_player, num_player_per_role);
  gameCycle(num_player);
  free_struct(players);
}
