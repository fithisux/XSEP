#include "cameraModule.h"
#include <iostream>
#include<exception>
using namespace std;

cameraModule::cameraModule(const char * address,int port)
{	
	this->my_blob=new Magick::Blob;
	this->my_image=new Magick::Image;
	duration=0;
	frame_rate=25;
	resolution=cvSize(-1,-1);
	compression=-1;
	format=_INVALID_;
	forever=true;
	is_some_stream=false;	 
	this->first_frame=false;
	trans=new axisTransaction(address,port);
	this->current_frame=NULL;
	this->started=false;
	this->frame_number=0;
}

cameraModule::~cameraModule()
{
	delete this->my_image;
	delete this->my_blob;
	this->current_frame=this->trans->getFrame();
	IplImage *image=(IplImage *)this->current_frame->getImage();
	if(image) cvReleaseImage(&image);
	this->current_frame->setImage(NULL);
	if(this->trans) delete trans;	
	this->current_frame=NULL;
}

void cameraModule::setFormat(camera_support_format s)
{
	format=s;
	if(s == _MJPG_)is_some_stream=true;
	else is_some_stream=false;
}

void cameraModule::setResolution(CvSize spec)
{
	resolution.width=spec.width;
	resolution.height=spec.height;
}

void cameraModule::setCompression(int comp)
{
	compression = comp;
}

void cameraModule::setDuration(int dur)
{
	if(dur <0)
	{
		cout << "No negative duration allowed. Aborting ..." << endl;
		exit(0);
	}

	duration = dur;		
	if(!dur) forever=true;
}

void cameraModule::setFrameRate(int fr)
{
	frame_rate = fr;	
}
void cameraModule::changeZoom(int x)
{
//	this->trans->changeZoom(x);
}

bool cameraModule::sendRequest()
{
	int index;
	char comm[200];
	bool is_first_arg;

	index=0;
	is_first_arg=true;
	//if(! trans->isAlive()) return NULL;

	if(this->format==_INVALID_)
	{
		throw std::string("No format specified");
		return false;
	}

	switch(this->format)
	{
		case _JPG_ :
#ifdef CAMERA_LOG 
				cout << "we have a JPEG " << endl;
#endif
				index=sprintf(comm,"GET /jpg/image.jpg");
				break;
		case _BMP_ :
#ifdef CAMERA_LOG 
				cout << "we have a BMP " << endl;
#endif
				index=sprintf(comm,"GET /bitmap/image.bmp");
				break;
		case _MJPG_ :		
#ifdef CAMERA_LOG
				cout << "we have a MJPEG " << endl;
#endif
				index=sprintf(comm,"%s","GET /axis-cgi/mjpg/video.cgi");
				index+=sprintf(&comm[index],"\?duration=%d",this->duration);					
				is_first_arg=false;
				if(this->compression > 0)
				{
					index+=sprintf(&comm[index],"&compression=%d",this->compression);
				}

				if(this->frame_rate>0)
				{
					index+=sprintf(&comm[index],"&fps=%d",this->frame_rate);
				}
				break;
	}
	
	if( (this->resolution.height != -1) && (this->resolution.width!=-1))
	{
		if(is_first_arg)	index+=sprintf(&comm[index],"\?");
		else				index+=sprintf(&comm[index],"&");
		index+=sprintf(&comm[index],"resolution=");
		index+=sprintf(&comm[index],"%d",this->resolution.width);
		index+=sprintf(&comm[index],"x");
		index+=sprintf(&comm[index],"%d",this->resolution.height);
	}
					
	index+=sprintf(&comm[index]," HTTP/1.0 \r\n");
	index+=sprintf(&comm[index],"%s","From: vanag@telecom.ntua.gr \r\n");
	index+=sprintf(&comm[index],"%s","User-Agent: HawkNL \r\n\r");
#ifdef CAMERA_LOG
	printf(">> %s \n",comm);
#endif
	return this->trans->sendRequest(comm);	

}

void cameraModule::startCamera()
{
	printf("camera started \n");
	this->started=true;
	this->frame_number=this->duration;
}

bool cameraModule::grabFrame()
{	
#ifdef CAMERA_LOG
	cout << "we grab"<<endl;		
#endif

	if(this->started)
	{
		if(!this->forever)
		{
			if(this->frame_number == 0) return false;
			else this->frame_number--;
		}		
		if(!is_some_stream)
		{
			if(!this->sendRequest()) return false;
			return this->trans->safeGetResponse();	
			
		}
		else
		{
			if(!this->first_frame)
			{
				this->first_frame=true;
				if(!this->sendRequest()) return false;
			}
#ifdef CAMERA_LOG
	cout << "the stream "<<endl;		
#endif
			return this->trans->safeGetResponse();		
		}
	}
	else return false;
}

frame_description *cameraModule::getFrame()
{
	this->reconstructFrame();
	return this->current_frame;		
}

void cameraModule::reconstructFrame()
{
	Magick::PixelPacket *pixel_runner=NULL;
	unsigned char *runner;
	CvSize asz;
	IplImage *image=NULL;
	

#ifdef CAMERA_LOG
	cout << "we reconstruct"<<endl;		
#endif

	if(!this->grabFrame())
	{
		this->current_frame=this->trans->getFrame();		
		image=(IplImage *)this->current_frame->getImage();
		if(image) cvReleaseImage(&image);
		this->current_frame->setImage(NULL);
		this->current_frame=NULL;
		return;
	}
	this->current_frame=this->trans->getFrame();

	
#ifdef CAMERA_LOG
	cout << "new blob with length "<<trans->frame->img->length<<endl;		
#endif

#ifdef CAMERA_LOG
	cout << "new image\n";		
#endif

	this->my_blob->update(this->current_frame->getBlob(),this->current_frame->getLength());
	try 
	{   	
		this->my_image->read(*this->my_blob);
    }	 
	catch (std::exception &error)
    {
       // Handle problem while rotating or writing outfile.
       cerr << "Caught Magick++ exception: " << error.what() << endl;
	   exit(0);
    }
	
	asz.width=this->my_image->columns();
	asz.height=this->my_image->rows();
	pixel_runner = this->my_image->getPixels(0,0,asz.width,asz.height);

	image=(IplImage *)this->current_frame->getImage();
	if(!image)
	{
		image=cvCreateImage(asz,IPL_DEPTH_8U,3);
		this->current_frame->setImage(image);
	}
	
	if( (image->width !=asz.width) || (image->height!=asz.height) )
	{
		cvReleaseImage(&image);
		image=cvCreateImage(asz,IPL_DEPTH_8U,3);
		this->current_frame->setImage(image);
	}	

	unsigned char *ptr=(unsigned char *) image->imageData;
	for(int i=0;i<asz.height;i++)
	{
		runner=ptr;
		for(int j=0;j<asz.width;j++)
		{
			*runner++=pixel_runner->blue;
			*runner++=pixel_runner->green;
			*runner++=pixel_runner->red;
			pixel_runner++;
		}
		ptr+=image->widthStep;
	}

#ifdef CAMERA_LOG
	printf("data ok\n");
#endif
}
