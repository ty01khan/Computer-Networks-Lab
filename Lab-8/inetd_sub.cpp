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
char temp[MAXLEN], ip[20];

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

int main(int argc, char *argv[]) {

  // gets h1 ip address
  strcpy(ip, argv[1]);
  UDPClient result(9000);   // h3 port
  int port;
  ll num1, num2;
  // read from the STDIN (not actually)
  int n = read(0, temp, MAXLEN);
  temp[n] = '\0';
  sscanf(temp, "%d%lld%lld", &port, &num1, &num2);
  sprintf(temp, "%s %d %lld", ip, port, num1 - num2);
  printf("Sent %lld to h3...\n", num1 - num2);
  result.send_msg(temp);
  return 0;
}
