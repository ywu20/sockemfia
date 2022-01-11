#include "pipe_networking.h"
struct player{
  char name [50];
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

void remove_shm(){
  int shmd = shmget(ROLE_NUM_MEM,0,0);
  shmctl(shmd, IPC_RMID,0);
}
int main() {

  int to_client;
  int sd;

  sd = server_setup();

  char* roles[6] = {"civilian", "mafia", "doctor","detective", "lead mafia", "hunter"};

  // set number of people per role
  int * num_player_per_role = role_setup(2,1,0,0,1,0);

  srand(time (NULL));
    while (1)
    {
      to_client = server_connect(sd);

        // when there's more client than positions, goes forever loop
        // no error checking yet, relying on semaphores.
              printf("===============Client connected===============\n");
              char name[50];
              read(to_client, name,sizeof(name));
              printf("client name: %s\n", name);
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
      /*
      while(1){
        char input[BUFFER_SIZE] = {0};
        read(to_client, input, sizeof(input));
        int i;
        for (i = 0; i < BUFFER_SIZE && input[i]; i++)
        {
          input[i] += 1;
          if (input[i] == 'a' || input[i] == 'A')
          {
            input[i] = '4';
          }
          else if (input[i] == 'e' || input[i] == 'E')
          {
            input[i] = '3';
          }
          else if (input[i] == 'I' || input[i] == 'i')
          {
            input[i] = '1';
          }
        }
        write(to_client, input, sizeof(input));
      }
      */

      remove_shm();
  }
}
