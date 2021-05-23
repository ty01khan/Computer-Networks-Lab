#include <bits/stdc++.h>
#include <dirent.h>

using namespace std;

// IPv4 packet structure
typedef struct IPPacket_t {
	unsigned char v_hl;
	unsigned char dscp_ecn;
	unsigned short int totalLen;
	unsigned short int id;
	unsigned short int flags_frag_offset;
	unsigned char ttl;
	unsigned char proto;
	unsigned short int checksum;
	unsigned char sAddr[4];
	unsigned char dAddr[4];
	unsigned int o1;
	unsigned int o2;
	unsigned char data[1024];
} IPPacket;

int checkBits = 0xffff;
char directory[] = "ipfrags";
int cnt;	// kepps counts of packet legimate

// Checking for validation of checksum (if addition == 0xfff, then checksum is valid)
int isChecksumValid(char *byte)
{
	unsigned int checksum = 0;
	int size = ((*byte) & 0xf) << 2;	// header size of IPv4 fragments
	
	// 2 bytes = 16 bits, taking at a time for addition
	for(int i = 0; i < size; i = i+2)
	{
		// First 8-bits
		unsigned int temp = (*byte) & 0xff;
		byte++;
		
		// Another 8-bits
		temp = (temp << 8) + ((*byte) & 0xff);
		
		// Calculating checksum
		checksum += temp;
		byte++;
	}
	
	// Carry addition
	while(checksum >> 16)
	{
		unsigned int carry_bit = checksum >> 16;
		checksum = checksum & checkBits;
		checksum += carry_bit;
	}
	
	// if checksum == 0xffff, means "No Error"
	if(checksum == checkBits)
	{
		return 1;
	}
	return 0;
}

void header(IPPacket *packet)
{
	int len = (packet -> v_hl) & 0xf;
	
	printf("---------------------------------------------------------------------------------------\n");
	
	// Version
	printf("IP Version: %d | ", (packet -> v_hl) >> 4);
	
	// Header length
	printf("Header Length: %d byte words | ", len);
	
	// Type of Service
	printf("TOS (HEX): %x | ", packet -> dscp_ecn);
	
	// Total Length
	printf("Datagram Length: %d bytes\n", packet -> totalLen);
	printf("---------------------------------------------------------------------------------------\n");
	
	// Identification
	printf("Identifer: %d | ", packet -> id);
	
	// Flags
	printf("Flag (HEX): %x | ", (packet -> flags_frag_offset) >> 13);
	
	// Fragment Offset
	printf("Fragment Offset: %d\n", (packet -> flags_frag_offset) & 0x1fff);
	printf("------------------------------------------------------\n");
	// Time to Live
	printf("TTL: %d | ", packet -> ttl);
	
	// Protocol
	printf("Protocol(HEX): %x | ", packet -> proto);
	
	// Header checksum
	printf("Header Checksum (HEX): %x\n", packet -> checksum);
	printf("------------------------------------------------------\n");
	
	// Formatted Source IP Address
	printf("Source IP Addr : ");
	for(int i = 0; i < 4; i++)
	{
		printf("%d", (packet -> sAddr)[i]);
		if(i != 3)
		{
			putchar('.');
		}
	}
	printf("\n");
	printf("------------------------------------------------------\n");
	
	// Formatted Destination IP Address
	printf("Destination IP Addr : ");
	for(int i = 0; i < 4; i++)
	{
		printf("%d", (packet -> dAddr)[i]);
		if(i != 3)
		{
			putchar('.');
		}
	}
	printf("\n");
	printf("------------------------------------------------------\n");
	
	// Options
	if (len >= 6) 
	{
		// Option 1
		printf("Option 1 (HEX) : %x    ", packet -> o1);
		if (len >= 7)
		{
			// Option 2
			printf("Option 2 (HEX) : %x\n", packet -> o2);
		}
	}
	printf("------------------------------------------------------\n");
	puts(" ");
	return;
}

// Read packet from filename
void packetRead(char* filename)
{
	// Path = ipfrags/filename
	char path[50];
	strcpy(path, directory);
	strcat(path, "/");
	strcat(path, filename);
	FILE *fptr = fopen(path, "rb"); 	// binary read
	
	IPPacket packet;
	fread(&packet, sizeof(packet), 1, fptr);
	if(isChecksumValid((char *)&packet) == 1)
	{
		printf("Packet : %s\n", filename);
		header(&packet);
		cnt++;
	}
	fclose(fptr);
	return;
}

int main()
{
	// Directory List
	struct dirent *dir;
	DIR *d = opendir(directory);
	
	if(d)
	{
		while((dir = readdir(d)) != NULL)
		{
			if(strcmp(dir -> d_name, ".") == 0 || strcmp(dir -> d_name, "..") == 0)
			{
				continue;
			}
			
			// read packets and print its header if not corrupted
			packetRead(dir -> d_name);
		}
		closedir(d);
	}
	printf("Total IPv4 fragments legitimate = %d\n", cnt);
	return 0;
}
