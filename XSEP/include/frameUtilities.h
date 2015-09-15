#ifndef FRAMEUTILITIES_H
#define FRAMEUTILITIES_H

#define MAX_HEADER_LENGTH 200
#include "XSEP.h"
#include <stdio.h>
#include <stdlib.h>

class DLLIMPORT frame_description
{
private:
		char time_stamp[MAX_HEADER_LENGTH];
		void* image;
		int length;
		char *blob;		
		int max_length;	    
public :
		int Id;
	    DLLEXPORT frame_description();
		DLLEXPORT frame_description(int some_id);
	    DLLEXPORT ~frame_description();
		DLLEXPORT void update(int some_length);
		DLLEXPORT void copy(frame_description *aframe);
		DLLEXPORT char *getBlob();
		DLLEXPORT int getLength();
		DLLEXPORT char *getTimeStamp();
		DLLEXPORT int getMaxLength();
		DLLEXPORT void setTimeStamp();
		DLLEXPORT void* getImage();
		DLLEXPORT void setImage(void *img);
};

#endif

