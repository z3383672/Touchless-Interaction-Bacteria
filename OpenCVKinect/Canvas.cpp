#include "Canvas.h"
#include <iostream>
#include<Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glut.h>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<string>
#include"calibration.h"
using std::cout;
using std::endl;
using std::ifstream;
using std::getline;
using namespace std;
#define PI 3.14159265
double Canvas::Xselect=0.0;
double Canvas::Yselect=0.0;
double Canvas::MajorSelect=0.0;
double Canvas::MinorSelect=0.0;
double Canvas::OrientationSelect=0.0;
double zoomlevel=1;
bool test=false;

Canvas::Canvas(void)
{
	char * argv[1];
	char dummystring[8];
	int argc=1;
	argv[0]=dummystring;
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(widescreen,heighscreen);
	glutCreateWindow("Oval");
	glutFullScreen();
}
void Canvas::myInit()
{
	glPointSize(3.0);
	//glMatrixMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glLoadIdentity();
	gluOrtho2D(0.0,1440,0.0,810);
	//gluOrtho2D(0.0,640,0.0,480.0);
	dtResetGrid();
	dtLoadGrid("dometwist.dat");
	heighscreen=810;
    widescreen=1440;

}

int Canvas::getHeight()
{
	return heighscreen;
}
int Canvas::getwidescreen()
{
	return widescreen;
}

Canvas::~Canvas(void)
{
}

void Canvas::setWindow(float left,float right,float bottom,float top)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left,right,bottom,top);
}
void Canvas::setViewPort(int left,int right,int bottom,int top)
{
	glViewport(left,right,bottom,top);

}
bool Canvas::HighLightBacteriaAtIndex(double XX,double YY,int frame,bool DrawInformation)
{
	double x_begin;
	double x_end;
	double y_end;
	double y_begin;
	bool detect=false;
	double  X=((XX-40.0)/250.0)*widescreen;
	double Y=((YY-10)/190.0)*heighscreen;
	std::string s=std::to_string(static_cast<long long>(frame));
	s="csv/1 ("+s+").csv";
	std::string filepath=s;
	std::string line;
	ifstream inFile;
	inFile.open(filepath);
	std::istringstream linestream(line);
	string item;
	int i=0;
	float x_center,y_center,orientation,majoraxis,minoraxis;
	
	if(X > 0 && Y > 0)
	{
		//GLuint text=LoadTexture(frame);
		int t=0;
		while(getline(inFile,line))
			{
				t++;
				std::istringstream linestream(line);
				string item;
				int i=0;
				while(getline(linestream,item,','))
				{
					switch(i)
					{
						case 0:
							x_center=atof(item.c_str());
							break;
						case 1:
							y_center=atof(item.c_str());
							break;
						case 2:
							orientation=atof(item.c_str());
							break;
						case 3:
							majoraxis=atof(item.c_str());
							break;
						case 4:
							minoraxis=atof(item.c_str());
							break;
						}
							i++;
					}
					double x_begin=x_center-(majoraxis/2.0)*cos(orientation*PI/180);
					double x_end=x_center+(majoraxis/2.0)*cos(orientation*PI/180);
					double y_end=y_center-(majoraxis/2.0)*sin(orientation*PI/180);
					double y_begin=y_center+(majoraxis/2.0)*sin(orientation*PI/180);
					majoraxis=sqrt((1024.0/1376.0)*(1024.0/1376.0)*(x_begin-x_end)*(x_begin-x_end)+(512.0/1032.0)*(512.0/1032.0)*(y_end-y_begin)*(y_end-y_begin));
					x_begin=x_center-(minoraxis/2.0)*sin(orientation*PI/180);
					x_end=x_center+(minoraxis/2.0)*sin(orientation*PI/180);
					y_end=y_center-(minoraxis/2.0)*cos(orientation*PI/180);
					y_begin=y_center+(minoraxis/2.0)*cos(orientation*PI/180);
					minoraxis=sqrt((1024.0/1376.0)*(1024.0/1376.0)*(x_begin-x_end)*(x_begin-x_end)+(512.0/1032.0)*(512.0/1032.0)*(y_end-y_begin)*(y_end-y_begin));
					x_center=(x_center*1024)/1376;
					y_center=(y_center*512)/1032;
				//////////////////////////////////////////////////////////////////////////////////////////
					x_begin=x_center-(majoraxis/2.0)*cos(orientation*PI/180);
					x_end=x_center+(majoraxis/2.0)*cos(orientation*PI/180);
					y_end=y_center-(majoraxis/2.0)*sin(orientation*PI/180);
					y_begin=y_center+(majoraxis/2.0)*sin(orientation*PI/180);
					majoraxis=sqrt((widescreen/1024.0)*(widescreen/1024.0)*(x_begin-x_end)*(x_begin-x_end)+(heighscreen/512.0)*(heighscreen/512.0)*(y_end-y_begin)*(y_end-y_begin));
					x_begin=x_center-(minoraxis/2.0)*sin(orientation*PI/180);
					x_end=x_center+(minoraxis/2.0)*sin(orientation*PI/180);
					y_end=y_center-(minoraxis/2.0)*cos(orientation*PI/180);
					y_begin=y_center+(minoraxis/2.0)*cos(orientation*PI/180);
					minoraxis=sqrt((widescreen/1024.0)*(widescreen/1024.0)*(x_begin-x_end)*(x_begin-x_end)+(heighscreen/512.0)*(heighscreen/512.0)*(y_end-y_begin)*(y_end-y_begin));
					x_center=(x_center/1024)*widescreen;
					y_center=(512-y_center)*heighscreen/512;
					if (abs(X-x_center) < 5 && abs(Y-y_center)< 5)
						{
							detect=true;
							Xselect=x_center;
							Yselect=y_center;
							MinorSelect=minoraxis;
							OrientationSelect=orientation;
							if (t % 30 ==0)
							{
								MajorSelect=majoraxis*3.7;
								MinorSelect=minoraxis*3.7;
							}
							else
							{
								MajorSelect=majoraxis*0.7;
								MinorSelect=minoraxis;
							}
							break;
						}
		}
		dtDisplay(frame,true,X,Y,detect,Xselect,Yselect,MajorSelect,MinorSelect,OrientationSelect,DrawInformation);//MinorSelect,OrientationSelect);
		if (detect== true)
			test=true;
		detect=false;
	}
	return test;
}



