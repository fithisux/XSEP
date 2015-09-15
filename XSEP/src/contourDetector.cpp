 
#include "contourDetector.h"
#include "bgUtilities.h"

// page 2 , paper for parameters
contourDetector::contourDetector(separ_regionlevel_conf_t* aconf)						
{
	this->conf=aconf;
	this->temporal_thresh=10;
	this->previous_frame=this->current_frame=NULL;	
	this->current_separation=NULL;
	this->current_J=NULL;
	this->filt1=this->filt2=NULL;
	this->mbfilt=NULL;	
}


void contourDetector::updateState()
{
	//image_u8_rgb_diff(this->current_frame,this->previous_frame,this->current_J,this->conf->colourthres);
	this->mbfilt->pushRaster(this->current_J);
	this->filt2->fithisExecuteCommand(FITHIS_FILTER);
}

contourDetector::~contourDetector()
{
	cvReleaseImage(&previous_frame);
	cvReleaseImage(&current_frame);
	cvReleaseImage(&current_separation);
	cvReleaseImage(&current_J);	
}
	
//Puts region separation in current_K
void contourDetector::updateWithFrameAndMarked(IplImage *raw,IplImage *marked)
{
	if(!this->current_J)
	{			
		this->current_frame=cvCloneImage(raw);			
		this->current_separation=cvCloneImage(marked);	
		this->previous_frame=cvCloneImage(raw);
		this->initializeAll();
	}
	else
	{			
		cvCopy(this->current_frame,this->previous_frame);		
		cvCopy(raw,this->current_frame);	
		cvCopy(marked,this->current_separation);		
	}
	
	cvNot(this->current_separation,this->current_separation);
	this->filt1->fithisExecuteCommand(FITHIS_FILTER);
	cvNot(this->current_separation,this->current_separation);
	this->filt1->fithisExecuteCommand(FITHIS_FILTER);

#ifdef REGION_LEVEL_LOG
	printf("Updating J and K \n");
#endif
	this->updateState();
#ifdef REGION_LEVEL_LOG			
	printf("Backprojecting \n");
#endif	
}


void contourDetector::initializeAll()
{	
	this->current_J=cvCloneImage(this->current_separation);
	cvSet(this->current_J,cvScalarAll(255));
			
	this->filt1 = new fithisConnectedRegions(this->current_separation);
	this->filt1->setMinimumArea(this->conf->recover_area);	
	
	this->filt2 = new fithisConnectedRegions(this->current_separation);
	this->filt2->setThreshold(this->conf->colourthres);
	this->filt2->setMinimumArea(this->conf->recover_area);
	this->filt2->setWorkingFrame(this->current_separation);
	this->filt2->setIndicator(this->current_J);	
		
	this->mbfilt=new simpleMotionBasedFilter(this->current_J,this->conf->recover_area,false);	
}
