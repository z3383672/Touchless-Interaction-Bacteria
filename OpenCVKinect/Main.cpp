#include <windows.h>
#include <wchar.h>
#include <list>
#include <vector>
#include <conio.h>
#include "pxcsession.h"
#include "pxcsmartptr.h"
#include "pxccapture.h"
#include "util_render.h"
#include "util_capture_file.h"
#include "util_cmdline.h"
#include "pxcgesture.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glut.h>
#include<fstream>
#include<sstream>
#include<string>
#include<stdlib.h>
#include"SOIL.h"
#include"Canvas.h"
#include<stdio.h>
#include <wchar.h>
#include <list>
#include <vector>
#include <conio.h>
#include "util_pipeline.h"
#include "pxcsmartptr.h"
#include "pxccapture.h"
#include "util_render.h"
#include "util_capture_file.h"
#include "util_cmdline.h"
#include<iostream>
#include<process.h>
#include"calibration.h"
int BacPerFrame=0;
bool DrawInformation;
Canvas *csv;//=new Canvas();
class MyPipeline;
MyPipeline *pipeline;
using namespace std;
int frame=1;
int frameKinect=1;
#define PI 3.14159265
using std::cout;
using std::endl;
using std::ifstream;
using std::getline;
UtilCaptureFile *captures;
PXCSmartPtr<PXCGesture> gestureDetector;
HANDLE hVectorMutex;
bool LABEL_POSE_THUMB_UP=false;
bool LABEL_HAND_CLOSE=true;
bool LABEL_HAND_OPEN=false;
bool LABEL_POSE_THUMB_DOWN=true;
double Xhightlight;
double Yhighlight;
bool FirstEntertoDraw=true;
GLuint texture;
bool Alert=false;
bool goforit=false;
class MyPipeline: public UtilPipeline {
public:
	MyPipeline(void):UtilPipeline() { 
		EnableGesture();
	}
	virtual void PXCAPI OnGesture(PXCGesture::Gesture *data) {
	}
	virtual bool OnNewFrame(void)
	{
		Alert=false;
		PXCGesture::Gesture data;
		PXCGesture *pxc=QueryGesture();
		pxc->QueryGestureData(0,PXCGesture::GeoNode::LABEL_ANY,0,&data);
		PXCGesture::GeoNode ndata;
		if (data.label==PXCGesture::Gesture::LABEL_POSE_THUMB_UP)
		{
			LABEL_POSE_THUMB_UP=true;
			LABEL_POSE_THUMB_DOWN=false;
			LABEL_HAND_OPEN=true;
			LABEL_HAND_CLOSE=false;
		}
		if (data.label==PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN)
		{
			LABEL_POSE_THUMB_UP=false;
			LABEL_POSE_THUMB_DOWN=true;
		}
		
		if (LABEL_POSE_THUMB_UP==true && pxc->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT,&ndata)>=PXC_STATUS_NO_ERROR)
		{
			WaitForSingleObject(hVectorMutex,INFINITE);
					Xhightlight=ndata.positionImage.x;
					Yhighlight=ndata.positionImage.y;
			ReleaseMutex(hVectorMutex);
			if (ndata.opennessState==PXCGesture::GeoNode::LABEL_OPEN)
			{
				
				LABEL_HAND_CLOSE=false;
				LABEL_HAND_OPEN=true;
			}
			if (LABEL_POSE_THUMB_UP==true  &&  ndata.opennessState==PXCGesture::GeoNode::LABEL_CLOSE && LABEL_HAND_OPEN==true)
			{
				LABEL_HAND_CLOSE=true;
				LABEL_HAND_OPEN=false;
			}
		}
		return true;
	}
};

void TrackOpenClose(GLuint texture)
{
	double X,Y;
	
	WaitForSingleObject(hVectorMutex,INFINITE);//This is just a mutex to assure that two thread doen't access the memory at the same time
			X=Xhightlight;
			Y=Yhighlight;
	ReleaseMutex(hVectorMutex);
	if ((LABEL_HAND_OPEN==true) && (LABEL_HAND_CLOSE==false))
	{
		DrawInformation=false;
		FirstEntertoDraw=true;
		if (X > 40 && X < 290 && Y > 5 && Y<200)
		{
			csv->HighLightBacteriaAtIndex(320-X,240-Y,frame-1,DrawInformation);
		}
	}
	if ((LABEL_HAND_OPEN==false) && (LABEL_HAND_CLOSE==true) && (FirstEntertoDraw==true))
	{
		DrawInformation=true;
		FirstEntertoDraw=false;
		goforit=csv->HighLightBacteriaAtIndex(csv->Xselect,csv->Yselect,frame-1,DrawInformation);
		
	}
	if ((LABEL_HAND_OPEN==false) && (LABEL_HAND_CLOSE==true) && (FirstEntertoDraw==false))
	{
		if (X > 40 && X < 290 && Y > 10 && Y<200 && goforit==true)
		{
			
			Motion(((320-X)-40)/250.0*1024,((240-Y)-10)/190.0*512);
		}
	}
}

void display (void)
{
	if(LABEL_POSE_THUMB_UP ==true)  
	{
		TrackOpenClose(texture);
	}
	if (LABEL_POSE_THUMB_DOWN==true)
	{
			dtDisplay(frame,false,0,0,false,0,0,0,0,0,false);
			for(int ss=0;ss<20000000;ss++);
			frame++;
			if(frame > 20)
				frame=1;	 
	}
}

void F2(void *pMyID)
{
	pipeline->LoopFrames();
}

int main(int argc,WCHAR* argv[])
{
	csv=new Canvas();
	csv->myInit();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	LoadTextures();
	glutKeyboardFunc(dtKeyboard);
	glutMouseFunc(dtMouse);
	glutMotionFunc(dtMotion);
	glutReshapeFunc(dtResize);
	MyPipeline pipe;
	pipeline=&pipe;
	_beginthread(F2,0,0);
	glutMainLoop();	
}	




