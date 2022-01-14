#include "chat.h"

int main() {

  int from_server;

  from_server = client_handshake();
  printf("from_server: %d\n",from_server);

  if (errno) printf("error %d: %s\n", errno, strerror(errno));
  
  printf("You have entered the chatroom!\n");
  chat(from_server);
}

int chat(int server) {
  int f = fork();

  if (f == 0) { // child waits for input to send
    while (1) {
      char input[100];
      read(STDIN_FILENO, input, sizeof(input));
      // fgets(input, 100, stdin);
      write(server, input, 100);
    }
  }
  // main program reads from server client msgs
  while(1){
    char input[100] = {0};
    read(server, input, sizeof(input));
    // fgets(input, 100, stdin);
    // write(from_server, input, 100);
    printf("[other client]: %s\n", input);
  }
  return 0;
}