#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// frame containing recieved frame
// bin containing bits after correcting and deframing
// result contains character (converted from bits)

#define MAXLEN 1 << 20
char frame[MAXLEN];
int bin[MAXLEN], result[MAXLEN];

// Funcion correcting the 1-bit errors caused by h2
void correct_err(char* frame, int fSize)
{
	int checkxor = 0, bitxor, r;
	for (r = 30; r >= 0; r--)
    	if (fSize & (1 << r))
      		break;
  	r++;
	
	for(int i = 0; i < fSize; i++)
	{
		for(int j = r-1; j >= 0; j--)
		{
			int bit = (1 << j) & (i+1);
			if(!bit)
				continue;
			bitxor = frame[i] - '0';
			checkxor = checkxor ^ (bitxor << j);
		}
	}
	if(checkxor)
	{
		int bit = frame[checkxor-1] - '0';
		bit = 1-bit;
		frame[checkxor-1] = bit + '0';
	}
	return;
}

// Function converting the (m+r) bits corrected codes
// into m bits data bits and storing the data bits into
// bin array
int deframe(char *frame, int fSize, int binSize)
{
	for(int i = 0; i < fSize; i++)
	{
		// if power of 2
		if(((i+1) & i) == 0)
			continue;
		else
			bin[binSize] = frame[i] - '0';
			binSize++;
	}
	return binSize;
}

int main()
{
	// Receiving packet frame from h2 after 1-bit error occured
	int sockfd, len, n, binSize = 0, charBit = 8, idx = 0, cl, cnt = 0;
	struct sockaddr_in serverAddr, clientAddr;
	
	// Creating socket file descriptor
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	
	// server informations
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(9567);
	
	// Bind the socket with the server address
	if(bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("Bind Failed");
		exit(EXIT_FAILURE);
	}
	len = sizeof(clientAddr);
	
	// infinite loop
	printf("UDP server running on port 9567...\n");
	while (1)
	{
		// Recieving packet frame everytime
		n = recvfrom(sockfd, (char *)frame, MAXLEN, MSG_WAITALL, (struct sockaddr *) &clientAddr, &len);
		frame[n] = '\0';
		n = strlen(frame);
		
		if(cnt == 0)
		{
			cl = n;
			cnt = 1;
		}
		
		if(cl == n)
		{
			int i = 0;
		
			correct_err(frame, n);
			binSize = deframe(frame, n, binSize);
			
			while(binSize >= charBit)
			{
				int ascii = 0;
				for(int i = charBit - 1; i >= charBit - 8; i--)
				{
					ascii = ascii + (bin[i]*pow(2,charBit-i-1));
				}
				
				char ch = ascii;
				result[idx] = ch;
				idx++;
				charBit = charBit + 8;
			}
		}
		else
		{
			// if all the packet is sent then 
			// print the element of result array with newline
			// and initialize everyting again
			for(int i = 0; i < idx; i++)
			{
				printf("%c",result[i]);
			}
			printf("\n");
			
			binSize = 0;
			cnt = 0;
			idx = 0;
			cl = 0;
			charBit = 8;
			continue;
		}
	}
	return 0;
}
