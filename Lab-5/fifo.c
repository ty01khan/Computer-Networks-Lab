#include <stdio.h>
#include <stdlib.h>
#include<string.h>

double aTime[100];	// containing arrivals time
int id[100];		// containing packet ID
int length[100];	// containing packet length
int* pLength;		// keep track of length of packets which has been sent
float* tTime;	// keep track of transmission time of packets which has been sent

// this function used to implement FIFO Queue
void fifo(int capacity, double rate)
{
	// n denotes the how much packet is being transmitted
	// emptySpace denotes, how much space remaining in queue
	int n = 0;
	int emptySpace = capacity;
	
	// if length of first packet is less than queue buffer size,
	// then send it. Otherwise not. 
	if(length[0] < emptySpace)
	{
		
		tTime[0] = aTime[0] + length[0]/rate;
		pLength[0] = length[0];
		n++;
		
		printf("%.2f %d %d\n", tTime[0], id[0], length[0]);
		
		// updating empty space in queue
		emptySpace = emptySpace - length[0];
	}
	
	// checking for remaining 99 packets
	for(int i=1; i<100; i++)
	{
		// checking for how much space is remaining in queue
		// from the packet which has been sent and which are being sending 
		emptySpace = capacity;
		for(int j = n-1; j >= 0; j--)
		{
			//if packet is in queue, then emptySpace = capacity - length(packet)
			if(tTime[j] > aTime[i])
			{
				emptySpace = emptySpace - pLength[j];
			}
			else
			{
				break;
			}
		}
		
		// if queue has space for packet
		if(length[i] <= emptySpace)
		{
			// if some packet is being sent
			if(n > 0)
			{	
				tTime = realloc(tTime, (n+1)*sizeof(float));
				pLength = realloc(pLength, (n+1)*sizeof(int));
				
				// if whole queue is empty, then
				// tTime = aTime + length/rate
				if(emptySpace == capacity)
				{
					tTime[n] = aTime[i] + length[i]/rate;
				}
				
				// if another packet is also being sending, then
				// tTime(current) = tTime(previous packet) + length/rate
				else
				{
					tTime[n] = tTime[n-1] + length[i]/rate;
				}
				pLength[n] = length[i];
				n++;
				
				printf("%.2f %d %d\n", tTime[n-1], id[i], length[i]);
				
				emptySpace = emptySpace - length[i];
			}
			
			// if no packet is being sent yet, then
			// tTime = aTime + length/rate
			else if(n == 0)
			{
				tTime[n] = aTime[i] + length[i]/rate;
				pLength[n] = length[i];
				n++;
				
				printf("%.2f %d %d\n", tTime[n-1], id[i], length[i]);
				
				emptySpace = emptySpace - length[i];
			}
		}
	}
	
	return;
}

int main(int argc, char *argv[])
{
	double rate;
	int capacity, i;
	
	// inputs capacity and rate
	sscanf(argv[1], "%d", &capacity);
	sscanf(argv[2], "%lf", &rate);
	
	// scanning arrivals.txt and storing the data into respective array
	for(i=0; i<100; i++)
	{
		scanf("%lf %d %d", &aTime[i], &id[i], &length[i]);
	}
	
	// allocating 1 unit space in both arrays
	tTime = (float*)calloc(1,sizeof(float));
	pLength = (int*)calloc(1,sizeof(int));
	
	// calling fifo function with argument capacity and rate
	fifo(capacity, rate);
	
	return 0;
}
