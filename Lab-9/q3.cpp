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

// pID is storing ID in 0th index and packet number in the remaining index
// offCnt keeps count of number of packet with respective ID
// countPack keeps number of different id's in legitimate packet
int pID[10][50], cntPack = 0, offCnt[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
char msg[1024];	// collects data
IPPacket packets[100];	// stores all legitimate packets

// to compare on the basis of offset of packet
int compare(int a, int b)
{
	int offsetA = packets[a].flags_frag_offset & 0x1fff;
	int offsetB = packets[b].flags_frag_offset & 0x1fff;
	
	return (offsetA < offsetB ? 1 : 0);
}

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

// Reading packet + collecting id's and counting IPv4 packets
void packetRead(char* filename)
{
	// Path = ipfrags/filename
	char path[50];
	strcpy(path, directory);
	strcat(path, "/");
	strcat(path, filename);
	FILE *fptr = fopen(path, "rb"); 	// binary read
	
	fread(packets + cnt, sizeof(packets[cnt]), 1, fptr);
	if(isChecksumValid((char *)(packets + cnt)) == 1)
	{
		int pid = packets[cnt].id;
		
		int i = 0;
		for(i = 0; i < cntPack; i++)
		{
			// If ID is not occuring for the first time
			if(pID[i][0] == pid)
			{
				pID[i][offCnt[i]] = cnt;	// appending packet number in the respective id
				offCnt[i]++;
				break;
			}
		}
		// If ID is occuring for the first time
		if(i == cntPack)
		{
			pID[cntPack][0] = pid;
			pID[cntPack][offCnt[cntPack]] = cnt;	// appending packet number in the respective id
			offCnt[cntPack]++;
			cntPack++;
		}
		cnt++;
	}
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
			
			// read packets
			packetRead(dir -> d_name);
		}
		closedir(d);
	}
	
	printf("Total IPv4 packets = %d\n\n", cntPack);
	
	// Sorting packets on the basis of offset
	for(int i = 0; i < cntPack; i++)
	{
		sort(pID[i]+1, pID[i]+offCnt[i], compare);
	}
	
	// Assembling
	for(int i = 0; i < cntPack; i++)
	{
		// packSize = size of packets
		int packSize = 0, h = 0;
		msg[0] = '\0';
		printf("IPv4 Packet ID = %d\n", pID[i][0]);
		for(int j = 1; j < offCnt[i]; j++)
		{
			// header length and data length
			int hedrLen = (packets[pID[i][j]].v_hl) & 0xf;
			hedrLen = hedrLen << 2;
			int dataLen = packets[pID[i][j]].totalLen - hedrLen;
			
			packSize += dataLen;
			packets[pID[i][j]].data[dataLen] = '\0';
			strcat(msg, (const char*)packets[pID[i][j]].data);
			if(h == 0)
				packSize += hedrLen;
			h = 1;
		}
		printf("IPv4 packet size = %d\n", packSize);
		printf("%s\n\n", msg);
	}
	
	return 0;
}
