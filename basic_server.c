#include "pipe_networking.h"

int *role_setup(int civilian, int mafia, int doctor, int detective)
{
  int shmd = shmget(ROLE_NUM_MEM, sizeof(int) * 4, IPC_CREAT | 0644);
  int *role_num = shmat(shmd, 0, 0);

  // set up number of each role
  role_num[0] = civilian;
  role_num[1] = mafia;
  role_num[2] = doctor;
  role_num[3] = detective;

  return role_num;
}

void remove_shm()
{
  int shmd = shmget(ROLE_NUM_MEM, 0, 0);
  shmctl(shmd, IPC_RMID, 0);
}
int main()
{

  int to_client;
  int sd;

  sd = server_setup();

  printf("dng\n");

  char *roles[4] = {"civilian", "mafia", "doctor", "detective"};

  // set number of people per role
  // now it's set to be 2 civilians and 1 mafia
  // int *num_player_per_role = role_setup(2, 1, 0, 0);

  srand(time(NULL));
  while (1)
  {
    to_client = server_connect(sd);

    printf("%d\n", to_client);
    if (fork() == 0)
    {
      // while(1){
      /*
      // when there's more client than positions, goes forever loop
      // no error checking yet, relying on semaphores.
      printf("===============Client connected===============\n");

      printf("Roles left:\n");
      int i;
      for (i = 0; i < 4; i++)
      {
        printf("%s:%d\n", roles[i], num_player_per_role[i]);
      }
      char *client_role = NULL;
      int r = rand() % 4;

      while (num_player_per_role[r] == 0)
      {
        r = rand() % 4;
      }

      // role is takable, reduce role number left by 1
      client_role = roles[r];
      (num_player_per_role[r])--;

      printf("Gave client %s\n", roles[r]);

      // tell client its role
      write(to_client, client_role, 10);
      */

     printf("===============Client connected===============\n");
      // }
    }
    else
    {
      remove_shm();
    }
  }
}
