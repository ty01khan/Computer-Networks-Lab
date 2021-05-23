#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

typedef long long ll;

const int MAXLEN = 1 << 10;
char recv_m[MAXLEN], send_m[MAXLEN];

class UDPServer {
private:
  const int port;
  int sockfd, len, n;
  struct sockaddr_in servaddr, cliaddr;
public:
  UDPServer(int port) : port(port) {
    // creates a socket and returns a socket file descriptor
    // AF_INET -> IPv4
    // SOCK_DGRAM -> UDP
    // SOCK_STREAM -> TCP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("Failed to create the socket");
      exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    // server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;    // localhost
    servaddr.sin_port = htons(port);
    
    // binds the socket with the address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
      perror("Failed to bind");
      exit(EXIT_FAILURE);
    }
  }

  void send_msg(char *buffer) {
    sendto(sockfd, (const char *)buffer, strlen(buffer),  
	   MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
	   (socklen_t )len);  
  }

  void recv_msg(char *buffer) {
    len = sizeof(cliaddr);  //value result argument 
    n = recvfrom(sockfd, (char *)buffer, MAXLEN,  MSG_WAITALL,
		 (struct sockaddr *) &cliaddr, (socklen_t *)&len); 
    buffer[n] = '\0'; 
    return;
  }

  ~UDPServer() { close(sockfd); }
};

class TCPClient {
private:
  const int port;
  int sockfd, connfd;
  struct sockaddr_in servaddr, cliaddr;
  char ip_serv[20];

public:
  TCPClient(int port, char *ip) : port(port) {
    // socket creation
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Failed to create the socket");
      exit(EXIT_FAILURE);
    }

    strcpy(ip_serv, ip);
    bzero(&servaddr, sizeof(servaddr));

    // server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip_serv);  // h1 address
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


int main() {

  ll sq;
  char ip[20];
  int port;
  UDPServer serv(9000);
  while (true) {
    serv.recv_msg(recv_m);
    sscanf(recv_m, "%s%d%lld", ip, &port, &sq);
    sq = sq * sq;
    TCPClient ret(port, ip);
    sprintf(send_m, "%lld", sq);
    ret.send_msg(send_m);
    printf("%lld sent to %s:%d\n", sq, ip, port);
  }
  return 0;
}
