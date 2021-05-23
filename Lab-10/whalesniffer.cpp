#include <iostream>
#include <cstring>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
//#include <netinet/ether.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

using namespace std;

const int MAXLEN = 65536;

/*
--------------------
net/if.h
--------------------
*/

/*

# define ifr_name       ifr_ifrn.ifrn_name      /* interface name  
# define ifr_ifindex    ifr_ifru.ifru_ivalue    /* interface index   

struct ifreq {
# define IFHWADDRLEN    6
# define IFNAMSIZ       IF_NAMESIZE
    union
      {
        char ifrn_name[IFNAMSIZ];       /* Interface name, e.g. "en0". 
      } ifr_ifrn;
      ...

*/
      
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
linux/if_packet.h
--------------------
*/

/*
struct sockaddr_ll {
        unsigned short  sll_family;
        __be16          sll_protocol;
        int             sll_ifindex;
        unsigned short  sll_hatype;
        unsigned char   sll_pkttype;
        unsigned char   sll_halen;
        unsigned char   sll_addr[8];
};
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
  struct sockaddr_ll sockaddr;
  struct ifreq if_idx;
  bool iany;
  
  Sniffer(char *interface) {
    // creates a raw socket
    // can send or recieve at device driver (OSI Layer 2)
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd < 0) {
      perror("Failed to create socket");
      exit(EXIT_FAILURE);
    }

    iany = false;
    memset(&if_idx, 0, sizeof(if_idx));
    strncpy(if_idx.ifr_name, interface, IFNAMSIZ - 1);

    if (strcmp(interface, "any") == 0)
      iany = true;
    else if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
      perror("SIOCGIFINDEX");

    //cout << if_idx.ifr_ifindex << endl;
  }

  void sniff(unsigned char *buffer, int length) {
    // reads from the raw socket
    int len = sizeof(sockaddr);
    int packet_sz = recvfrom(sockfd, buffer, length, 0,
			     (struct sockaddr *)&sockaddr, (socklen_t *)&len);
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


int main(int argc, char *argv[]) {

  if (argc < 3) {
    puts("No interface provided");
    return 0;
  }

  if (strcmp(argv[1], "-i")) {
    puts("No interface provided");
    return 0;
  }
  unsigned char *buffer = new unsigned char[MAXLEN];
  Sniffer sniffer(argv[2]);

  // Structs that contain source IP addresses
  struct sockaddr_in src_addr, dest_addr;
  struct ethhdr *eth;
  struct iphdr *iph;

  int times = 50;
  int cnt = 5;

  cout << sniffer.if_idx.ifr_ifindex << endl;

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    memset(&src_addr, 0, sizeof(src_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    sniffer.sniff(buffer, MAXLEN);

    // cout << sniffer.sockaddr.sll_ifindex << endl;
    if (sniffer.iany == false)
      if (sniffer.sockaddr.sll_ifindex != sniffer.if_idx.ifr_ifindex )
	continue;
    
    eth = (struct ethhdr *) buffer;

    // next header is ip header or not
    //if (eth -> h_proto != ETH_P_IP)
    //  continue;
    
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
