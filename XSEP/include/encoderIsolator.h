#ifndef ENCODER_ISOLATOR_H
#define ENCODER_ISOLATOR_H
#include "stringIsolator.h"
#include "configuratorIsolator.h"
#include"XSEP.h"

class DLLIMPORT encoderIsolator
{
	void *austrian_th;
	void *spanish_th;
	void *grabber_th;
	void *glob_state;	
public:
	void DLLEXPORT recycle_SMS(encoder_sms *el);
	DLLEXPORT encoderIsolator(encoder_conf_t *pt,int aStamp);
	DLLEXPORT ~encoderIsolator();
	DLLEXPORT encoder_sms *getNames();
};
#endif
