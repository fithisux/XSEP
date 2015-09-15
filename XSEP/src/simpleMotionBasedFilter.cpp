#include "simpleMotionBasedFilter.h"

simpleMotionBasedFilter::simpleMotionBasedFilter(IplImage* raster,int asz,bool mustErode)
{
	this->next_raster=cvCloneImage(raster);
	this->previous_raster=cvCloneImage(raster);
	cvSet(this->previous_raster,cvScalarAll(255));
	this->previous_raster=cvCloneImage(this->next_raster);
	filt = new fithisConnectedRegions(raster);
	filt->setMinimumArea(asz);
	if(mustErode)
	{
		int * some_values = new int[16];
		for(int k=0;k<16;k++) some_values[k]=255;	
		this->struct_elem=cvCreateStructuringElementEx(4,4,0,0,CV_SHAPE_RECT,some_values);	
		delete[] some_values;
	}
	else this->struct_elem=NULL;
}
void simpleMotionBasedFilter::pushRaster(IplImage *raster)
{	
	cvAnd(raster,this->previous_raster,this->next_raster);	
	cvCopy(raster,this->previous_raster);
	this->filt->setWorkingFrame(raster);
	this->filt->setIndicator(this->next_raster);		
	this->filt->fithisExecuteCommand(FITHIS_FILTER);
	if(this->struct_elem) cvErode(raster,raster,this->struct_elem,1);	
}

simpleMotionBasedFilter::~simpleMotionBasedFilter()
{
	cvReleaseImage(&this->next_raster);
	cvReleaseImage(&this->previous_raster);
	delete filt;
	if(this->struct_elem) cvReleaseStructuringElement(&this->struct_elem);
}
