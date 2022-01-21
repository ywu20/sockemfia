#include "chat.h"

int chat(int server, char living) {
  printf("You have entered the chatroom!\n");
  char input[100];
  int f = 0;

  if (living == '0') {
    f = fork();

    if (f == 0) { // child waits for input to send
      while (read(STDIN_FILENO, input, sizeof(input))) {
        write(server, input, 100);
      }
    }
  } else {
    printf("You are dead. You cannot talk.\n");
  }

  // main program reads from server client msgs
  while(read(server, input, sizeof(input)) && strcmp(input, "STOPTALKING")){
    printf("%s", input);
  }
  if (f) kill(f, SIGKILL); // removes child process
  printf("\nchatroom over\n\n");
  return 0;
}

int main() {

  int from_server;

  from_server = client_handshake("localhost");
  printf("from_server: %d\n",from_server);

  if (errno) printf("error %d: %s\n", errno, strerror(errno));
  
  chat(from_server, '0');
}
