#include "PixelLevel.h"

PixelLevel::PixelLevel(separ_pixellevel_conf_t *params)
{
#ifdef PIXEL_LEVEL_LOG
		printf("Initialize pixel level \n");
#endif
	this->filt=NULL;
	this->pixellevel_params=params->params;
			
	this->my_info.use_hsv = this->pixellevel_params->use_hsv;
	for(int l=0;l<4;l++)
	{
		this->my_info.flags.val[l]=this->pixellevel_params->weights[l];
	}
	my_info.norm=NORM_INFINITY;
	this->current_frame=this->foreground=this->prediction=NULL;
	this->goskip=this->pixellevel_params->skip;		
	this->mbfilt=NULL;
	this->filt=NULL;
	
#ifdef PIXEL_LEVEL_LOG
		printf("Finished initialize pixel level \n");
#endif

		int * some_values = new int[16];
		for(int k=0;k<16;k++) some_values[k]=255;	
		this->struct_elem=cvCreateStructuringElementEx(3,3,0,0,CV_SHAPE_RECT,some_values);	
		delete[] some_values;
	
}

void PixelLevel::updateWithFrame(IplImage* frame) 
{	
#ifdef PIXEL_LEVEL_LOG
		printf("Update and convert \n");
#endif
	
	if(!this->current_frame) this->initializeAll(frame);

	if(this->pixellevel_params->use_hsv)
	{
#ifdef PIXEL_LEVEL_LOG
		printf("we use hsv, we must convert \n");
#endif
		cvCvtColor(frame,this->current_frame,CV_BGR2HSV);		
#ifdef PIXEL_LEVEL_LOG
		printf("Converted \n");
#endif	
	}
	else cvCopy(frame,this->current_frame);
	
	if(--this->goskip <= 0) this->goskip=this->pixellevel_params->skip;	
#ifdef PIXEL_LEVEL_LOG
		printf("Finished update and convert %d \n",this->goskip);
#endif	
}

PixelLevel::~PixelLevel()
{	
#ifdef PIXEL_LEVEL_LOG
		printf("Delete pixel level \n");
#endif
	cvReleaseStructuringElement(&this->struct_elem);
	cvReleaseImage(&this->current_frame);
	if(this->mbfilt) delete this->mbfilt;
	cvReleaseImage(&this->prediction);
	cvReleaseImage(&this->foreground);
	if(this->filt) delete this->filt;
	
}

//compute the foreground , in this->foreground 255 means fg 0 bg
void PixelLevel::computeForeground(bool flag)
{	
	
	if(flag)
	{
		tri_image_diff(this->prediction,
					this->current_frame,
					this->foreground,
					this->pixellevel_params->diffthreshold,
					&this->my_info);		
	}		

	if(this->pixellevel_params->filterMotion)
	{
#ifdef PIXEL_LEVEL_LOG		
		printf("filterMotionEnabled \n");
#endif		
		this->mbfilt->pushRaster(this->foreground);
#ifdef PIXEL_LEVEL_LOG
		printf("Done filterMotion \n");
#endif
	}	
	

	cvNot(this->foreground,this->foreground);
	this->filt->fithisExecuteCommand(FITHIS_FILTER);
	cvNot(this->foreground,this->foreground);
	this->filt->fithisExecuteCommand(FITHIS_FILTER);
}

FithisMosaic* PixelLevel::getObjMosaic()
{	
	this->filt->fithisExecuteCommand(FITHIS_FILTER);
	return this->filt->getSegMosaic();
}

void PixelLevel::initializeAll(IplImage *frame)
{
#ifdef PIXEL_LEVEL_LOG
		printf("Bulk initialize for pixel level \n");
#endif
	
	this->current_frame = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,frame->nChannels);
	this->prediction = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,frame->nChannels);
	this->foreground=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	this->filt=new fithisConnectedRegions(this->foreground);
	this->filt->setMinimumArea(this->pixellevel_params->minArea);
	if(this->pixellevel_params->filterMotion)
	{
		this->mbfilt=new simpleMotionBasedFilter(this->foreground,1,this->pixellevel_params->mustErode);
	}
	cvZero(this->foreground);
#ifdef PIXEL_LEVEL_LOG
		printf("Finished bulk initialize for pixel level \n");
#endif
	
}
