#ifndef contourDetector_h
#define contourDetector_h
#include "simpleMotionBasedFilter.h"
#include "cxcore.h"
#include "cv.h" 
#include <stdio.h>
#include "fithisConnectedRegions.h"
#include "confParametersDefs.h"
//#define REGION_LEVEL_LOG


/**
\class contourDetector
\brief Implements a simple RegionLevel algorithm

  This class actually tries to recover region information in the image based on the preliminary
  pixel level observations and the color in the image.
*/
class  contourDetector
{
	private :
	separ_regionlevel_conf_t *conf;
	int temporal_thresh;
	IplImage *current_frame,*previous_frame;	
	fithisConnectedRegions *filt1,*filt2;
	simpleMotionBasedFilter *mbfilt;

	public :

		IplImage *current_separation,*temp_storage;
/**
The current_J variable in the WallFlower paper.
 */
	IplImage *current_J;
	
/**
The current_K variable of the WallFlower Paper. It is actually a logic AND between
current_J , previous_J and marked from the previous call to the (contourDetector::updateWithFrameAndMarked) 
method.
*/

/**
Contains the foreground objects discovered at the region Level. See the WallFlower paper
*/

/**
Marks a rectangle around K
*/
	
	public :
/**
\fn contourDetector(IplImage *raw,IplImage *marked,confParameters *params)
\brief A constructor that initializes the contourDetector class. Initially it needs
configuration parameters and a pair of Frame/Foreground marked images in order to initiallize
the members of the object.
\param raw A frame
\param marked A mask that marks pixels in frame as belonging to foreground
\param params Configuration parameters. See confParameters
*/	
	contourDetector(separ_regionlevel_conf_t* aconf);
	~contourDetector();

/**
\fn 	bool updateWithFrameAndMarked(IplImage *raw,IplImage *marked)
\brief Executes the region-level part of wallflower
\param raw A frame
\param marked A mask that marks pixels in frame as belonging to foreground
\return true on success false on failure.
*/	
	void updateWithFrameAndMarked(IplImage *raw,IplImage *marked);
	void initializeAll();
		
	private :
	void updateState();	
};

#endif
