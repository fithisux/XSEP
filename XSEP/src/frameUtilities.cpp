#include "frameUtilities.h"
#include "generalConf.h"

DLLEXPORT void frame_description::setTimeStamp()
{
	pt::datetime nowstamp;
	pt::string s;

	nowstamp=pt::now();

	s=pt::itostring(nowstamp);
	sprintf(this->time_stamp,"%s",pt::pconst(s));
}

DLLEXPORT frame_description::frame_description()
{
	this->Id=-1;
	this->blob=NULL;
	this->length=this->max_length=0;
	this->image=NULL;
	sprintf(this->time_stamp,"");
}

DLLEXPORT frame_description::frame_description(int some_id)
{
	this->Id=some_id;
	this->blob=NULL;
	this->length=this->max_length=0;
	this->image=NULL;
	sprintf(this->time_stamp,"");
}

DLLEXPORT frame_description::~frame_description()
{
	if(this->blob) delete[] this->blob;
	this->blob=NULL;
	this->image=NULL;
}

DLLEXPORT void frame_description::update(int some_length)
{
	this->length=some_length;
	if(some_length > this->max_length)
	{
		if(this->blob) delete[] this->blob;
		this->blob=new char [some_length];
		this->max_length=some_length;
	}
}

DLLEXPORT void frame_description::copy(frame_description *aframe)
{
	aframe->update(this->length);
	memcpy(aframe->getBlob(),this->blob,this->length);
	strcpy(aframe->getTimeStamp(),this->getTimeStamp());
}

DLLEXPORT char * frame_description::getBlob()
{
	return this->blob;
}

DLLEXPORT int frame_description::getLength()
{
	return this->length;
}

DLLEXPORT void* frame_description::getImage(){return this->image;}
DLLEXPORT void frame_description::setImage(void *img){this->image=img;}

DLLEXPORT int frame_description::getMaxLength()
{
	return this->max_length;
}

DLLEXPORT char *frame_description::getTimeStamp(){return this->time_stamp;}
