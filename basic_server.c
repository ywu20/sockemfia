#include "pipe_networking.h"
#include "constants.h"

struct player * players[20];

int* role_setup(int civilian, int mafia, int doctor, int detective, int lead_mafia, int hunter){
  int shmd = shmget(ROLE_NUM_MEM, sizeof(int) * 6, IPC_CREAT | 0644);
  int* role_num = shmat (shmd,0,0);

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

void remove_shm(){
  int shmd = shmget(ROLE_NUM_MEM,0,0);
  shmctl(shmd, IPC_RMID,0);
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

char* disclose_players_to_player(){
  int i;
  char *out = malloc(BUFFER_SIZE);
  for (i = 0; players[i]; i++)
  {
    if (players[i]->alive)
    {
      char cur[BUFFER_SIZE];
      sprintf(cur, "%d", i);
      strcat(cur, ": ");
      strcat(cur, players[i]->name);
      strcat(cur, "\n");
      strcat(out, cur);
    }
  }
  out[strlen(out) - 1] = '\0';
  printf("%s\n", out);
  return out;
}

void free_struct(struct player * s[20]){
  int i;
  for(i=0;i<20;i++){
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
}

void reset_votes(int playerCount){
  int i;
  for (i = 0; i < playerCount; i++){
    players[i]->votes = 0;
  }
}

void eliminate_player(int playerCount){
  int playerOut = -1;
  int votes = 0;
  int i;
  for (i = 0; i < playerCount; i++){
    if (votes < players[i]->votes){
      playerOut = i;
      votes = players[i]->votes;
    }
  }
  if (playerOut != -1){
    players[playerOut]->alive = false;
    char msg[BUFFER_SIZE] = NOTIFY_PLAYER;
    strcat(msg, sep);
    strcat(msg, players[playerOut]->name);
    strcat(msg, " was elimated.");
    for (i = 0; i < playerCount; i++){
      write(players[i]->socket, msg, sizeof(msg));
    }
  }
  reset_votes(playerCount);
}

void sigint_handle(){
  remove_shm();
  free_struct(players);
  exit(SIGINT);
}

static void sighandler(int signo){
 if(signo == SIGINT){
   sigint_handle();
 }
}

int checkForGameEnd(int playerCount, int mafiaCount, int civilianCount, int specialCount){
  int i;
  char toPlayers[BUFFER_SIZE] = NOTIFY_PLAYER;
  if (mafiaCount == 0)
  {
    strcat(toPlayers, STRING_SEPERATOR);
    strcat(toPlayers, INNOCENT_WIN);
    for (i = 0; i < playerCount; i++){
      write(players[i]->socket, toPlayers, sizeof(toPlayers));
    }
    return 1;
  }
  else if (specialCount == 0 || civilianCount == 0)
  {
    strcat(toPlayers, STRING_SEPERATOR);
    strcat(toPlayers, MAFIA_WIN);
    for (i = 0; i < playerCount; i++){
      write(players[i]->socket, toPlayers, sizeof(toPlayers));
    }
    return 1;
  }
  return 0;
}

void nightCycle(int playerCount){
  int i;
  char in[BUFFER_SIZE] = {0};
  int dead_player=-1;

  // detective
  for (i = 0; i < playerCount; i++)
  {
    int votedPlayer = playerCount;
    if (strcmp("detective", players[i]->role) == 0 && players[i]->alive)
    {
      while (votedPlayer < 0 || votedPlayer >= playerCount)
      {
        char out[BUFFER_SIZE] = DETECTIVE_PROMPT;
        strcat(out, sep);
        strcat(out, disclose_players_to_player());
        write(players[i]->socket, out, BUFFER_SIZE);
        read(players[i]->socket, in, sizeof(in));
        sscanf(in, "%d", &votedPlayer);
        char message [100] = "Player ";
        strcat(message, players[votedPlayer] -> name);
        if((strcmp(players[votedPlayer] ->role,"mafia") == 0) || (strcmp(players[votedPlayer] -> role , "lead mafia") == 0)){
          strcat(message, " is a bad person. Hit enter to continue.\n");
        }else{
          strcat(message, " is a good person. Hit enter to continue.\n");
        }
        write (players[i] -> socket, message, 100);
        read(players[i]->socket, in, sizeof(in));
      }
      //players[votedPlayer]->votes++;
    }
  }
  checkForGameEnd(num_player, num_mafia, num_civilian, num_special);
  reset_votes(playerCount);

  // mafia
  for (i = 0; i < playerCount; i++)
  {
    int votedPlayer = playerCount;
    if (strcmp("lead mafia", players[i]->role) == 0 && players[i]->alive)
    {
      while (votedPlayer < 0 || votedPlayer >= playerCount)
      {
        char out[BUFFER_SIZE] = MAFIA_PROMPT;
        strcat(out, sep);
        strcat(out, disclose_players_to_player());
        write(players[i]->socket, out, BUFFER_SIZE);
        read(players[i]->socket, in, sizeof(in));
        sscanf(in, "%d", &votedPlayer);
        dead_player = votedPlayer;
        players[votedPlayer]->alive = 0;
      }
      //players[votedPlayer]->votes++;
    }
  }
  reset_votes(playerCount);
  // doctor
  for (i = 0; i < playerCount; i++)
  {
    int invalid_input = 1;
    int votedPlayer = playerCount;
    if (strcmp("doctor", players[i]->role) == 0 && players[i]->alive)
    {
      while (invalid_input)
      {
        if(players[i]->medicineCount > 0){
        char save [BUFFER_SIZE] =  "";
        strcat(save, players[dead_player]->name);
        strcat(save, " was killed tonight, do you want to save this person? [y/n]");
        write(players[i]->socket, save, BUFFER_SIZE);
        read(players[i]->socket, in, sizeof(in));

        char a;
        sscanf(in, "%c", &a);

        if(a == 'y'){
          players[i]->medicineCount--;
          players[dead_player] -> alive = 1;
          invalid_input = 0;
        }else if (a == 'n'){
          invalid_input = 0;
        }
    }

    if(players[i]->poisonCount > 0){
      char kill [100] = "Do you want to poison anyone tonight? [y/n]";
      write(players[i]->socket, kill, BUFFER_SIZE);
      read(players[i]->socket, in, sizeof(in));

      char a;
      sscanf(in, "%c", &a);
      char message [BUFFER_SIZE] = "";

      if(a == 'y'){
        while (votedPlayer < 0 || votedPlayer >= playerCount)
        {
          char out[BUFFER_SIZE] = DOCTOR_PROMPT;
          strcat(out, sep);
          strcat(out, disclose_players_to_player());
          write(players[i]->socket, out, BUFFER_SIZE);
          read(players[i]->socket, in, sizeof(in));
          sscanf(in, "%d", &votedPlayer);
          players[votedPlayer]->alive = 0;
          players[i]->poisonCount--;
          invalid_input = 0;
        }
      }else if (a == 'n'){
        invalid_input = 0;
      }
    }

    //write(players[i]->socket, "success!\n", 100);
    //  read(players[i]->socket, in, sizeof(in));
      }
    }
  }
  checkForGameEnd(num_player, num_mafia, num_civilian, num_special);
  reset_votes(playerCount);

  if(players[dead_player]->alive == 0){
    int i;
    char out[BUFFER_SIZE] = NOTIFY_PLAYER;
    strcat(out, sep);
    strcat(out, "Player ");
    strcat(out, players[dead_player]->name);
    strcat(out, " died last night.\n");
    for (i=0;players[i];i++){
      write(players[i]->socket, out, BUFFER_SIZE);
    }
    if(strcmp(players[dead_player]->role, "hunter") == 0){
      int invalid_input = 1;
      while(invalid_input){
      int votedPlayer = playerCount;
      write(players[dead_player]->socket, "You can bring a player with you when you die. Do you want to bring a player with you[y/n]?", BUFFER_SIZE);
      read(players[dead_player]->socket, in, sizeof(in));

      char a;
      sscanf(in, "%c", &a);
      printf("scanned character: %c\n", a);

      if(a == 'y'){
        while (votedPlayer < 0 || votedPlayer >= playerCount)
        {
        char out[BUFFER_SIZE] = HUNTER_PROMPT;
        strcat(out, sep);
        strcat(out, disclose_players_to_player());
        write(players[dead_player]->socket, out, BUFFER_SIZE);
        read(players[dead_player]->socket, in, sizeof(in));
        sscanf(in, "%d", &votedPlayer);
        printf("scanned: %d\n", votedPlayer);
        players[votedPlayer]->alive = 0;
      }
      invalid_input = 0;
      }else if (a == 'n'){
        invalid_input = 0;
      }
    }
  }

  }

}

void dayCycle(int playerCount){
  int i;
  char in[BUFFER_SIZE] = {0};
  int votedPlayer;
  for (i = 0; i < playerCount; i++)
  {
    if (players[i]->alive)
    {
      votedPlayer = playerCount;
      while (votedPlayer < 0 || votedPlayer >= playerCount)
      {
        char out[BUFFER_SIZE] = VOTE_PLAYER;
        strcat(out, sep);
        strcat(out, disclose_players_to_player());
        printf("%s\n", out);
        write(players[i]->socket, out, BUFFER_SIZE);
        read(players[i]->socket, in, sizeof(in));
        sscanf(in, "%d", &votedPlayer);
      }
      players[votedPlayer]->votes++;
    }
  }
  eliminate_player(playerCount);

  if(strcmp(players[votedPlayer]->role, "hunter") == 0){
    int invalid_input = 1;
    int hunter_voted_player = playerCount;
    while(invalid_input){
    write(players[votedPlayer]->socket, "You can bring a player with you when you die. Do you want to bring a player with you[y/n]?", BUFFER_SIZE);
    read(players[votedPlayer]->socket, in, sizeof(in));

    char a;
    sscanf(in, "%c", &a);
    if(a == 'y'){
      while (hunter_voted_player < 0 || hunter_voted_player >= playerCount)
      {
      char out[BUFFER_SIZE] = HUNTER_PROMPT;
      strcat(out, sep);
      strcat(out, disclose_players_to_player());
      write(players[votedPlayer]->socket, out, BUFFER_SIZE);
      read(players[votedPlayer]->socket, in, sizeof(in));
      sscanf(in, "%d", &hunter_voted_player);
      players[hunter_voted_player]->alive = 0;
      invalid_input = 0;
    }
    }else if (a == 'n'){
      invalid_input = 0;
    }
  }
}
}

void day1NightTask(int playerCount){
  int i;
  char in[BUFFER_SIZE] = {0};
  for (i = 0; i < playerCount; i++)
  {
    if (strcmp("lead mafia", players[i]->role) == 0 && players[i]->alive)
    {
      write(players[i]->socket, "Mafia, look up. See your fellow members. Press enter when you look back here.", BUFFER_SIZE);
      read(players[i]->socket, in, sizeof(in));
      i = playerCount;
    }
  }
}

void gameCycle(int playerCount){
  // day 1 night
  day1NightTask(playerCount);
  while (1)
  {
    // night cycle
    nightCycle(playerCount);
    // day cycle
    dayCycle(playerCount);
    // announce the deaths of the day here
  }
}

int main() {

  int to_client;
  int sd = server_setup();

  // set number of people per role
  int * num_player_per_role = role_setup(1,1,1,1,1,1);
  num_player = 0;
  num_mafia = 0;
  num_special = 0;
  num_civilian = 0;
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
    printf("===============Client connected===============\n");
    char name[50];
    read(to_client, name, 50);
    printf("client name: %s\n", name);

    // char * client_role = role_assign(to_client, roles, num_player_per_role);
    players[num_player] = player_setup(name, to_client);
    print_struct(players, num_player);
    num_player++;
    //remove_shm();
    //  free_struct(players);
    }
  // }
  // here we assign roles
  role_assign(num_player, num_player_per_role);
  gameCycle(num_player);
}
