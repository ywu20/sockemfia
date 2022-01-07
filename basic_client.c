#include "pipe_networking.h"


int main() {

  int from_server;

  from_server = client_handshake();
  
  char role[10];
  read(from_server, role, sizeof(role));
  printf("You are a %s\n", role);

  while(1){

    /*
    write(from_server, input, sizeof(input));
    char result[BUFFER_SIZE] = {0};
    read(from_server, result, sizeof(result));
    printf("Result: %s\n", result);
    */
  }
}
