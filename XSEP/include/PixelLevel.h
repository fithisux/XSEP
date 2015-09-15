#ifndef PIXELLEVEL_H_
#define PIXELLEVEL_H_
#include "cxcore.h"
#include  "cv.h" 
#include "cvaux.h"
#include "fithisConnectedRegions.h"
#include "confParameters.h"
#include "bgUtilities.h"
#include "simpleMotionBasedFilter.h"
#include "XSEP.h"
//#define PIXEL_LEVEL_LOG

/*!
\file PixelLevel.h

	\brief A file holding the template of PixelLevel

	One of the primary decompositions of background subtraction in Polymnia is 
	PixelLevel>RegionLevel>FrameLevel.  Each sub-module can be implemented in a variety of ways.
	Many algorithms exist. Instead of finalizing to a particular algorithm. We implement a plugin like
	architecture that takes different PixelLevel algorithms as plugins.
*/

/*! \class PixelLevel
    \brief An abstract class implementing the Pixel Level processing.

    The Pixel Level class is selected to be abstract with the intention to act
	as a template class to create specialized algorithms. So, Mixture Of Gaussians, 
	a bayesian estimation based algorithm and various improvements by NTUA team can
	be incorporated with little effort. It contains the least common denominator of features
*/
class PixelLevel
{
	
protected :
	simpleMotionBasedFilter* mbfilt;
	separ_pixellevel_params_t* pixellevel_params;
	tri_info my_info;	
	int goskip;
	IplConvKernel* struct_elem;
	fithisConnectedRegions* filt;
	public :
		
	IplImage *current_frame,*prediction,*foreground;
	
	PixelLevel(separ_pixellevel_conf_t *params);
	virtual bool isReady()=0;
	virtual void updateWithFrame(IplImage* frame);
	virtual void predict()=0;
	virtual ~PixelLevel();
	
	void computeForeground(bool flag);
	FithisMosaic * getObjMosaic();
	
	protected :	
	virtual void computePrediction()=0;
	void initializeAll(IplImage *frame);
	
};

#endif /*PIXELLEVEL_H_*/
