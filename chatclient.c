#include "chat.h"

int main() {

  int from_server;

  from_server = client_handshake("localhost");
  printf("from_server: %d\n",from_server);

  if (errno) printf("error %d: %s\n", errno, strerror(errno));
  
  printf("You have entered the chatroom!\n");
  chat(from_server);
}

int chat(int server) {
  char input[100];
  int f = fork();

  if (f == 0) { // child waits for input to send
    while (read(STDIN_FILENO, input, sizeof(input))) {
      write(server, input, 100);
    }
  }
  // main program reads from server client msgs
  while(read(server, input, sizeof(input))){
    printf("%s", input);
  }
  return 0;
}