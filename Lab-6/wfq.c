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
double qw[4], tTime;

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

/// chooses the minimum virtual finish time packet and transmits it
// gives preference to lowest index queue when virtual finish time
// is same
// It returns true if all queues becomes empty

bool transmission(Queue *queue, double rate) 
{
  int selectQueue = -1;
  bool allEmpty = false;
  double minVirt, virtTime;
  // looks into the head packet of each queues
  for (int h = 0; h < 4; ++h) 
  {
    if (isEmpty(queue + h) == false) 
    {
      Packet *packet = top(queue + h);
      virtTime = packet -> aTime + (packet -> length) / qw[h];
      if (selectQueue == -1) 
      {
		selectQueue = h;
		minVirt = virtTime;
      }
      else if (virtTime < minVirt) 
      {
		minVirt = virtTime;
		selectQueue = h;
      }
    }
  }

  // head packet from the selected queue is transmitted
  if (selectQueue != -1) 
  {
    Packet *packet = top(queue + selectQueue);
    tTime = tTime + (packet -> length) / rate;
    int pId = packet -> pId;
    deQueue(queue + selectQueue);
    // updates the aTime time
    // Here aTime time = max(aTime time, previous packet finish time)
    if (isEmpty(queue + selectQueue) == false) 
    {
      Packet *packet = top(queue + selectQueue);
      packet -> aTime = tTime;
    }
    printf("%.2lf %d %d\n", tTime, pId, selectQueue + 1);
  }
  else
    allEmpty = true;
  
  return allEmpty;
}

// simulates the weighted fair queue
void wfq(double rate) 
{
  // creates the queues
  Queue *queue = (Queue *)malloc(4 * sizeof(Queue));

  for (int i = 0; i < 4; ++i)
    queue[i].head = queue[i].tail = NULL;

  int selectQueue = -1, pId, qId, i = 0;
  bool allEmpty = true;
  double aTime;
  tTime = 0.0;
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
    allEmpty = transmission(queue, rate);
  }
  // transmitted the remaining packet in queues
  while (allEmpty == false)
    allEmpty = transmission(queue, rate);
  free(queue);
  return;
}

int main(int argc, char *argv[]) 
{
  double sRate, aTime;
  int pId, qId, length, n = 0;
  sscanf(argv[1], "%lf", &sRate);
  
  for (int i = 2; i <= 5; ++i)
    sscanf(argv[i], "%lf", qw + i - 2);
    
  while (scanf("%lf%d%d%d", &aTime, &pId, &qId, &length) != EOF) 
  {
    packet[n].aTime = aTime;
    packet[n].pId = pId;
    packet[n].qId = qId;
    packet[n].length = length;
    n++;
  }
  wfq(sRate);
  return 0;
}
