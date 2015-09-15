#include "frameGrabber.h"

#include "cameraModule.h"





	void frameGrabber::init_grabber()
	{
		camera_conf=NULL;
		sequence_conf=NULL;
		avi_conf=NULL;
		counter=0;
		frame=NULL;
		isFinished=false;
		capture=NULL;
		this->cam=NULL;
		this->frame=NULL;
	}

DLLEXPORT frameGrabber::frameGrabber(separ_device_camera_conf_t * acamera)
{
		cameraModule *acam;
		printf("you use a camera \n");
		printf("possibly with time stamp \n");
		this->init_grabber();
		this->camera_conf=acamera;

		acam = new cameraModule(acamera->address,acamera->port);
		acam->setDuration(acamera->duration);
		this->dur=acamera->duration;
		this->cam=acam;	

		
		if(!strcmp(acamera->format,"MJPG"))
		{
			printf("Format selected mjpg \n");
            acam->setFormat(_MJPG_);
		}
		else if(!strcmp(acamera->format,"JPG"))
		{
			printf("Format selected jpg \n");
			acam->setFormat(_JPG_);
		}
		else if(!strcmp(acamera->format,"BMP"))
		{
			printf("Format selected bmp \n");
			acam->setFormat(_BMP_);
		}
		else
		{
			printf("UNKNOWN camera format. Aborting ... \n");
			exit(0);
		}
	
		if(acamera->fps) 
		{
			acam->setFrameRate(acamera->fps);
		}
}

DLLEXPORT frameGrabber::frameGrabber(separ_device_sequence_conf_t* asequence)
{
	this->init_grabber();
	this->sequence_conf=asequence;
	this->frame=new frame_description();
}

DLLEXPORT frameGrabber::frameGrabber(separ_device_avi_conf_t* an_avi)
{
	this->init_grabber();
	this->avi_conf=an_avi;
	sprintf(this->ss_fname,"%s",an_avi->file);		
	capture = cvCaptureFromFile(this->ss_fname);
	this->frame=new frame_description();
}

bool frameGrabber::grabFromAVI()
{
	IplImage *raw_frame;
	
	if(!capture) throw (string("Capture from ")+string(this->ss_fname)+string(" cannot be accomplished!!!"));
	if( (raw_frame = cvQueryFrame(capture)) == NULL)
	{   			
		this->frame->setImage(NULL);		
		return false;
    }
	if(raw_frame->origin)
	{
		cvFlip(raw_frame,NULL,0);		
		raw_frame->origin=0;
	}
	this->frame->setImage(raw_frame);
	return true;
}

bool frameGrabber::grabFromCamera()
{
	cameraModule *acam=(cameraModule *) this->cam;
	if(this->counter==0) acam->startCamera();
	if(this->dur && (this->dur==this->counter))
	{
		this->frame->setImage(NULL);
		return false;
	}
	this->frame=acam->getFrame();
	if(!this->frame) return false;
	this->counter++;
	return true;
}

bool frameGrabber::grabFromFile()
{
	int file_num = this->counter+this->sequence_conf->image_from; 
	if( file_num > this->sequence_conf->image_to)
	{
			this->isFinished=true;
			return false;
	}

	IplImage* some_image = (IplImage *)this->frame->getImage();

	if(this->sequence_conf->image_length_prefix>0)
	{
		sprintf(this->ss_format,"%s%d%s","%s%0",
				this->sequence_conf->image_length_prefix,"d.%s");
	}
	else  sprintf(this->ss_format,"%s","%s%d.%s");
	
	this->counter++;
	sprintf(this->ss_fname,this->ss_format,
			this->sequence_conf->image_prefix,
			file_num,this->sequence_conf->image_type);
	IplImage *raw_frame = cvLoadImage(this->ss_fname);
	printf("Reading %s \n",this->ss_fname);

	if(!raw_frame)
	{
		if(some_image) cvReleaseImage(&some_image);	
		this->frame->setImage(NULL);
		throw (std::string("File ")+std::string(this->ss_fname)+std::string(" not found!!!"));
	}
	if(raw_frame->origin) cvFlip(raw_frame,NULL,1);		
		
	if(!some_image)
	{
		some_image=cvCloneImage(raw_frame);			
		this->frame->setImage(some_image);
	}
	else cvCopy(raw_frame,some_image); 
	cvReleaseImage(&raw_frame);	
	return true;
}

DLLEXPORT frame_description * frameGrabber::getFrame()
{
	if(this->isFinished) return NULL;
	bool resus;	
	
	try
	{
		if(this->avi_conf) resus=this->grabFromAVI();
		else if(this->sequence_conf) resus=this->grabFromFile();
		else resus=this->grabFromCamera();
	}
	catch(std::string &e)
	{
		resus=false;
		printf("Closing grabber because %s \n",e.c_str());
	}

	if(!resus)
	{
		this->isFinished=true;
		if(!this->cam) delete this->frame;
		this->frame=NULL;
	}
	else
	{
		if(strlen(this->frame->getTimeStamp())==0)
		{
			this->frame->setTimeStamp();
		}							
	}
	return this->frame;
}

DLLEXPORT frameGrabber::~frameGrabber()
{
	cameraModule *acam=(cameraModule *) this->cam;
	this->cam=NULL;
	if(acam)
	{
		this->frame=NULL;
		delete acam;
	}
	if(this->capture) cvReleaseCapture(&this->capture);
	if(this->frame) delete this->frame;
}
