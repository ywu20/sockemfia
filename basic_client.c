#include "pipe_networking.h"

char * get_name(int server){
  char * name;
  printf("Enter your name (less than 50 characters): ");
  fgets(name, 50, stdin);
  write(server, name, 50);
  return name;
}

int main() {

  int from_server;

  from_server = client_handshake();
  char * name = get_name(from_server);
  printf("Your name is: %s",name);
  char role[10] = {0};
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
