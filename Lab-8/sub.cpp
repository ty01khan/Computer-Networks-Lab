#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

typedef long long ll;

const int MAXLEN = 1 << 10, PORT = 12501;     // SUB port
char send_m[MAXLEN], recv_m[MAXLEN], ip[20];

class UDPClient {
private:
  const int port;
  int sockfd, len, n;
  struct sockaddr_in servaddr;
  
public:
  UDPClient(int port) : port(port) {
    // creates a socket and returns a socket file descriptor
    // AF_INET -> IPv4
    // SOCK_DGRAM -> UDP
    // SOCK_STREAM -> TCP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("Failed to create the socket");
      exit(EXIT_FAILURE);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    
    // server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.4");  // h3 address
    servaddr.sin_port = htons(port);
  }

  void send_msg(char *buffer) {
    len = sizeof(servaddr);  //value result argument
    sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM,
	   (const struct sockaddr *) &servaddr, (socklen_t )len); 
  }
  
  void recv_msg(char *buffer) {
    n = recvfrom(sockfd, (char *)buffer, MAXLEN,  MSG_WAITALL,
		 (struct sockaddr *) &servaddr, (socklen_t *)&len); 
    buffer[n] = '\0';
    return;
  }
  ~UDPClient() { close(sockfd); }
  
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

  void client_addr(char *buffer) {
    // gets client ip address
    inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLEN);
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

  TCPServer compute(PORT);
  UDPClient result(9000);   // h3 port
  int port;
  ll num1, num2;
  while (true) {
    compute.accept_conn();
    compute.recv_msg(recv_m);
    compute.client_addr(ip);
    printf("Recieved numbers from %s\n", ip);
    sscanf(recv_m, "%d%lld%lld", &port, &num1, &num2);
    sprintf(send_m, "%s %d %lld", ip, port, num1 - num2);
    printf("Sent %lld to h3...\n", num1 - num2);
    result.send_msg(send_m);
  }
  return 0;
}
