#include "chat.h"
#include "constants.h"

int main() {

  int from_server;

  from_server = client_handshake("localhost");
  printf("from_server: %d\n",from_server);

  if (errno) printf("error %d: %s\n", errno, strerror(errno));
  
  chat(from_server);
}

int chat(int server) {
  printf("You have entered the chatroom!\n");
  char input[100] = {0};
  int f = 0;

  if (read(server,input,sizeof(input)) && strcmp(input, "DEAD")==0){
    printf("input (dead?): %s\n\n", input);
    f = fork();

    if (f == 0) { // child waits for input to send
      while (read(STDIN_FILENO, input, sizeof(input))) {
        write(server, input, sizeof(input));
      }
    }
  }

  // main program reads from server client msgs
  int gameEnd = -1;
  while (read(server, input, sizeof(input)) && strcmp(input, STOP_TALKING) && (gameEnd = strcmp(input, END_GAME)))
  {
    printf("%s", input);
  }
  kill(f, SIGKILL); // removes child process
  printf("\nchatroom over\n\n");
  if (gameEnd == 0){
    printf("Game has ended!\n");
  }
  return 0;
}