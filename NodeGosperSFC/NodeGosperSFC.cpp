#include "stdafx.h"
#include "freeglut.h"
#include "NodeGosperSFC.h"

/*
NodeGosperSFC example

The following code shows an example using the NodeGosperSFC class.
Results are presented in the simple FreeGLUT visualization.
*/

NodeGosperSFC * sfc = 0; //NodeGosperSFC object
uint level; //Max. level of SFC recursion

/*GLUT Visualization parameters*/
int displaySizeX = 1024,  //Window size
	displaySizeY = 1024;
REAL scale; //Visualization scale
int colorLevel = 2; //Highlighted level of recursion
GLUquadric* q;
/*
GLUT display function
*/
void display(void);
/*
GLUT function starting the visualization

CTR - 1 = centered projection
*/
template<uint CTR> void RunGLUT(int argc, char ** argv);

int main(int argc, char ** argv)
{
	//Load 2D point cloud
	uint n = 100000; //Number of points
	level = 20; //Max. level of SFC recursion (Allowed: 0 - 20)
	PointCloud<2> * pc = new PointCloud<2>();
	if (!pc->LoadDataset("points.txt", n)) {
		delete pc;
		return 1;
	}
	
	/*
	//Construct the Node-Gosper SFC of the point cloud

	Available types:
	----------------------
	NodeGosperSFC_Center,
	NodeGosperSFC_Precise,
	NodeGosperSFC_Simple,
	NodeGosperSFC_Linear,
	NodeGosperSFC_Snake
	*/
	sfc = new NodeGosperSFC(level, pc, NodeGosperSFC_Precise);
	sfc->ConstructSFC();

	//Computation of visualization scale
	const BB *bb = sfc->GetBB();
	REAL bbs = (((bb->max.x - bb->min.x) > (bb->max.y - bb->min.y)) ? (bb->max.x - bb->min.x) : (bb->max.y - bb->min.y));
	scale = (displaySizeX / bbs) * 0.98;
	//Run visualization
	RunGLUT<1>(argc, argv);

	delete sfc;
	delete pc;

    return 0;
}

const Point3D colors[7] = { Point3D(0.f, 0.f, 1.f), Point3D(.0f, .8f, 0.f), Point3D(.3f, .8f, .95f), Point3D(1.f, 0.f, 1.f), Point3D(1.f, 0.f, 0.f), Point3D(1.f, .6f, 0.f), Point3D(0.f, 0.f, 0.f) };
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	glLineWidth(3);

	const BB * bb = sfc->GetBB();
	glScalef(scale, scale, 1.f);

	if (sfc) {
		Point3D color(0, 0, 0);
		const Point * p;
		glBegin(GL_LINE_STRIP);
		for (uint i = 0; i < sfc->GetPointNum(); i++) {
			p = sfc->GetSFCPoint(i);
			color = colors[(sfc->GetCodes()[i] >> 3 * (level - colorLevel - 1)) & 7];
			glColor3f(color.x, color.y, color.z);
			glVertex2f(p->x, p->y);
		}
		glEnd();
	}

	glPopMatrix();
	glFlush();
}

template<uint CTR> void RunGLUT(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(displaySizeX, displaySizeY);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Node-Gosper SFC");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (CTR) { //Centered projection
		glOrtho(-displaySizeX / 2, displaySizeX / 2, -displaySizeY / 2, displaySizeY / 2, -1.0, 1.0);
	}
	else {
		glOrtho(0., displaySizeX, 0., displaySizeY, -1.0, 1.0);
	}

	q = gluNewQuadric();
	glutDisplayFunc(display);
	glutMainLoop();
	gluDeleteQuadric(q);
}

