#include "pipe_networking.h"

char * get_name(int server){
  char * name;
  printf("Enter your name (less than 50 characters): ");
  fgets(name, 50, stdin);
  write(server, name, 50);
  return name;
}

void rules(){
  printf(" \
  This is MAFIA, an epic party game.\n \
  Try to vote out the mafia! (or kill the good people and hide yourself if you are a mafia)\n \
  Roles: \n \
  Bad people: \n \
    Mafia: Can kill one person at night. \n \
    Lead mafia: Cast the ultimate decision regarding who to kill on the computer. \n \
  Good people: \n \
    Civilians: regular people. Try to protect special roles! \n \
    special roles: \n \
    Detective: can check if a person is good or bad. Check one person per night. \n \
    Doctor: can kill a person with poison and save a person with medicine during the entire game, but can't kill and save at the same night. \n \
    Hunter: can kill a person when the hunter dies.\n");
}
int main() {
  rules();
  int from_server;

  from_server = client_handshake();
  char * name = get_name(from_server);
  printf("Your name is: %s",name);

  char role[15] = {0};
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
