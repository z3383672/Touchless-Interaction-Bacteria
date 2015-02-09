
#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<glut.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#include"SOIL.h"
#include"glext.h"

// Window initial size and position.

// Reset grid to rectangle.
void dtResetGrid();

// Load DomeTwister grid
bool dtLoadGrid(char *fn);
void LoadTextures(void) ;


GLuint LoadTexture(int frame);

void renderBitmapString(float x, float y, float z, void *font, char *string);

// DomeTwister Display function.
void dtDisplay(int ,bool,double,double,bool,double,double,double,double,double,bool);

// DomeTwister Resize function
void dtResize(int w, int h);

// DomeTwister Idle function.
void dtIdle(void);

// DomeTwister Keyboard function.
void dtKeyboard(unsigned char key, int x, int y);

// Finds closest dot to a mouse click.
void findSection(int x, int y);

// DomeTwister Mouse function.
void dtMouse(int button, int state, int x, int y);

void dtMotion(int x, int y);
void Motion(int x,int y);

void DisplayText(double Xselect,double Yselect,double Orientation,double Major,double Minor);



//int main(int argc, char **argv)
//{
	/*int argcc = 1;
	char *argvc[] = {"dometwister", NULL};

	glutInit(&argcc, argvc);
	glutInitWindowSize(windowSizeX, windowSizeY);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("Dome Twister version 1.0");

	// Glut functions.
	glutIdleFunc(dtIdle);
	glutKeyboardFunc(dtKeyboard);
	glutDisplayFunc(dtDisplay);
	glutMouseFunc(dtMouse);
	glutMotionFunc(dtMotion);
	glutReshapeFunc(dtResize);

	glEnable(GL_MAP2_TEXTURE_COORD_2);   
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);

	texture = LoadTexture("1 (1).bmp");

	//mesh = (warping_mesh*)malloc(gridSizeX*gridSizeY*sizeof(warping_mesh));
	//dtResetGrid();
	//dtLoadGrid("dometwist.dat");

	//glutMainLoop();*/

