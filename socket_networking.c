#include "socket_networking.h"
#include <ctype.h>


/*=========================
  server_setup
  args:
  returns socket descriptor
  =========================*/
int server_setup() {
  // sets up addrinfo and hints to set up socket
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; // for TCP ports
  hints->ai_flags = AI_PASSIVE; // ???
  getaddrinfo(NULL, "7010", hints, &results); // NULL to connect with multiple clients
  // does the search for the ports and stuff

  // make the socket
  int soc = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  printf("soc: %d\n",soc);

  // bind the port
  bind(soc,results->ai_addr, results->ai_addrlen);
  printf("setup completed\n");

  free(hints);
  freeaddrinfo(results);
  return soc;
}

/*=========================
  server_connect
  args: int socket_descriptor
  =========================*/
int server_connect(int soc) {
  // listen
  listen(soc,10);

  // accept
  int client_socket; 
  socklen_t sock_size;
  struct sockaddr_storage client_address;
  sock_size = sizeof(client_address);
  printf("[server] server waiting to accept\n");
  client_socket = accept(soc,(struct sockaddr *)&client_address, &sock_size);
  return client_socket;
}


/*=========================
  client_handshake
  args:
  =========================*/
int client_handshake(int * to_server) {
  // set up addrinfo
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; // for TCP ports
  getaddrinfo("149.89.150.129", "7010", hints, &results); // localhost rn
  if (errno) {
    printf("error %d: %s\n",errno, strerror(errno));
    errno = 0;
  }


  int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  printf("sd: %d\n",sd);
  connect(sd, results->ai_addr, results->ai_addrlen);
  if (errno) {
    printf("error %d: %s\n",errno, strerror(errno));
    errno = 0;
  }
  printf("[client] connected\n");
  
  free(hints);
  freeaddrinfo(results);
  *to_server = sd;
  return sd;
}