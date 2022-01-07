#include "pipe_networking.h"


int main() {

  int from_server;

  from_server = client_handshake();

  while(1){
    char input[BUFFER_SIZE] = {0};
    printf("Enter something...\n");
    read(STDIN_FILENO, input, sizeof(input));
    write(from_server, input, sizeof(input));
    char result[BUFFER_SIZE] = {0};
    read(from_server, result, sizeof(result));
    printf("Result: %s\n", result);
  }
}
