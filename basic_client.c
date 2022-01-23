#include "pipe_networking.h"
#include "parse.h"
#include "constants.h"
#include "chat.h"
char * get_name(int server){
  char * name = malloc(sizeof(char) * 50);
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

int main(int argc, char *argv[]) {
  rules();
  char *ipAddress = "localhost";
  if (argc > 1){
    ipAddress = argv[1];
  }
  int from_server = client_handshake(ipAddress);
  char * name = get_name(from_server);
  printf("Your name is: %s",name);

  while(1){
    char serverComms[BUFFER_SIZE] = {0};
    char in[BUFFER_SIZE] = {0};
    read(from_server, serverComms, sizeof(serverComms));
    char **parsedIn = parse_args(serverComms, STRING_SEPERATOR);

    if (strcmp(parsedIn[0], END_GAME) == 0){
      printf(GAME_HAS_ENDED);
      break;
    }else if(strcmp(parsedIn[0], TELL_ROLE) == 0){
      printf("Your role is: %s\n", parsedIn[1]);
    }
    else if(strcmp(parsedIn[0], NOTIFY_PLAYER) == 0){
      printf("%s\n", parsedIn[1]);
    }
    else if(strncmp(parsedIn[0], "CHAT",4) == 0) {
      chat(from_server, parsedIn[0][4]);
    }
    else
    {
      printf("%s\n", parsedIn[0]);
      int f = fork();

      if (f == 0){
        while(read(from_server, in, sizeof(in)) && strcmp(in, END_GAME)){
        }
        kill(getppid(), SIGINT);
        printf(GAME_HAS_ENDED);
        exit(0);
      }
      else
      {
        read(STDIN_FILENO, in, sizeof(in));
        write(from_server, in, sizeof(in));
        kill(f, SIGKILL);
      }
    }
    free(parsedIn);
  }
}

int chat(int server, char living) {
  printf("You have entered the chatroom!\n");
  char input[100];
  int f = 0;

  if (living == '1') {
    // printf("living: %c\tliving='0': %d\n", living,(living == '0'));
    f = fork();

    if (f == 0) { // child waits for input to send
      while (read(STDIN_FILENO, input, sizeof(input)-1)) {
        // for (int i = 0; i < sizeof(input); i++) {
        //   if (input[i]=='\n') input[i] = '\0';
        // }
        input[99] = '\n';
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
