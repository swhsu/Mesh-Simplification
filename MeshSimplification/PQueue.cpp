
#include "PQueue.h"

PQueue::PQueue(void)
{
	// array based priority queue implementation
	// make the queue start from index 1
	Edge* head = new Edge();
	queue.clear();
	queue.push_back(head);
}

PQueue::~PQueue(void)
{
}

int PQueue::size()
{
	return queue.size()-1;
}

Edge* PQueue::top()
{
	if(size() == 0)
		return NULL;
	else
		return queue[1];
}

void PQueue::enQueue(Edge *e)
{
	queue.push_back(e);
	
	// mark
	e->pqIndex = size();

	bubbleUp(queue.size()-1);
}

Edge* PQueue::deQueue()
{
	if(size() == 0)
		return NULL;
	else {
		Edge* min = queue[1];

		if(size()>1) {
			Edge* lastElem = queue[queue.size()-1];
			queue.pop_back(); // delete last elem
			queue[1] = lastElem;
			
			// mark
			lastElem->pqIndex = 1;
			
			// heapify
			bubbleDown(1);

		} else {
			queue.erase(queue.begin()+1);
		}

		// mark
		min->pqIndex = -1;

		return min;
	}
}

void PQueue::bubbleUp(int cell)
{
	while(isParentBigger(cell)) {
		// prepare for swap
		Edge* parent = queue[cell/2];
		Edge* child  = queue[cell];

		// swap
		queue[cell/2] = child;
		queue[cell]   = parent;

		// mark
		child->pqIndex = cell/2;
		parent->pqIndex = cell;

		// go up one level
		cell /= 2;
	}
}

void PQueue::bubbleDown(int cell)
{
	while(isLChildSmaller(cell) || isRChildSmaller(cell)) {
		
		int child = compareChild(cell);

		if(child==LEFT) {
			// prepare swap
			Edge* parent = queue[cell];
			Edge* lChild = queue[2*cell];
			
			// swap
			queue[cell]   = lChild;
			queue[2*cell] = parent;
			
			// mark
			lChild->pqIndex = cell;
			parent->pqIndex = 2*cell;

			// go down one level
			cell = 2*cell;
		} else if (child == RIGHT) {
			// prepare swap
			Edge* parent = queue[cell];
			Edge* rChild = queue[2*cell+1];
			
			// swap
			queue[cell]   = rChild;
			queue[2*cell+1] = parent;

			// mark
			rChild->pqIndex = cell;
			parent->pqIndex = 2*cell+1;

			// go down one level
			cell = 2*cell+1;
		}
	}
}

bool PQueue::isParentBigger(int childCell)
{
	if( childCell == 1 )		// top, no parent
		return false;
	else
		return queue[childCell/2]->err > queue[childCell]->err;
}

bool PQueue::isLChildSmaller(int parentCell)
{
	if( (unsigned)(parentCell*2) >= queue.size())
		return false;	// no child
	else
		return queue[2*parentCell]->err < queue[parentCell]->err;
}

bool PQueue::isRChildSmaller(int parentCell)
{
	if( (unsigned)(parentCell*2+1) >= queue.size())
		return false;	// no child
	else
		return queue[2*parentCell+1]->err < queue[parentCell]->err;
}

// check which child is smaller
int PQueue::compareChild(int parentCell)
{
	bool lChildSmaller = isLChildSmaller(parentCell);
	bool rChildSmaller = isRChildSmaller(parentCell);

	if(lChildSmaller || rChildSmaller) {
		// chick which one is smaller
		if(lChildSmaller && rChildSmaller) {
			int lChild = 2*parentCell;
			int rChild = 2*parentCell+1;
			Edge* left  = queue[lChild];
			Edge* right = queue[rChild];
			
			if(left->err <= right->err)
				return LEFT;
			else
				return RIGHT;
		} else if (lChildSmaller)
			return LEFT;
		else
			return RIGHT;
	}
	return BOTH;
}

Edge* PQueue::remove(int cell)
{
	if(size() == 0)
		return NULL;
	else {
		Edge* target = queue[cell];
		if(size()>cell) {
			Edge* lastElem = queue[queue.size()-1];
			queue.pop_back(); // delete last elem
			queue[cell] = lastElem;

			// mark
			lastElem->pqIndex = cell;

			// heapify
			bubbleDown(cell);

		} else {
			queue.erase(queue.begin()+cell);
		}

		// mark
		target->pqIndex = -1;

		return target;
	}
}


