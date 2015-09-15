#include"hiddenVideoWriter.h"
#include "cxcore.h"
#include "highgui.h"
#include "cv.h"
#include "cvaux.h"

hiddenVideoWriter::hiddenVideoWriter(const char *avi_name)
{
	this->my_writer=NULL;
	this->file_name=new char[1+strlen(avi_name)];
	strcpy(this->file_name,avi_name);
}

void hiddenVideoWriter::writeFrame2AVI(frame_description *el)
{	
	if(!this->my_writer)
	{
		CvSize pt=cvGetSize((IplImage *)el->getImage());
		this->my_writer=cvCreateVideoWriter(this->file_name,0,25,pt,1);
	}
	cvWriteFrame( (CvVideoWriter *)this->my_writer,(IplImage *)el->getImage());
}

hiddenVideoWriter::~hiddenVideoWriter()
{	
	CvVideoWriter *some_writer=(CvVideoWriter *)this->my_writer;
	if(some_writer) cvReleaseVideoWriter(&some_writer);
}
