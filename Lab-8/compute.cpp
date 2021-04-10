#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;


const int MAXLEN = 1 << 20, PORT = 8000; // TCP server port
char send_m[MAXLEN], recv_m[MAXLEN];

class TCPClient {
private:
  const int port;
  int sockfd, connfd;
  struct sockaddr_in servaddr, cliaddr;

public:
  TCPClient(int port) : port(port) {
    // socket creation
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Failed to create the socket");
      exit(EXIT_FAILURE);
    }
    bzero(&servaddr, sizeof(servaddr));

    // server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");  // h2 address
    servaddr.sin_port = htons(port);

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
        perror("Connection with the server failed...\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
        puts("Connected to the server...");
  }

  void send_msg(char *buffer) {
    write(sockfd, buffer, strlen(buffer));
  }

  void recv_msg(char *buffer) {
    read(sockfd, buffer, strlen(buffer));
  }

  ~TCPClient() { close(sockfd); }
};

class TCPServer {
private:
  const int port;
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cliaddr;

public:
  TCPServer(int port) : port(port) {
    
    // TCP scoket creation with IPv4
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      puts("Couldn't create a socket!");
      exit(EXIT_FAILURE);
    }
    
    bzero((void*)&servaddr, sizeof(servaddr));
    
    // assigns IP, port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // hostlong
    servaddr.sin_port = htons(port);                // hostshort
    
    // binds
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
	     (socklen_t)sizeof(servaddr)) != 0) {
      perror("Socket binding failed!");
      exit(EXIT_FAILURE);
    }
    // listens
    if (listen(sockfd, 5) != 0) {
      perror("Listen failed!");
      exit(EXIT_FAILURE);
    }
  }
  void accept_conn() {
    
    // accept connection
    len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t*)&len);

    if (connfd < 0) {
      perror("Server accept failed!");
      exit(EXIT_FAILURE);
    }
  }

  void send_msg(char *buffer) {
    write(connfd, buffer, strlen(buffer));
  }

  void recv_msg(char *buffer) {
    int n = read(connfd, buffer, MAXLEN);
    buffer[n] = '\0';
  }

  ~TCPServer() { close(sockfd); }
};


int main(int argc, char *argv[]) {

  if (argc != 4) {
    perror("Invalid number of arguments!\n");
    exit(EXIT_FAILURE);
  }
  sprintf(send_m, "%d ", PORT);
  strcat(send_m, argv[1]);
  strcat(send_m, " ");
  strcat(send_m, argv[2]);

  TCPServer result(PORT);
  int port;
  if (strcmp(argv[3], "ADD") == 0)
    port = 12500;
  else if (strcmp(argv[3], "SUB") == 0)
    port = 12501;
  else if (strcmp(argv[3], "MUL") == 0)
    port = 12502;
  else
    port = 12503;
  
  TCPClient comp(port);
  comp.send_msg(send_m);
  puts("Numbers sent...");
  result.accept_conn();
  result.recv_msg(recv_m);
  printf("Computed result is : %s\n", recv_m);
  return 0;
}
