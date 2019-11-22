#ifndef _BASIC_TYPE_H_
#define _BASIC_TYPE_H_

#include "cyPoint.h"

using namespace cy;

typedef struct HalfEdge HalfEdge;
typedef struct Face Face;
typedef struct Vertex Vertex;
typedef struct Edge Edge;

typedef struct HalfEdge{
	HalfEdge* next;
	HalfEdge* prev;
	HalfEdge* flip;
	Face* face;
	Edge* edge;
	Vertex* origin;
} HalfEdge;

typedef struct Face{
	bool alive;
	HalfEdge* he;
	Face() {alive = true;};
} Face;

typedef struct Vertex{
	bool alive;
	HalfEdge* he;
	Point3f p;
	float QEF[5];
	Vertex() {alive = true;};
} Vertex;

typedef struct Edge{
	int pqIndex;
	bool alive;
	float QEF[5];
	float err;
	HalfEdge* he;
	Edge() { alive = true; };
} Edge;


#endif
