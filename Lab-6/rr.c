#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// defines packet
typedef struct Packet 
{
  double aTime;
  int pId, qId, length;
  struct Packet *next;
} Packet;

// defines queue
typedef struct Queue 
{
  Packet *head, *tail;
} Queue;

Packet packet[10000];
int qId;

int max(int a, int b)
{
	if(a > b)
	{
		return a;
	}
	return b;
}

// returns true if the queue is empty.
// otherwise false
bool isEmpty(Queue *queue) 
{
  if (queue -> head == NULL)
    return true;
  return false;
}

void enQueue(Queue *queue, Packet *packet) 
{
  if (queue -> head == NULL) 
  {
    queue -> head = queue -> tail = packet;
    return;
  }
  queue -> tail -> next = packet;
  queue -> tail = packet;
  return;
}

void deQueue(Queue *queue) 
{
  if (queue -> head == NULL)
    return;
  if (queue -> head == queue -> tail) 
  {
    queue -> head = queue -> tail = NULL;
    return;
  }
  queue -> head = queue -> head -> next;
  return;
}

// returns head element
Packet *top(Queue *queue) 
{
  return queue -> head;
}

// simulates round robin

// may not give same result as a real one.
// it is due to the order in which the queue
// will be selected when all queues becomes empty.
void rr(double rate) 
{
  // creates queues
  Queue *queue = (Queue *)malloc(4 * sizeof(Queue));

  for (int i = 0; i < 4; ++i)
    queue[i].head = queue[i].tail = NULL;

  // curHost -> current queue number from where the packet will be transmitted
  // allEmpty -> tells whether all queues are empty or not
  // tTime -> transmission time for the latest transmitted packet
  int curHost = 0, pId, qId, i = 0;
  bool allEmpty = true;
  float tTime = 0.0, aTime;

  // iterates through all packet
  while (i < 10000) 
  {
    aTime = packet[i].aTime;
    // inserts packet which came before the
    // latest transmitted packet transmission
    // if all queue are empty, it inserts packet
    if ((aTime < tTime) || allEmpty) 
    {
      qId = packet[i].qId - 1;
      enQueue(queue + qId, packet + i);
      i++;
      if (allEmpty) 
      {
		tTime = aTime;
		allEmpty = false;
      }
      continue;
    }

    // round robin
    // follows the order curHost -> .. -> 4 -> .. -> curHost - 1 (previous)
    for (int h = 0; h < 4; ++h) 
    {
      // if the queue has packet, it transmits the packet
      // next queue will be checked in later iteration
      if (isEmpty(queue + curHost) == false) 
      {
		Packet *packet = top(queue + curHost);
		tTime = tTime + (packet -> length) / rate;
		pId = packet -> pId;
		deQueue(queue + curHost);
		printf("%.2lf %d\n", tTime, pId);
		curHost = (curHost + 1) % 4;
		break;
      }
      curHost = (curHost + 1) % 4;
      if (h == 4 - 1)
		allEmpty = true;
    }
  }
  // transmits the packet in the queue
  // breaks when all queues are empty
  while (allEmpty == false) 
  {
    allEmpty = true;
    
    // round robin
    for (int h = 0; h < 4; ++h) 
    {
      if (isEmpty(queue + curHost) == false) 
      {
		Packet *packet = top(queue + curHost);
		tTime = tTime + (packet -> length) / rate;
		pId = packet -> pId;
		deQueue(queue + curHost);
		printf("%.2lf %d\n", tTime, pId);
		allEmpty = false;
      }
      curHost = (curHost + 1) % 4;
    }
  }
  free(queue);
  return;
}


int main(int argc, char *argv[]) 
{
  double sRate, aTime;
  int pId, qId, length, n = 0;
  sscanf(argv[1], "%lf", &sRate);
  
  while (scanf("%lf%d%d%d", &aTime, &pId, &qId, &length) != EOF) 
  {
    packet[n].aTime = aTime;
    packet[n].pId = pId;
    packet[n].qId = qId;
    packet[n].length = length;
    n++;
  }
  rr(sRate);
  return 0;
}
