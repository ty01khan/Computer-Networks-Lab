#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Function Converting string into binary bits
void stringToBinary(char *S, int *bin, int n, int l)
{
	int i;
	for(i = 0; i < 8*l; i++)
	{
		bin[i] = 0 != (S[i/8] & 1 << (~i&7));
	}
	if(n != i)
	{
		while(i < n)
		{	
			bin[i] = 0;
			i++;
		}
	}
}

// Generation m+r bit code with redundant bit value = 0
void generatedCode(int *B, int* A, int m, int r)
{
	// A is storing frame, j is for redundant bit initialised as 0
	// k is for databit
	int j = 0, k = 0;
	for(int i = 0; i < m+r; i++)
	{
		if((i+1) == pow(2,j))
		{
			// if index == 2^n for n in (0,1,2...,r-1) then A[i] = 0
			// codeword[i] = 0 ---
			A[i] = 0;
			j++;
		}
		else
		{
			// else codeword[i] = databit[k]
			A[i] = B[k];
			k++;
		}
	}
}

// Finding redundant bit value
void checkBit(int* A, int m, int r)
{
	for(int i = 0; i < r; i++)
	{
		int x = pow(2,i);
		for(int j = 1; j <= m+r; j++)
		{
			if(((j >> i) & 1) == 1)
			{
				if(x != j)
				{
					A[x-1] = A[x-1] ^ A[j-1];
				}
			}
		}
	}
}

// finding value of r, r = min{(if 2^r >= m+r+1)}
int findr(int m)
{
	int r = 0;
	while(pow(2,r) < m+r+1)
	{
		r++;
	}
	return r;
}

int main(int argc, char *argv[])
{
	// m = data bit, r = redundant bit, l = length of input string, S = input string
	int m, r, l;
	
	l = strlen(argv[1]);	// first input
	char S[l];	
	strcpy(S,argv[1]);
	sscanf(argv[2], "%d", &m);	// second input
	
	// Only positive value of m is acceptable
	if(m <= 0)
	{
		printf("Please enter positive m\n");
		return 0;
	}
	
	r = findr(m);
	
	// bin = array which is storing bits corresponding to string
	// if (8*l) is not divisible by m, then increase it till divisible by m
	// size is the length of bits whose value is divisible by m
	int *bin, size;
	int rem = (8*l)%m;
	if(rem != 0)
		size = (((8 * l) / m) + 1) * m;
	else
		size = 8*l;
	
	bin = (int*)malloc(size*sizeof(int));
	
	int i;
	
	// input string is converted into bits
	stringToBinary(S, bin, size, l);
	for(i = 0; i < size; i++)
	{
		printf("%d", bin[i]);
	}
	printf("\nNo. of cheak bits = %d\n", r);
	
	// strPack storing the char bits of packet frame
	int A[m+r], B[m];
	char strPack[m+r+1];
	i = 0;
	
	// sending all packets frame to port no. 8567 of h2
	// AF_INET -> IPv4
	// SOCK_STREAM for TCP, SOCK_DGRAM for UDP
	// sockfd = file descriptor of socket to be binded

	int sockfd;
	struct sockaddr_in serverAddr;
	
	// Creating socket file descripter
	// 0 means use default protocol for the address family
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	
	// Server informations
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("192.168.1.2");
	serverAddr.sin_port = htons(8567);
	
	while(i < size)
	{
		B[i%m] = bin[i];
		if(i%m == m-1)
		{
			generatedCode(B, A, m, r);
			
			checkBit(A, m, r);
			
			printf("Sent frame with content ");

			for(int j = 0; j < m+r; j++)
			{
				strPack[j] = '0' + A[j];
				printf("%c",strPack[j]);
			}
			printf("\n");
			strPack[m+r] = '\0';
			
			// Sending packet to h2
			sendto(sockfd, (const char *)strPack, strlen(strPack), MSG_CONFIRM, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));
		}
		i++;
	}
	
	// sending an extra string to h2, 
	// to find where the previous packet end
	// (because sizes are different)
	char compare[r];
	for(int j = 0; j < r-1; j++)
	{
		compare[j] = '0';
	}
	compare[r] = '\0';
	sendto(sockfd, (const char *)compare, strlen(compare), MSG_CONFIRM, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));
	
	return 0;
}
