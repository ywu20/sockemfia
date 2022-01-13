#include "pipe_networking.h"

int main() {

  int from_server;

  from_server = client_handshake();
  printf("from_server: %d\n",from_server);

  if (errno) printf("error %d: %s\n", errno, strerror(errno));
  
  printf("You have entered the chatroom!\n");
  while(1){
    char input[100] = {0};
    // read(STDIN_FILENO, input, sizeof(input));
    fgets(input, 100, stdin);
    write(from_server, input, 100);
  }
}