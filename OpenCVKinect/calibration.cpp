// Dome Twister, TB June 2009
// Dome Twister Beziers, 2009.07.13
#include"calibration.h"
#include<windows.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<time.h>



using namespace std;
int windowSizeX = 1024;//1440; //960;
int windowSizeY = 512;//810; //540;
int windowPosX = 100;
int windowPosY = 50;
#define PI 3.14159265

// Mouse coordinates
float mouseX;
float mouseY;
// Left Mouse button State
int leftButtonState = 0;

// Grid size.
int gridSizeX = 9;
int gridSizeY = 7;

typedef struct {
	float x, y;
	float u, v;
	int selectedMode; // selection Mode, 0 - unselected, 1 selected 
} warping_mesh;
warping_mesh *mesh;


// Is grid visible? 'g' key.
bool bGridInvisible = false;
int gridMode = 1;

// Warping mesh.

int selectedDot;

int selectedDotX;
int selectedDotY;

// Fullscreen mode switch.
bool bFullScreenMode = false;

// Anti aliasing
bool bAntialiasing = false;

// Font.
int font = (int)GLUT_BITMAP_HELVETICA_12;

// Texture.

int textureSizeX, textureSizeY;

char *texture_file;
char *movie_file; // from argv
bool isMovieFileFromArgv = false;

bool isMovie= false;
double XSelect;
double YSelect;
double ORientation;
double MAjor;
double MInor;


// bilinear helper matrix for texturing
GLfloat grid2x2[2][2][3] =
{
	{ {-0.0, -0.5, 0.0},
	{1.0, -0.5, 0.0}
	},
	{ {-1.0, 1.0, 0.0},
	{1.0, 1.0, 0.0}
	}
};
GLfloat *grid = &grid2x2[0][0][0];
int uSize = 2;
int vSize = 2;
GLfloat texpts[2][2][2] = {{{0.0, 0.0}, {0.0, 1.0}}, {{1.0, 0.0}, {1.0, 1.0}}};
/* More complex 4nd order initial grid.  16 points (4 by 4). */

GLfloat grid4x4[4][4][3] =
{
	{
		{-1.0, -1.0, 0.0},
		{-0.33, -1.0, 0.0},
		{0.33, -1.0, 0.0},
		{1.0, -1.0, 0.0}
	},
	{
		{-1.0, -0.33, 0.0},
		{-0.33, -0.33, 0.0},
		{0.33, -0.33, 0.0},
		{1.0, -0.33, 0.0}
	},
	{
		{-1.0, 0.33, 0.0},
		{-0.33, 0.33, 0.0},
		{0.33, 0.33, 0.0},
		{1.0, 0.33, 0.0}
	},
	{
		{-1.0, 1.0, 0.0},
		{-0.33, 1.0, 0.0},
		{0.33, 1.0, 0.0},
		{1.0, 1.0, 0.0}
	}
};
GLfloat texpts4x4[4][4][2] = 
{
	{
		{0.0, 0.0},
		{0.33, 0.0},
		{0.66, 0.0},
		{1.0, 0.0}
	},
	{
		{0, 0.33},
		{0.33, 0.33},
		{0.66, 0.33},
		{1.0, 0.33}
	},
	{
		{0.0, 0.66},
		{0.33, 0.66},
		{0.66, 0.66},
		{1.0, 0.66}
	},
	{
		{0.0, 1.0},
		{0.33, 1.0},
		{0.66, 1.0},
		{1.0, 1.0}
	}
};
bool grid4x4selected[4][4];
int gridSize = 60;
GLuint Taxi;
bool isSymmetricalMode = false;
GLuint textureid[20]; 
GLuint textureidoriginal[20];
GLint textureWidth=1024;
GLint textureHeight=512;
unsigned char *bufferwithline=(unsigned char*) malloc(textureWidth * textureHeight*4);
#define VIEWING_DISTANCE_MIN  3.0
GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
GLuint fb;
GLuint fr;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
PFNGLWINDOWPOS2IPROC glWindowPos2i;
void dtmvGetOpenGLFunctionPointers(void)
{	
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffersEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glCheckFramebufferStatusEXT	= (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glGenRenderbuffersEXT=(PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");
	glBindRenderbufferEXT=(PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");
	glRenderbufferStorageEXT=(PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glFramebufferRenderbufferEXT=(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glDeleteFramebuffersEXT=(PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glWindowPos2i=(PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
} 


GLuint dtmvOffscreenTexture(int sx, int sy)
{

	dtmvGetOpenGLFunctionPointers();
	glEnable(GL_TEXTURE_2D);
	GLuint	txtnumber;
	glGenTextures(1, &txtnumber);
    glBindTexture(GL_TEXTURE_2D, txtnumber);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glGenFramebuffersEXT(  1, &fb );
	glGenRenderbuffersEXT(1,&fr);
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb );
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, txtnumber, 0);
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, fr );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 1024, 512 );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fr );
	glViewport(0,0,1024,512);
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status==GL_FRAMEBUFFER_COMPLETE_EXT)
      cout<<"good";
   else
      cout<<"Wrong";//HANDLE_THE_ERROR;
	glDisable(GL_TEXTURE_2D);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return txtnumber;
}


// Reset grid to rectangle.
void dtResetGrid()
{
	mesh = (warping_mesh*)malloc(gridSizeX*gridSizeY*sizeof(warping_mesh));
	if (gridMode < 2)
	{
		for (int j=0; j<gridSizeY; j++) {
			for (int i=0; i<gridSizeX; i++) {
				mesh[i+j*gridSizeX].x = -1.0f+i*(2.0f/(gridSizeX-1));
				mesh[i+j*gridSizeX].y = -1.0f+j*(2.0f/(gridSizeY-1));
				mesh[i+j*gridSizeX].u = (float)i/(float)(gridSizeX-1);
				mesh[i+j*gridSizeX].v = /*1.0f-*/(float)j/(float)(gridSizeY-1);
				mesh[i+j*gridSizeX].selectedMode = 0;
			}
		}
	}
	if (gridMode == 2)
	{
		for (int j=0; j<4; j++) {
			for (int i=0; i<4; i++) {
				grid4x4[i][j][1] = (float)2*i/3-1.0f;
				grid4x4[i][j][0] = (float)2*j/3-1.0f;
			}
		}	
	}
}

// Load DomeTwister grid
bool dtLoadGrid(char *fn)
{
	FILE *fp;

	//if (fp = fopen(fn,"rt")) 
	fopen_s(&fp, fn, "rt");
	if (fp)
	{
		fscanf_s(fp, "dometwistmode %d\n", &gridMode);

		if (gridMode < 2)
		{
			fscanf_s(fp, "%d %d\n", &gridSizeX, &gridSizeY);
			for (int i=0;i<gridSizeX;i++) {
				for (int j=0;j<gridSizeY;j++) {
					fscanf_s(fp, "%f %f %f %f\n", &mesh[i+j*gridSizeX].x, &mesh[i+j*gridSizeX].y,  &mesh[i+j*gridSizeX].u, &mesh[i+j*gridSizeX].v);
				}
			}
		}
		if (gridMode == 2)
		{
			fscanf_s(fp, "%d\n", &gridSize);
			for (int j=0;j<4;j++) {
				for (int i=0;i<4;i++) {
					fscanf_s(fp, "%f %f %f %f\n", &grid4x4[i][j][0], &grid4x4[i][j][1], &texpts4x4[i][j][0], &texpts4x4[i][j][1]);
				}
			}
		}
		fclose(fp);
		return true;
	} else return false;
}

void LoadTextures(void) 
{
	//GLuint *textureid; 
	string s,th;
	glEnable(GL_TEXTURE_2D);
	for(int frame=1;frame<=20;frame++)
	{
		s=std::to_string(static_cast<long long>(frame));
		th="image/1 ("+s+").jpg";
		int width=1024;
		int height=512;
		textureid[frame-1]=SOIL_load_OGL_texture(th.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y|SOIL_FLAG_NTSC_SAFE_RGB|SOIL_FLAG_COMPRESS_TO_DXT);
		textureidoriginal[frame-1]=SOIL_load_OGL_texture(th.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y|SOIL_FLAG_NTSC_SAFE_RGB|SOIL_FLAG_COMPRESS_TO_DXT);
		glBindTexture(GL_TEXTURE_2D, textureid[frame-1]); //bind the texture to it's array
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glBindTexture(GL_TEXTURE_2D, textureidoriginal[frame-1]); //bind the texture to it's array
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}
	std::cout<<"Initilization successfull";
	glDisable(GL_TEXTURE_2D);
	Taxi=dtmvOffscreenTexture(1024,512);
	
}

// DomeTwister Display function.
void dtDisplay(int frame,bool pointer,double X, double Y,bool oval,double Xselect,double Yselect, double Major,double Minor,double Orientation,bool DrawInformation)
{
	int heighscreen=810;
    int widescreen=1440;
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//GLfloat texpts4x4t[4][4][2];
	//gluPerspective(10, 1920/1080, 0.1, 100.0);
	glBindTexture(GL_TEXTURE_2D,textureidoriginal[frame-1] ); //bind our texture to our shape
	Xselect=(Xselect/widescreen)*1024;
	Yselect=(Yselect/heighscreen)*512;
	
	GLuint size = textureWidth * textureHeight;
	unsigned char *buffer;
	buffer = (unsigned char*) malloc(textureWidth * textureHeight*4);
	
	if(pointer==false)
	{

		glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 4 * 3, 4, (GLfloat*)grid4x4);
		glEnable(GL_MAP2_VERTEX_3);
		glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 4, 0, 1, 4*2, 4, &texpts4x4[0][0][0]);
		glEnable(GL_MAP2_TEXTURE_COORD_2); 
		glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
		glEvalMesh2(GL_FILL, 0, gridSize, 0, gridSize); // 20 is GridSize
		glutSwapBuffers();	
	}
	else if(pointer==true)
				{
					glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
					glBindTexture(GL_TEXTURE_2D,textureid[frame-1] );
					X=(X/widescreen)*1024;
					Y=(Y/heighscreen)*512;
					for(int y = 0; y < textureHeight; y++)
						for(int x = 0; x < textureWidth; x++)
							{
								int startAddressOfPixel = ((y*textureWidth) + x)*4;
								if(sqrt((x-X)*(x-X)+(y-Y)*(y-Y)) < 5){
									buffer[startAddressOfPixel+0]=0;
									buffer[startAddressOfPixel+1]=255;
									buffer[startAddressOfPixel+2]=0;
									buffer[startAddressOfPixel+3]=255;
								}
							}
			if(oval==true)
					{
						int num=0;
						glBindTexture(GL_TEXTURE_2D,textureidoriginal[frame-1] );
						glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
						glBindTexture(GL_TEXTURE_2D,textureid[frame-1] );
						for(int y = Yselect-Major; y < Yselect+Major; y++)
							for(int x = Xselect-Major; x < Xselect+Major; x++)
							{
								double xp=x-Xselect;
								double yp=y-Yselect;
								double xpp=xp*cos(Orientation*PI/180)+yp*sin(Orientation*PI/180);
								double ypp=-xp*(sin(Orientation*PI/180))+yp*cos(Orientation*PI/180);
								int startAddressOfPixel = ((y*textureWidth) + x)*4;
								if(((xpp/(Major/2.0))*(xpp/(Major/2.0))+((ypp)/(Minor/2.0))*((ypp)/(Minor/2.0))) <=1)
								{
									buffer[startAddressOfPixel+0]=255;
								}
							}
							double x_begin=Xselect-(Major/2.0)*cos(Orientation*PI/180);
							double x_end=Xselect+(Major/2.0)*cos(Orientation*PI/180);
							double y_end=Yselect+(Major/2.0)*sin(Orientation*PI/180);
							double y_begin=Yselect-(Major/2.0)*sin(Orientation*PI/180);
							for(int x = ceil(x_begin) ; x < ceil(x_end) ; x++)
								{
									double y=y_begin+((y_end-y_begin)/(x_end-x_begin))*(x-x_begin);
									int startAddressOfPixel = ((ceil(y)*textureWidth) + x)*4;
									buffer[startAddressOfPixel+0]=255;
									buffer[startAddressOfPixel+1]=255;
									buffer[startAddressOfPixel+2]=255;
									buffer[startAddressOfPixel+3]=255;
								}
					}
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
			if(oval==true)
				{
					glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bufferwithline);
					glBindTexture(GL_TEXTURE_2D,textureid[frame-1] );
					glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, bufferwithline );
				}
				
				glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 4 * 3, 4, (GLfloat*)grid4x4);
				glEnable(GL_MAP2_VERTEX_3);
				glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 4, 0, 1, 4*2, 4, &texpts4x4[0][0][0]);
				glEnable(GL_MAP2_TEXTURE_COORD_2); 
				glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
				glEvalMesh2(GL_FILL, 0, gridSize, 0, gridSize); // 20 is GridSize
				glutSwapBuffers();
			}
			if(DrawInformation==true)
					{
						glBindTexture(GL_TEXTURE_2D,textureid[frame-1] );
						glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, bufferwithline );
						if(Xselect !=0.0 && Yselect!=0)
						{
						  double XX=(Xselect/1024.0);
						  double YY=(Yselect/512.0);
						  glPushMatrix();
						  int w;
						  int h;
						  GLdouble size;
						  GLdouble aspect;
						  w = 1480;
						  h = 810;
							for(int k=0;k<80;k++)
							{
								GLint m_viewport[4];

								glGetIntegerv( GL_VIEWPORT, m_viewport );

								glClearColor(0.0, 0.0, 0.0, 0.0);
								glClearDepth(1.0);
								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
								GLfloat texpts4x4t[4][4][2] = 
											{
												{
													{XX-0.5+k/1000.0, YY-0.27135+k/1000.0},
													{XX-0.5+0.33+k/1000.0, YY-0.27135+k/1000.0},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.27135+k/1000.0},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.27135+k/1000.0}
												},
												{
													{XX-0.5+k/1000.0, YY-0.27135+k/1000.0+0.1824},
													{XX-0.5+0.33+k/1000.0, YY-0.27135+k/1000.0+0.1824},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.27135+k/1000.0+0.1824},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.27135+k/1000.0+0.1824}
												},
												{
													{XX-0.5+k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824},
													{XX-0.5+0.33+k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824}
												},
												{
													{XX-0.5+k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824+0.1824},
													{XX-0.5+0.33+k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824+0.1824},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824+0.1824},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.27135-k/1000.0+0.1824+0.1824+0.1824}
												}
									};
								glPopMatrix();
								glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 4 * 3, 4, (GLfloat*)grid4x4);
								glEnable(GL_MAP2_VERTEX_3);
								glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 4, 0, 1, 4*2, 4, &texpts4x4t[0][0][0]);
								glEnable(GL_MAP2_TEXTURE_COORD_2); 
								glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
								glEvalMesh2(GL_FILL, 0, gridSize, 0, gridSize); // 20 is GridSize
								//glFlush();
								glutSwapBuffers();				
							}
							
						}
				}
	free(buffer);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}


void dtMotion(int x, int y)
{
	//object.motion( x, y );

	if (leftButtonState == 1) 
	{
		float mx, my;

		// Convert mouse position to proper coordinate.
		if (bFullScreenMode) {
			mx = x/(0.5f*glutGet(GLUT_SCREEN_WIDTH))-1.0f;
			my = -(y/(0.5f*glutGet(GLUT_SCREEN_HEIGHT))-1.0f);
		} else {
			mx = x/(0.5f*windowSizeX)-1.0f;
			my = -(y/(0.5f*windowSizeY)-1.0f);
		}

		if (gridMode < 2)
		{
			mesh[selectedDot].x = mx;
			mesh[selectedDot].y = my;
		}
		if (gridMode == 2)
		{
			grid4x4[selectedDotX][selectedDotY][0] = mx;
			grid4x4[selectedDotX][selectedDotY][1] = my;

			if (isSymmetricalMode)
			{
				grid4x4[selectedDotX][3-selectedDotY][0] = -mx;
				grid4x4[selectedDotX][3-selectedDotY][1] = my;
			}

		}
		mouseX = mx;
		mouseY = my;
	}
}


void Motion(int x, int y)
{
	double XX=(x/1024.0);
	double YY=(y/512.0);
	int k=79;
	glEnable(GL_TEXTURE_2D);
	GLfloat texpts4x4t[4][4][2] = 
								{
												{
													{XX-0.5+k/1000.0, YY-0.3125+k/1000.0},
													{XX-0.5+0.33+k/1000.0, YY-0.3125+k/1000.0},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.3125+k/1000.0},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.3125+k/1000.0}
												},
												{
													{XX-0.5+k/1000.0, YY-0.3125+k/1000.0+0.20833},
													{XX-0.5+0.33+k/1000.0, YY-0.3125+k/1000.0+0.20833},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.3125+k/1000.0+0.20833},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.3125+k/1000.0+0.20833}
												},
												{
													{XX-0.5+k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833},
													{XX-0.5+0.33+k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833}
												},
												{
													{XX-0.5+k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833+0.20833},
													{XX-0.5+0.33+k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833+0.20833},
													{XX-0.5+0.33+0.33-k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833+0.20833},
													{XX-0.5+0.33+0.33+0.33-k/1000.0, YY-0.3125-k/1000.0+0.20833+0.20833+0.20833}
												}
									};
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 4 * 3, 4, (GLfloat*)grid4x4);
	glEnable(GL_MAP2_VERTEX_3);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 4, 0, 1, 4*2, 4, &texpts4x4t[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2); 
	glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, gridSize, 0, gridSize); // 20 is GridSize
	glutSwapBuffers();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// DomeTwister Resize function
void dtResize(int w, int h)
{
	std::cout<<"I am called";
	const float ar = (float)w/float(h);
	windowSizeX = w;
	windowSizeY = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// DomeTwister Idle function.
void dtIdle(void)
{
	glutPostRedisplay();
}

// DomeTwister Keyboard function.
void dtKeyboard(unsigned char key, int x, int y)
{
	int i, j;
	FILE *fp;

 	switch(key)
	{
	case 27:
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'f':
	case 'F':
		bFullScreenMode = !bFullScreenMode;
		if (bFullScreenMode) 
		{
			glutFullScreen();
		}
		else
		{
			glutReshapeWindow(windowSizeX, windowSizeY); 
			glutPositionWindow(windowPosX, windowPosY);
		}
		break;  	
	case 'g':
	case 'G':
		bGridInvisible = !bGridInvisible;
		break;
	case 'a':
	case 'A':
		if (bAntialiasing) {
			bAntialiasing = false;
			glDisable(GL_BLEND);
			glDisable(GL_LINE_SMOOTH);
			glDisable(GL_POINT_SMOOTH);
		} else {
			bAntialiasing = true;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
		}
		break;
	case 'm':
	case 'M':
		gridMode = (gridMode+1)%3;
		break;
	case 'r':
	case 'R':
		dtResetGrid();
		break;
	case 's':
	case 'S':
		fopen_s(&fp, "dometwist.dat", "wt");
		// fp = fopen("dometwist.dat","wt");
		fprintf_s(fp, "dometwistmode %d\n", gridMode);
		if (gridMode < 2) 
		{
			fprintf_s(fp, "%d %d\n", gridSizeX, gridSizeY);
			for (i=0;i<gridSizeX;i++) {
				for (j=0;j<gridSizeY;j++) {
					fprintf_s(fp, "%f %f %f %f\n", mesh[i+j*gridSizeX].x, mesh[i+j*gridSizeX].y,  mesh[i+j*gridSizeX].u, mesh[i+j*gridSizeX].v);
				}
			}
		}
		if (gridMode == 2)
		{
			fprintf_s(fp, "%d\n", gridSize);
			for (j=0;j<4; j++) {
				for (i=0;i<4;i++) {
					fprintf_s(fp, "%f %f %f %f\n", grid4x4[i][j][0], grid4x4[i][j][1], texpts4x4[i][j][0], texpts4x4[i][j][1]);
				}
			}
		}
		fclose(fp);
		break;
	case '+':
		if (gridMode<2)
		{
			for (i=0; i<gridSizeX; i++) {
				for (j=0; j<gridSizeY; j++) {
					mesh[i+j*gridSizeX].x *= 1.05f;
					mesh[i+j*gridSizeX].y *= 1.05f;
				}
			}
		}
		if (gridMode == 2)
		{
			for (j=0; j<4; j++) {
				for (i=0; i<4; i++) {
					grid4x4[i][j][0] *= 1.05f;
					grid4x4[i][j][1] *= 1.05f;
				}
			}
		}
		break;
	case '-':

		if (gridMode<2)
		{
			for (i=0; i<gridSizeX; i++) {
				for (j=0; j<gridSizeY; j++) {
					mesh[i+j*gridSizeX].x *= 0.95f;
					mesh[i+j*gridSizeX].y *= 0.95f;
				}
			}
		}

		if (gridMode == 2)
		{
			for (j=0; j<4; j++) {
				for (i=0; i<4; i++) {
					grid4x4[i][j][0] *= 0.95f;
					grid4x4[i][j][1] *= 0.95f;
				}
			}
		}

		break;
	case 'u':
		if (gridMode < 2)
		{
			for (i=0; i<gridSizeX; i++) {
				for (j=0; j<gridSizeY; j++) {
					mesh[i+j*gridSizeX].y += 0.05f;
				}
			}
		}
		if (gridMode == 2)
		{
			for (i=0; i<4; i++) {
				for (j=0; j<4; j++) {
					grid4x4[i][j][1] += 0.05f;
				}
			}
		}
		break;
	case 'd':
		if (gridMode < 2)
		{
			for (i=0; i<gridSizeX; i++) {
				for (j=0; j<gridSizeY; j++) {
					mesh[i+j*gridSizeX].y -= 0.05f;
				}
			}
		}
		if (gridMode == 2)
		{
			for (i=0; i<4; i++) {
				for (j=0; j<4; j++) {
					grid4x4[i][j][1] -= 0.05f;
				}
			}
		}
		break;
	case 'z':
	case 'Z':
		isSymmetricalMode = !isSymmetricalMode;
		break;
	}
	glutPostRedisplay();
}

// Finds closest dot to a mouse click.
void findSection(int x, int y)
{
	float mx = (float)x;
	float my = (float)y;
	int selection = 0; 
	// Convert mouse position to proper coordinate.
	if (bFullScreenMode) {
		mx = mx/(0.5f*glutGet(GLUT_SCREEN_WIDTH))-1.0f;
		my = -(my/(0.5f*glutGet(GLUT_SCREEN_HEIGHT))-1.0f);
	} else {
		mx = mx/(0.5f*windowSizeX)-1.0f;
		my = -(my/(0.5f*windowSizeY)-1.0f);
	}
	int selx=0;
	int sely=0;
	if (gridMode == 2)
	{
		// Determine which dot needs to be selected.
		float distance = 100000.0f;
		for (int j=0; j<4; j++) {
			for (int i=0; i<4; i++) {
				float d = sqrt((mx-grid4x4[i][j][0])*(mx-grid4x4[i][j][0])+(my-grid4x4[i][j][1])*(my-grid4x4[i][j][1]));
				if (d<distance) {
					// selection = i+j*gridSizeX;
					selx = i;
					sely = j;
					distance = d;
				}
			}
		}
		selectedDotX = selx;
		selectedDotY = sely;
		grid4x4selected[selx][sely] = true;
	}

	// return selection;
}

// DomeTwister Mouse function.
void dtMouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON) 
	{
		// mesh[selectedDot].selectedMode = 0;
		// findSection(x, y);	
	}
	if (button == GLUT_LEFT_BUTTON) 
	{
		if (gridMode < 2)
			mesh[selectedDot].selectedMode = 0;
		if (gridMode == 2)
			grid4x4selected[selectedDotX][selectedDotY] = false;
		findSection(x, y);	
		leftButtonState = (leftButtonState+1)%2;
	}
}



