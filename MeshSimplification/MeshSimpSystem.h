
#ifndef _MESH_SIMP_SYSTEM_
#define _MESH_SIMP_SYSTEM_

#include <vector>
#include <map>
#include "BasicType.h"
#include "PQueue.h"
#include "cyPoint.h"

using namespace std;
using namespace cy;

#define DEBUG

typedef map< pair<int,int>, HalfEdge*> EdgeTable;

class MeshSimpSystem
{
public:
	MeshSimpSystem(void);
	~MeshSimpSystem(void);
	void readObj(string filename);
	void render();
	void computeVertexQEF(Vertex* v);
	void computeAllVertexQEF();
	void computeEdgeQEF(Edge* e);
	void simplify();
	bool collapse(Edge* edge);

	EdgeTable edgeTable;			// for flip
	PQueue pq;
	vector<Vertex*> vertex;
	vector<Face*> face;
	vector<Edge*> edge;
	int goal;
	int numCollapse;
	Point3f COM;							// center of mass;
	Point3f MAX;							// AABB boundary
	Point3f MIN;							// AABB boundary
};

#endif

