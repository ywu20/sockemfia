#define DETECTIVE_PROMPT "Who do you want to know about?\n"
#define MAFIA_PROMPT "Who do you want to kill?\n"
#define DOCTOR_PROMPT "Who do you want to save?"
#define VOTE_PLAYER "Who do you want to accuse?\n"
#define HUNTER_PROMPT "Who do you want to take down with you?\n"
#define MAFIA_WIN "The mafia has won!"
#define INNOCENT_WIN "The civilians has won!"
#define GAME_HAS_ENDED "Game has ended!\n"

#define TELL_ROLE "tellRole"
#define END_GAME "gameEnd"
#define RECEIVED_MSG "msgGot"
#define NOTIFY_PLAYER "notifyPlayer"
#define STRING_SEPERATOR '\\'
#define true 1
#define false 0
#define MIN_PLAYERS 6

struct player{
  char name [50];
  char role[15];
  int alive; // 1 alive, 0 dead
  int socket;
  int votes;
  int medicineCount;
  int poisonCount;
};

char sep[2] = {STRING_SEPERATOR, 0};
char* roles[6] = {"civilian", "mafia", "doctor","detective", "lead mafia", "hunter"};

int num_player;
int num_mafia;
int num_special;
int num_civilian;
int gameCapacity;