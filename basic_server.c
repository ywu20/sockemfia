#include "pipe_networking.h"


int main() {

  int to_client;
  int sd;

    sd = server_setup();

    printf("dng");

    while (1)
    {
      to_client = server_connect(sd);

      if (fork() == 0)
      {

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
    }
  }
}