#include <iostream>
#include <cstring>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <netinet/ether.h>
#include <linux/if_ether.h>

using namespace std;

const int MAXLEN = 65536;

/*
--------------------
linux/if_ether.h
--------------------
*/

/*

#define ETH_ALEN        6               // Octets in one ethernet addr 
#define ETH_P_ALL       0x0003          // Every packet (be careful!!!)

struct ethhdr {
        unsigned char   h_dest[ETH_ALEN];       // destination eth addr 
        unsigned char   h_source[ETH_ALEN];     // source ether addr    
        __be16          h_proto;                // packet type ID field 
} __attribute__((packed));
*/

/*
--------------------
netinet/ip.h
--------------------
*/

/*
struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else

#endif
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
    //The options start here.
  };
*/

class Sniffer {
private:
  int sockfd;
public:
  Sniffer() {
    // creates a raw socket
    // can send or recieve at device driver (OSI Layer 2)
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
      perror("Failed to create socket");
      exit(EXIT_FAILURE);
    }
  }

  void sniff(unsigned char *buffer, int length) {
    // reads from the raw socket
    // not interested in source address
    int packet_sz = recvfrom(sockfd, buffer, length, 0, NULL, NULL);
    if (packet_sz < 0) {
      puts("Failed to get packets");
      exit(EXIT_FAILURE);
    }
  }
  //~Sniffer() { close(sockfd); }
};

void print_mac(unsigned char *mac) {
  
  for (int i = 0; i < ETH_ALEN; ++i)
    if (i != ETH_ALEN - 1)
      printf("%.2X-", mac[i]);
    else
      printf("%.2X\n", mac[i]);
  return;
}

int main() {

  unsigned char *buffer = new unsigned char[MAXLEN];
  Sniffer sniffer;

  // Structs that contain source IP addresses
  struct sockaddr_in src_addr, dest_addr;
  struct ethhdr *eth;
  struct iphdr *iph;

  int times = 50;
  int cnt = 100;
  while (true) {
    memset(buffer, 0, sizeof(buffer));
    memset(&src_addr, 0, sizeof(src_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    sniffer.sniff(buffer, MAXLEN);
    
    eth = (struct ethhdr *) buffer;

    // next header is ip header or not
    if (eth -> h_proto != ETH_P_IP)
      continue;
    
    iph = (struct iphdr *) (buffer + sizeof(struct ethhdr));
    src_addr.sin_addr.s_addr = iph -> saddr;
    dest_addr.sin_addr.s_addr = iph -> daddr;

    puts("\nPacket Received");
    puts("\nMAC Addresses\n");
    printf("\t|-Source Address : ");
    print_mac(eth -> h_source);
    printf("\t|-Destination Address : ");
    print_mac(eth -> h_dest);
    
    puts("\nIPv4 Addresses\n");
    printf("\t|-Source Address: %s\n", (char *)inet_ntoa(src_addr.sin_addr));
    printf("\t|-Destination Address: %s\n", (char *)inet_ntoa(dest_addr.sin_addr));
    
    for (int i = 0; i < times; ++i)
      putchar('-');
    puts("");
  }
  return 0;
}
