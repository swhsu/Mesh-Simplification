
#include <gl/glut.h>
#include <iostream>
#include "LightMat.h"
#include "MeshSimpSystem.h"

using namespace std;

// Set screen size
#define ImageW 640
#define ImageH 480

// Frames per second
#define FRAMERATE 60.0

// Set Camera Control parameters
float zoomResolution  = 0.05;
float transResolution = 0.05;
float rotResolution   = 0.5;
float zoom = 10.0f;
float rotx = 0;
float roty = 0;
float tx = 0;
float ty = 0;
int lastx=0;
int lasty=0;
unsigned char Buttons[3] = {0};

bool displayMode=1;
MeshSimpSystem* meshSimpSystem;


void Motion(int x,int y)
{
	int diffx=x-lastx;
	int diffy=y-lasty;
	lastx=x;
	lasty=y;

	if( Buttons[2] )
		zoom -= (float) zoomResolution * diffy;
	else {
		if( Buttons[0] ) {
			rotx += (float) rotResolution * diffy;
			roty += (float) rotResolution * diffx;		
		} else {
			if( Buttons[1] ) {
				tx += (float) transResolution * diffx;
				ty -= (float) transResolution * diffy;
			}
		}
	}

	glutPostRedisplay();
}

void Mouse(int b,int s,int x,int y)
{
	lastx=x;
	lasty=y;
	switch(b) {
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN==s)?1:0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN==s)?1:0);
		break;
	case GLUT_RIGHT_BUTTON:
		Buttons[2] = ((GLUT_DOWN==s)?1:0);
		break;
	default:
		break;		
	}

	glutPostRedisplay();
}


void Keyboard(unsigned char key, int x, int y) 
{
	switch(key) {
		case ' ':	// space
			if(displayMode == 1)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			displayMode ^= 1;
			
			glutPostRedisplay();
			break;
	}
}

void reshape(int w, int h)
{
	// prevent divide by 0 error when minimised
	if(w==0) 
		h = 1;

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(float)w/h,0.01,1500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// update the camera
	glTranslatef(0,0,-zoom);
	glTranslatef(tx,ty,0);
	glRotatef(rotx,1,0,0);
	glRotatef(roty,0,1,0);	

	glColor3f(1, 0, 1);
	meshSimpSystem->render();
	
	glutSwapBuffers();
}

void lightInit()
{

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);	

	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);	
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);	

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_low_shininess);
}

void glInit() 
{
	glClearColor(0.9, 0.9, 0.9, 1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	lightInit();
}

void meshSimpSystemInit()
{
	meshSimpSystem = new MeshSimpSystem();
	float len = (meshSimpSystem->MAX - meshSimpSystem->MIN).Length()/ImageH;
	zoomResolution  = len;
	transResolution = len;
	rotResolution   = 0.5;
	zoom = 10*meshSimpSystem->MAX.z;
}

int main(int argc, char** argv) 
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(ImageW,ImageH);
	glutInitWindowPosition(300,100);
	glutCreateWindow("[HW4]Mesh Simplification: Shu-Wei Hsu");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glInit();
	meshSimpSystemInit();

	glutMainLoop();
	return 0;
}
