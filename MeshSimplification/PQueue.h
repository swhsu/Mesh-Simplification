#ifndef _P_QUEUE_
#define _P_QUEUE_

#include <vector>
#include "BasicType.h"

#define LEFT		-1
#define RIGHT		 1
#define BOTH		 0

using namespace std;

// Top-Min Priority Queue
class PQueue
{
private:
	vector<Edge*> queue;
	void bubbleUp(int startCell);
	void bubbleDown(int startCell);
	bool isParentBigger(int childCell);
	bool isLChildSmaller(int parentCell);
	bool isRChildSmaller(int parentCell);
	int  compareChild(int parentCell);

public:
	PQueue(void);
	~PQueue(void);

	int size();
	void enQueue(Edge* e);
	Edge* deQueue();
	Edge* top();
	Edge* remove(int cell);
};


#endif
