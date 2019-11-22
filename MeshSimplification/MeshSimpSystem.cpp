
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <algorithm>
#include <vector>
#include "MeshSimpSystem.h"
#include "Timer.h"
#include <gl/glut.h>

MeshSimpSystem::MeshSimpSystem(void):pq()
{
	stopWatch timer;

	// init
	char* param1= new char[32];
	char* param2= new char[32];
	ifstream file;
	GetPrivateProfileString("filename", "filename", "bigguy2.obj", param1, 32, "./setting.ini");
	GetPrivateProfileString("numCollapse", "numCollapse", "20000", param2, 32, "./setting.ini");

	this->goal = atoi(param2);
	this->numCollapse = 0;

	// read data
	cerr << "reading file...";
	startTimer(&timer);
	readObj(param1);
	stopTimer(&timer);
	cerr << "read obj done (including build HaflEdge)." << endl;
	cerr << "time: " << getElapsedTime(&timer) << endl;

	// compute center of mass & boundary
	MAX.Set(-999, -999, -999);
	MIN.Set(999,999,999);
	COM.Set(0,0,0);
	for(int i=0; (unsigned)i<vertex.size(); i++) {
		COM+=vertex[i]->p;
		if(vertex[i]->p.x > MAX.x)
			MAX.x = vertex[i]->p.x;
		if(vertex[i]->p.y > MAX.y)
			MAX.y = vertex[i]->p.y;
		if(vertex[i]->p.z > MAX.z)
			MAX.z = vertex[i]->p.z;
		if(vertex[i]->p.x < MIN.x)
			MIN.x = vertex[i]->p.x;
		if(vertex[i]->p.y < MIN.y)
			MIN.y = vertex[i]->p.y;
		if(vertex[i]->p.z < MIN.z)
			MIN.z = vertex[i]->p.z;

	}
	COM/= vertex.size();

	// compute init QEF of all vertexs
	startTimer(&timer);
	computeAllVertexQEF();
	stopTimer(&timer);
	cerr << "compute QEF for each vertex done." << endl;
	cerr << "time: " << getElapsedTime(&timer) << endl;
	
	// do simplification
	startTimer(&timer);
	simplify();
	stopTimer(&timer);
	cerr << "mesh simplification done." << endl;
	cerr << "time: " << getElapsedTime(&timer) << endl;
}

MeshSimpSystem::~MeshSimpSystem(void)
{
}

void MeshSimpSystem::render()
{
	Point3f p[3];
	Point3f v1;
	Point3f v2;
	Point3f norm;
	
	glTranslatef(-COM.x, -COM.y, -COM.z);

	// draw model
	int numFace = face.size();
	for(int i=0;i<numFace;i++) {
		if(!face[i]->alive)
			continue;
		p[0] = face[i]->he->prev->origin->p;
		p[1] = face[i]->he->origin->p;
		p[2] = face[i]->he->next->origin->p;

 		glBegin(GL_TRIANGLES);
		// calculate normal
		v1 = p[1]-p[0];
		v2 = p[2]-p[1];
		norm = ((p[1]-p[0]).Cross(p[2]-p[1])).GetNormalized();

		glNormal3f(norm.x, norm.y, norm.z);
		for(int j=0;j<3;j++) {
			glVertex3f(p[j].x, p[j].y, p[j].z);
		}
		glEnd();
	}
}

void MeshSimpSystem::readObj(string filename)
{
	ifstream file;
	file.open(filename.c_str());
	if( !file.is_open() ) {
		cerr << "read obj file error..." << endl;
		exit(0);
	}

	string line;
	char type;
	int id[3];
	HalfEdge* he_[3];
	pair<int, int> key;
	EdgeTable::iterator itr;
	int vID=0;

	while(getline(file, line) && (line.length() != 0)) {
		stringstream ss(line);
		ss >> type;

		if(type == 'v') {
			Vertex* v = new Vertex();
			ss >> v->p.x;
			ss >> v->p.y;
			ss >> v->p.z;

			vertex.push_back(v);
		}
		else if(type == 'f') {
			ss >> id[0];
			ss >> id[1];
			ss >> id[2];

			Face* f = new Face();

			// add three half edge to face
			he_[0] = new HalfEdge();
			he_[1] = new HalfEdge();
			he_[2] = new HalfEdge();

			he_[0]->prev = he_[2];
			he_[1]->prev = he_[0];
			he_[2]->prev = he_[1];
			
			he_[0]->next = he_[1];
			he_[1]->next = he_[2];
			he_[2]->next = he_[0];

			he_[0]->face  = f;
			he_[1]->face  = f;
			he_[2]->face  = f;

			he_[0]->origin = vertex[id[0]-1];			// "-1" cause vertex index starts from 1 in obj format
			he_[1]->origin = vertex[id[1]-1];
			he_[2]->origin = vertex[id[2]-1];
			
			vertex[id[0]-1]->he = he_[0];
			vertex[id[1]-1]->he = he_[1];
			vertex[id[2]-1]->he = he_[2];

			// flip for he[0]
			key = (id[0] > id[1])? make_pair(id[0], id[1]) : make_pair(id[1], id[0]);
			itr = edgeTable.find(key);
			if(itr != edgeTable.end()) {
				he_[0]->flip = itr->second;
				itr->second->flip = he_[0];
				Edge* e = new Edge();
				e->he = he_[0];
				he_[0]->edge = e;
				he_[0]->flip->edge = e;
				edge.push_back(e);
			} 
			else {
				edgeTable.insert(make_pair(key, he_[0]));				
			}
			
			// flip for he[1]
			key = (id[1] > id[2])? make_pair(id[1], id[2]) : make_pair(id[2], id[1]);
			itr = edgeTable.find(key);
			if(itr != edgeTable.end()) {
				he_[1]->flip = itr->second;
				itr->second->flip = he_[1];
				Edge* e = new Edge();
				e->he = he_[1];
				he_[1]->edge = e;
				he_[1]->flip->edge = e;
				edge.push_back(e);
			} 
			else {
				edgeTable.insert(make_pair(key, he_[1]));	
			}
			
			// flip for he[2]
			key = (id[2] > id[0])? make_pair(id[2], id[0]) : make_pair(id[0], id[2]);
			itr = edgeTable.find(key);
			if(itr != edgeTable.end()) {
				he_[2]->flip = itr->second;
				itr->second->flip = he_[2];
				Edge* e = new Edge();
				e->he = he_[2];
				he_[2]->edge = e;
				he_[2]->flip->edge = e;
				edge.push_back(e);
			} 
			else {
				edgeTable.insert(make_pair(key, he_[2]));
			}

			f->he = he_[0];
			face.push_back(f);
		}
		else {
			// skip
		}
	}
	file.close();
}


void MeshSimpSystem::computeVertexQEF(Vertex* v)
{
	HalfEdge* finder;
	HalfEdge* first;
	Point3f neig_;
	Point3f sumVi(0,0,0);
	float sumVTV=0;
	int n=0;

	// visit neighbor and accum value		
	first = v->he;
	finder = v->he;
	do {
		finder = finder ->prev;
		neig_ = finder->origin->p;			
		sumVi += neig_;
		sumVTV += neig_.Dot(neig_);
		n++;

		finder = finder->flip;			
	} while (finder != first);
	
	// save value to QEF
	v->QEF[0] = n;
	v->QEF[1] = sumVi.x;
	v->QEF[2] = sumVi.y;
	v->QEF[3] = sumVi.z;
	v->QEF[4] = sumVTV;
}

void MeshSimpSystem::computeAllVertexQEF()
{
	int numVertex = vertex.size();

	for(int i=0; i<numVertex; i++) {
		computeVertexQEF(vertex[i]);
	}
}

void MeshSimpSystem::computeEdgeQEF(Edge* e)
{
	HalfEdge* he = e->he;
	Point3f v;
	Point3f sumVi;
	int numEdge = edge.size();
	int n;
	float sumVTV;

	// combine QEF
	for(int j=0; j<5; j++)
		e->QEF[j] = he->next->origin->QEF[j] + he->origin->QEF[j];

	// compute Error
	n = e->QEF[0];
	sumVi.Set(e->QEF[1], e->QEF[2], e->QEF[3]);
	sumVTV = e->QEF[4];
	v = sumVi/n;
	//err = n*(v.Dot(v)) - 2*(v.Dot(sumVi)) + sumVTV;
	e->err = -sumVi.Dot(sumVi)/n + sumVTV;
}

void MeshSimpSystem::simplify()
{
	int numContinuouslyUnsafe=0;
	int numVertex=0, numEdge = 0, numFace=0;

	// compute QEF for edges
	numEdge = edge.size();
	for(int i=0; i<numEdge; i++) {
		computeEdgeQEF(edge[i]);
		pq.enQueue(edge[i]);
	}

	// all edge in pq is alive
	Edge* target = pq.top();
	while(target != NULL && numCollapse < goal) {
		if( collapse(target)) {							// success
			numCollapse++;
			target = pq.top();								// collapse next edge
			numContinuouslyUnsafe=0;
		}
		else {															// failed
			target = pq.deQueue();
			target->err = target->err+50;			// change err value
			pq.enQueue(target);								// put it back
			numContinuouslyUnsafe++;
		}

		if(numContinuouslyUnsafe > 50) {
			cerr << "can not collapse more..." << endl;
			break;
		}
	}

	// print mesh info
	numVertex = 0; numEdge = 0; numFace = 0;
	for(int i=0; (unsigned)i<vertex.size(); i++)
		if(vertex[i]->alive)
			numVertex++;
	for(int i=0; (unsigned)i<edge.size(); i++)
		if(edge[i]->alive)
			numEdge++;
	for(int i=0; (unsigned)i<face.size(); i++)
		if(face[i]->alive)
			numFace++;
	cerr << "=====================================" << endl;
	cerr << "befor simplify..." << endl;
	cerr << "\t v size: "<< vertex.size() << endl;
	cerr << "\t e size: "<< edge.size()   << endl;
	cerr << "\t f size: "<< face.size()   << endl;
	cerr << "after simplify..." << endl;
	cerr << "\t v size: "<< numVertex << endl;
	cerr << "\t e size: "<< numEdge   << endl;
	cerr << "\t f size: "<< numFace   << endl;	


}

bool MeshSimpSystem::collapse(Edge *e)
{
	HalfEdge* he = e->he;																				
	Vertex* const v0 = he->origin;															// point 0 of collapse edge
	Vertex* const v1 = he->next->origin;												// point 1 of collapse edge
	Vertex* const v2 = he->prev->origin;												// share point 0 of collapse edge
	Vertex* const v3 = he->flip->prev->origin;									// share point 1 of collapse edge
	HalfEdge* a0 = he->next->flip;															
	HalfEdge* a1 = he->prev->flip;															
	HalfEdge* b0 = he->flip->prev->flip;												
	HalfEdge* b1 = he->flip->next->flip;												
	HalfEdge* finder;									
	HalfEdge* first;
	int numShareVertex=0;	

	// check if it's safe to collapse
	// ============================================================================ 0
	// 1. find all neighbors of v0, put them in to a set
	set<Vertex*> common;
	first = he->prev;
	finder = he->prev;
	
	do {
		common.insert(finder->origin);
		finder = finder->flip->prev;
	} while (finder != first );

	// 2. count the common neighbors of v0 and v1
	first = he->flip->prev;
	finder = he->flip->prev;
	do {
		if( common.find(finder->origin) != common.end() )
			numShareVertex++;
		finder = finder->flip->prev;
	} while (finder != first );	

	if(numShareVertex != 2) {				// not safe
		return false;
	}
	// ============================================================================ 1


	// take all neighbor edges of v0 and v1 from pq
	// ============================================================================ 0
	// v0
	first = he->prev;
	finder = he->prev;
	
	do {
		pq.remove(finder->edge->pqIndex);
		finder = finder->flip->prev;
	} while (finder != first );

	// v1
	first = he->flip->prev;
	finder = he->flip->prev;
	do {
		if(finder->edge->pqIndex > 0)
			pq.remove(finder->edge->pqIndex);
		finder = finder->flip->prev;
	} while (finder != first );	
	// ============================================================================ 1
	

	// delete 2 faces, 3 edges, and 1 vertex
	// ============================================================================ 0
	he->face->alive = false;
	he->face->he = NULL;
	he->flip->face->alive = false;
	he->flip->face->he = NULL;
	he->edge->alive = false;
	he->next->edge->alive = false;
	he->flip->prev->edge->alive = false;
	v1->alive = false;
	// ============================================================================ 1

	// update vertex's half edge
	// ============================================================================ 0
	v0->he = he->prev->flip;								
	v2->he = he->next->flip;								
	v3->he = he->flip->next->flip;					
	// ============================================================================ 1


	// update half edge's vertex
	// ============================================================================ 0
	// deleted vertex has "some" out going half edges. 
	// update their origin to another vertex.
	// notice: although some out going half edges will be deleted,
	// but for easy coding, I will update the origin for all of them.
	finder = he->next;
	first = he->next;
	do {
		finder->origin = v0;
		finder = finder->flip->next;
	} while( finder != first);
	// ============================================================================ 1


	// update edge's half edge
	// ============================================================================ 0
	he->prev->edge->he = he->prev->flip;								// LU edge.he -> 0
	he->flip->next->edge->he = he->flip->next->flip;		// LD edge.he -> 3
	// ============================================================================ 1


	// update half edge's edge
	// ============================================================================ 0
	a0->edge = a1->edge;										// 15.edge = 0.edge
	b0->edge = b1->edge;										// 12.edge = 3.edges
	// ============================================================================ 1


	// update flips
	// ============================================================================ 0
	a0->flip = a1;													// 0.flip->15
	a1->flip = a0;													// 15.flip->0
	b0->flip = b1;													// 12.flip->3
	b1->flip = b0;													// 3.flip->12
	// ============================================================================ 1


	// update position
	// ============================================================================ 0
	// 1. recompute QEF of v0 -> get a new position
	// 2. move to that position
	for(int i=0; i<5; i++)
		v0->QEF[i] = he->edge->QEF[i];
	v0->p.Set(v0->QEF[1]/v0->QEF[0], v0->QEF[2]/v0->QEF[0], v0->QEF[3]/v0->QEF[0]);
	// ============================================================================ 1


	// update edge QEF of v0
	// ============================================================================ 0
	// v0
	first = v0->he->prev;
	finder = v0->he->prev;
	
	do {
		computeEdgeQEF(finder->edge);
		pq.enQueue(finder->edge);
		finder = finder->flip->prev;
	} while (finder != first );
	// ============================================================================ 1
	
	return true;
}
