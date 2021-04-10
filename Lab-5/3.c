#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <stdbool.h>

#define maxErr 1e-7

/* tokenTime array containing transmission time after packets passed through tokenBucket which is the arrival time of packet for fifo queue */

/* fifoTime keep track of transmission time of packets which has been sent bi fifo queue */
double aTime[100];		// containing arrivals time
int id[100];			// containing packet ID
int length[100];		// containing packet length
float tokenTime[100];
float* fifoTime;
int* pLength;			// keep track of length of packets which has been sent

// this function used to find the absolute value of any number
double modulus(double a)
{
	if(a < 0)
	{
		return (-1 * a);
	}
	return a;
}

void shape(int capacity, double rate)
{
	// filledBucket denotes, how much bucket is filled
	double filledBucket = 0;
	int i;
	
	// for first packet, if packet length is greater than capacity,
	// then it would take (length - capacity) time to transmit.
	// otherwise it would take 0 time.
	if(capacity < length[0])
	{
		tokenTime[0] = aTime[0] + (length[0]-capacity)/rate;
	}
	else
	{
		tokenTime[0] = aTime[0];
		filledBucket = capacity - length[0];
	}
	
	// for remaining 99 packets
	for(i=1; i<100; i++)
	{
		// if bucket capacity = 0, then time taken to transmit = length/rate
		// and at max{tTime(previous packet), aTime(current packet)} + length/rate
		// current packet will transmit
		if(capacity == 0)
		{
			if(tokenTime[i-1] > aTime[i])
			{
				tokenTime[i] = tokenTime[i-1] + length[i]/rate;
			}
			else
			{
				tokenTime[i] = tokenTime[i] + length[i]/rate;
			}
		}
		
		// if bucket capacity is not 0
		else
		{
			if(tokenTime[i-1] > aTime[i])
			{
				tokenTime[i] = tokenTime[i-1] + length[i]/rate;
			}
			else
			{
				// if previous packet is transmitted earlier than
				// arrival time of current packet,
				// then in between bucket will be filled by rate R
				filledBucket = filledBucket + (aTime[i] - tokenTime[i-1])*rate;
				if(filledBucket > length[i] & length[i] <= capacity)
				{
					tokenTime[i] = aTime[i];
					if(filledBucket > capacity)
					{
						filledBucket = capacity - length[i];
					}
					else
					{
						filledBucket = filledBucket - length[i];
					}
					
				}
				
				// after bucket being filled, tokens will fall down
				else if(filledBucket >= capacity & length[i] > capacity)
				{
					tokenTime[i] = aTime[i] + (length[i]-capacity)/rate;
					filledBucket = 0;
				}
				else
				{
					tokenTime[i] = aTime[i] + (length[i] - filledBucket)/rate;
				}
			}
		}
	}
	return;
}

// this function used to implement FIFO Queue
bool fifo(int capacity, double rate)
{
	// n denotes the how much packet is being transmitted
	// emptySpace denotes, how much space remaining in queue
	int n = 0;
	int emptySpace = capacity;
	
	// if length of first packet is less than queue buffer size,
	// then send it. Otherwise not. 
	if(length[0] < emptySpace)
	{
		
		fifoTime[0] = tokenTime[0] + length[0]/rate;
		n++;
		
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
			if(fifoTime[j] > tokenTime[i])
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
				fifoTime = realloc(fifoTime, (n+1)*sizeof(float));
				pLength = realloc(pLength, (n+1)*sizeof(int));
				
				// if whole queue is empty, then
				// tTime = aTime + length/rate
				if(emptySpace == capacity)
				{
					fifoTime[n] = tokenTime[i] + length[i]/rate;
				}
				
				// if another packet is also being sending, then
				// tTime(current) = tTime(previous packet) + length/rate
				else
				{
					fifoTime[n] = fifoTime[n-1] + length[i]/rate;
				}
				pLength[n] = length[i];
				n++;
				
				emptySpace = emptySpace - length[i];
			}
			
			// if no packet is being sent yet, then
			// tTime = aTime + length/rate
			else if(n == 0)
			{
				fifoTime[n] = tokenTime[i] + length[i]/rate;
				pLength[n] = length[i];
				n++;
				
				emptySpace = emptySpace - length[i];
			}
		}
	}
	
	// if all 100 packet is transmitted, i.e. there is no packet drop happened
	// so return true
	// otherwise some packet is dropped, so return false
	if(n == 100)
	{
		return true;
	}
	
	return false;
}

int main()
{
	int i;
	
	// scanning arrivals.txt and storing the data into respective array
	for(i=0; i<100; i++)
	{
		scanf("%lf %d %d", &aTime[i], &id[i], &length[i]);
	}
	
	int queueSize = 1000;
	double queueRate = 10.0;
	int bucketSize = 500;
	
	double start = 0.1, finish = 100.0, x = 0.1, mid;
	int itr = 0;
	
	// Using Binary Search till the error became less than 10^-7
	// if error is less than 10^-7 then break
	// or if error is > 10^-7 or some packets dropped then 
	// free the fifoTime and pLength array
	while(true)
	{
		fifoTime = (float*)calloc(1,sizeof(float));
		pLength = (int*)calloc(1,sizeof(int));
		mid = (start + finish)/2;
		
		shape(bucketSize, mid);
		bool result = fifo(queueSize, queueRate);
		
		if(result)
		{
			if(modulus(x - mid) <= maxErr)
				break;
			
			x = mid;
			start = x;
			free(fifoTime);
			free(pLength);
		}
		else
		{
			finish = mid;
			free(fifoTime);
			free(pLength);
		}
	}
	
	printf("Largest x = %0.6lf\n",x);
	
	return 0;
}
