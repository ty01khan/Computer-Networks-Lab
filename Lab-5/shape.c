#include <stdio.h>
#include <stdlib.h>
#include<string.h>

double aTime[100];		// containing arrivals time
int id[100];			// containing packet ID
int length[100];		// containing packet length
float tTime[100];	//keep track of transmission time of packets which has been sent

// this function use to implement token bucket based traffic shaping
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
		tTime[0] = aTime[0] + (length[0]-capacity)/rate;
	}
	else
	{
		tTime[0] = aTime[0];
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
			if(tTime[i-1] > aTime[i])
			{
				tTime[i] = tTime[i-1] + length[i]/rate;
			}
			else
			{
				tTime[i] = aTime[i] + length[i]/rate;
			}
		}
		
		// if bucket capacity is not 0
		else
		{
			if(tTime[i-1] > aTime[i])
			{
				tTime[i] = tTime[i-1] + length[i]/rate;
			}
			else
			{
				// if previous packet is transmitted earlier than
				// arrival time of current packet,
				// then in between bucket will be filled by rate R
				filledBucket = filledBucket + (aTime[i] - tTime[i-1])*rate;
				if(filledBucket > length[i] & length[i] <= capacity)
				{
					tTime[i] = aTime[i];
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
					tTime[i] = aTime[i] + (length[i]-capacity)/rate;
					filledBucket = 0;
				}
				else
				{
					tTime[i] = aTime[i] + (length[i] - filledBucket)/rate;
				}
			}
		}
	}
	
	for(i=0; i<100; i++)
	{
		printf("%.2f %d %d\n", tTime[i], id[i], length[i]);
	}
	
	return;
}

int main(int argc, char *argv[])
{
	double rate, filledBucket = 0;
	int capacity, i;
	
	// inputs capacity and rate
	sscanf(argv[1], "%d", &capacity);
	sscanf(argv[2], "%lf", &rate);
	
	// scanning arrivals.txt and storing the data into respective array
	for(i=0; i<100; i++)
	{
		scanf("%lf %d %d", &aTime[i], &id[i], &length[i]);
	}
	
	// calling shape function with argument capacity and rate
	shape(capacity, rate);
	
	return 0;
}
