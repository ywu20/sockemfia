#include "pipe_networking.h"
struct player{
  char name [50]; // I stored the \n in name since when people enter names the \n comes with it.
  char role[15];
  int alive; // 1 alive, 0 dead
};

int* role_setup(int civilian, int mafia, int doctor, int detective, int lead_mafia, int hunter){
  int shmd = shmget(ROLE_NUM_MEM, sizeof(int) * 6, IPC_CREAT | 0644);
  int* role_num = shmat (shmd,0,0);

  // set up number of each role
  role_num[0] = civilian;
  role_num[1]= mafia;
  role_num[2] = doctor;
  role_num[3] = detective;
  role_num[4] = lead_mafia;
  role_num[5] = hunter;

  return role_num;
}

struct player * player_setup(char name[50], char role[15]){
  struct player * a = malloc(sizeof(struct player));
  strcpy(a->name, name);
  strcpy(a->role, role);
  a->alive = 1;
  return a;
}

void remove_shm(){
  int shmd = shmget(ROLE_NUM_MEM,0,0);
  shmctl(shmd, IPC_RMID,0);
}

void print_struct(struct player * s [20], int num_player){
  int i;
  for(i=0;i<=num_player;i++){
  printf("Player name: %s Player role: %s\n Player alive status: %d\n", s[i]->name, s[i]->role, s[i]->alive);
}
}

char * role_assign(int to_client, char * roles[6], int num_player_per_role[6]){
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
}
int main() {

  int to_client;
  int sd;

  sd = server_setup();

  char* roles[6] = {"civilian", "mafia", "doctor","detective", "lead mafia", "hunter"};

  // set number of people per role
  int * num_player_per_role = role_setup(2,1,0,0,1,0);
  struct player * players[20];
  srand(time (NULL));
  int num_player = 0;
    while (1)
    {
      to_client = server_connect(sd);
      printf("===============Client connected===============\n");
      char name [50];
      read(to_client, name,50);
      printf("client name: %s\n", name);

      char * client_role = role_assign(to_client, roles, num_player_per_role);
      players[num_player] = player_setup(name, client_role);
      print_struct(players, num_player);
      num_player++;
      remove_shm();

  }
}
