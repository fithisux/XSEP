#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include "confParametersDefs.h"
#include "frameUtilities.h"
#include "highgui.h"

class DLLIMPORT frameGrabber
{
public :
	frame_description * frame;
	void *cam;

	separ_device_camera_conf_t * camera_conf;
	separ_device_sequence_conf_t* sequence_conf;
	separ_device_avi_conf_t* avi_conf;

	char ss_format[20];
	char ss_fname[100];
	int counter;
	int dur;
	bool isFinished;
	CvCapture * capture;

	DLLEXPORT frameGrabber(separ_device_camera_conf_t * acamera);
	DLLEXPORT frameGrabber(separ_device_sequence_conf_t* asequence);
	DLLEXPORT frameGrabber(separ_device_avi_conf_t* an_avi);
	void init_grabber();
	bool grabFromAVI();
	bool grabFromCamera();
	bool grabFromFile();
	bool hasStopped(){return this->isFinished;}

	DLLEXPORT ~frameGrabber();	
	DLLEXPORT frame_description * getFrame();
};
#endif
