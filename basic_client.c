#include "socket_networking.h"


int main() {

    int to_server;
    int from_server;

    from_server = client_handshake(&to_server);

    // gets user input
    char input[100];

    char input2[100];
    printf("Enter input: ");
    fgets(input, 100, stdin);

    // sends input to server
    write(from_server, input, 100);

    // receives input from server
    read(from_server, input2, 100);

    // prints returned input
    printf("Server returned: %s\n", input2);
}