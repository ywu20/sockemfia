#include "pipe_networking.h"
#include "parse.h"

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
  int from_server = client_handshake();
  char * name = get_name(from_server);
  printf("Your name is: %s",name);

  while(1){
    char serverComms[BUFFER_SIZE] = {0};
    char in[BUFFER_SIZE] = {0};
    read(from_server, serverComms, sizeof(serverComms));
    char **parsedIn = parse_args(serverComms, STRING_SEPERATOR);

    if (strcmp(parsedIn[0], END_GAME) == 0){
      printf("Game has ended!\n");
      break;
    }else if(strcmp(parsedIn[0], TELL_ROLE) == 0){
      printf("Your role is: %s\n", parsedIn[1]);
    }
    else
    {
      printf("%s\n", parsedIn[0]);
      read(STDIN_FILENO, in, sizeof(in));
      write(from_server, in, sizeof(in));
    }
    free(parsedIn);
  }
}
