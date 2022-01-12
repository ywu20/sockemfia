#include "pipe_networking.h"

struct player{
  char name [50]; // I stored the \n in name since when people enter names the \n comes with it.
  char role[15];
  int alive; // 1 alive, 0 dead
  int socket;
};

struct player * players[20];
char* roles[6] = {"civilian", "mafia", "doctor","detective", "lead mafia", "hunter"};

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
  strcpy(a->name, name);
  a->alive = 1;
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
     printf("Player name: %s Player role: %s\n Player alive status: %d\n", s[i]->name, s[i]->role, s[i]->alive);
    // }
  }
}

void free_struct(struct player * s[20]){
  int i;
  for(i=0;i<20;i++){
    write(s[i]->socket, "gameEnd", sizeof("gameEnd"));
    free(s[i]);
  }
}

void role_assign(int num_player, int num_player_per_role[6]){
  /*
  // when there's more client than positions, goes forever loop
  // no error checking yet, relying on semaphores.
  printf("Roles left:\n");
  int i;
  for(i=0;i<6;i++){
    printf("%s:%d\n", roles[i], num_player_per_role[i]);
  }
  char* client_role = NULL;
  int r = rand()%6;

  while(num_player_per_role[r] == 0){
    r = rand()%6;
  }

  // role is takable, reduce role number left by 1
  client_role = roles[r];
  (num_player_per_role[r])--;

  printf("Gave client %s\n", roles[r]);

  // tell client its role
  write(to_client, client_role, 15);

  return client_role;
  */
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
    write(players[i]->socket, client_role, 15);
  }
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

int main() {

  int to_client;
  int sd;

  sd = server_setup();

  // set number of people per role
  int * num_player_per_role = role_setup(2,1,0,0,1,0);
  int num_player = 0;

  srand(time (NULL));
  int pid = fork();
  if (pid)
  {
    char in[100] = {0};
    read(STDIN_FILENO, in, sizeof(in));
    kill(pid, 0);
  }
  else
  {
    signal(SIGINT, sighandler);
    while (1)
    {
      to_client = server_connect(sd);
      printf("===============Client connected===============\n");
      char name [50];
      read(to_client, name,50);
      printf("client name: %s\n", name);

      // char * client_role = role_assign(to_client, roles, num_player_per_role);
      players[num_player] = player_setup(name, to_client);
      print_struct(players, num_player);
      num_player++;
      //remove_shm();
    //  free_struct(players);
    }
  }
  // here we assign roles
  role_assign(num_player, num_player_per_role);
  printf("done\n");
}
