#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;
const int SERVERS = 4, MAXLEN = 100;
int ports[] = {12500, 12501, 12502, 12503};
char *progs[] = {(char *)"./inetd_add", (char *)"./inetd_sub",
		 (char *)"./inetd_mul", (char *)"./inetd_idiv"}; 
vector <int> sockfds;
char ip[MAXLEN];

int services(int port) {

  int sockfd;
  struct sockaddr_in servaddr;
  
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
  return sockfd;
}

int accept_conn(int sockfd) {

  struct sockaddr_in cliaddr;
  // accept connection
  int len = sizeof(cliaddr);
  int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t*)&len);
  inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLEN);
  
  if (connfd < 0) {
    perror("Server accept failed!");
    exit(EXIT_FAILURE);
  }

  return connfd;
}

// spawns the respective server
void server_processing(int sockfd, int i) {

  int connfd = accept_conn(sockfd);
  if (fork() == 0) {
    // child process
    for (int fd : sockfds)
      if (fd != sockfd)
	close(fd);
    // server can read through STDIN
    dup2(connfd, 0);
    char *arg[] = {progs[i], ip, NULL};
    
    if (execv(arg[0], arg) < 0)
      perror("execv():");
  }
}

int main() {

  fd_set rfds;
  struct timeval tv;

  int maxfd = -1;
  // listens for all 4 servers
  for (int i = 0; i < SERVERS; ++i) {
    int sockfd = services(ports[i]);
    maxfd = max(maxfd, sockfd);
    sockfds.push_back(sockfd);
    //cout << sockfd << endl;
  }
  // Wait up to five seconds;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  while (true) {

    FD_ZERO(&rfds);
    for (int fd : sockfds)
      FD_SET(fd, &rfds);

    int ret = select(maxfd + 2, &rfds, NULL, NULL, &tv);
    if (ret == -1)
      perror("select()");
    else if (ret) {
      int sockfd, i = 0;
      for (int fd : sockfds) {
	if (FD_ISSET(fd, &rfds) == 1) {
	  sockfd = fd;
	  break;
	}
	i++;
      }
      server_processing(sockfd, i);
    }
  }
  return 0;
}
