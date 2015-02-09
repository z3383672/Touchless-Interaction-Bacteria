#pragma once
#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<glut.h>
class Canvas
{
public:
	Canvas(void);
	~Canvas(void);
	bool HighLightBacteriaAtIndex(double ,double ,int frame,bool);
	int getHeight();
	int getwidescreen();
	void myInit();
	static double Xselect,Yselect,MajorSelect,MinorSelect,OrientationSelect;
	void setWindow(float left,float right,float bottom,float top);
	void setViewPort(int left,int right,int bottom,int top);
	GLuint fb;
	GLuint fr; 
private:
	int heighscreen;
	int widescreen;

};


