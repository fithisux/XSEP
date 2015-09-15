#ifndef ENCODERSPECIFICS_H
#define ENCODERSPECIFICS_H
#include "stringIsolator.h"
#include "configuratorIsolator.h"
#include "generalConf.h"
#include "frameUtilities.h"

class stateOfEncoder
{
	public :
	int timeStamp;
	encoder_conf_t *encoder_conf;
	bool isActiveAustrian;
	bool isActiveSpanish;
	bool isActiveGrabber;
	frame_description * austrian_frame;
	frame_description * spanish_frame;


	stringIsolator *rtc_mesg_queue;
	stateOfEncoder(encoder_conf_t *some_conf)
	{
		this->encoder_conf=some_conf;
		this->isActiveAustrian=
		this->isActiveGrabber=
		this->isActiveSpanish=false;
		this->austrian_frame=new frame_description();
		this->spanish_frame=new frame_description();
		this->rtc_mesg_queue=new stringIsolator();
	}	
	~stateOfEncoder()
	{
		delete this->rtc_mesg_queue;
		delete this->austrian_frame;
		delete this->spanish_frame;
	}	
};
#endif /*ENCODERSPECIFICS_H*/
