#include "cxcore.h"
#include "fithisConnectedRegions.h"

#ifndef SIMPLE_MOTION_BASED_FILTER_H
#define SIMPLE_MOTION_BASED_FILTER_H
class simpleMotionBasedFilter
{
public :
	IplImage *previous_raster;
	IplImage *next_raster;
	int region_size;
	IplConvKernel* struct_elem;
	fithisConnectedRegions *filt;

	simpleMotionBasedFilter(IplImage* raster,int asz,bool mustErode);
	void pushRaster(IplImage *raster);
	IplImage* getState(){return this->previous_raster;}
	~simpleMotionBasedFilter();
};
#endif
