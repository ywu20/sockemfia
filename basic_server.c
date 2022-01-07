#include "socket_networking.h"
#include <ctype.h>
#include <signal.h>

static void sighandler(int sig) {
  if (sig == SIGINT) {
    remove("mario");
    exit(0);
  }
}


int main() {

  signal(SIGINT, sighandler);

  int to_client;
  int from_client;
  from_client = server_setup();

  while (1) {
    // connects to client in child process
    // from_client = server_setup();
    // to_client = server_connect(from_client);
    to_client = server_connect(from_client);
      
    int f = fork();
    
    if (f == 0) {
      // get input from client
      char input[100];
      printf("[subserver] reading from client\n");
      read(to_client, input, 100);
      printf("[subserver] received %s\n",input);

      // modify input
      printf("[subserver] initializing modifications\n");
      int i = 0;
      while (input[i] != '\n') {
        input[i] = toupper(input[i]);
        i++;
      }
      printf("[subserver] modified input %s\n",input);

      // sends input back to client
      printf("[subserver] writing to socket\n");
      write(to_client, input, 100);
      printf("[subserver] finished writing\n");
    }
  }
}