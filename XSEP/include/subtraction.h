#ifndef subtraction_h
#define subtraction_h

#include <iostream>
#include <fstream>
using namespace std;

#include "PixelLevel.h"
#include "faceDetector.h"
#include "confParameters.h"
#include "contourDetector.h"
#include "fithisConnectedRegions.h"
#include "frameGrabber.h"
#include "shadowclass.h"
#include "cv.h"
#include <stdio.h>
#include"XSEP.h"


/*!
* @class subtraction
* @brief Implements the HC Localization
*
*  This class actually finds faces in a region constrained by background subtraction
*/


//#define SUBTRACTION_LOG

class DLLIMPORT subtraction
//class subtraction
{
protected :
	int counter;
	bool isFinished;
	frameGrabber *agrabber;
	confParameters *params;
	PixelLevel* apred;
	shadowClass * deshade;
	face_detect_ctx *facedet;
	contourDetector *region_level;
	contourDetector *face_level;

	fithisConnectedRegions* filt;
	fithisConnectedRegions* face_filt;
	CvHistogram* face_hist;
	int* face_hist_sizes;
	float** face_hist_ranges;

	bool ready;	

public :
	/*!
	* The current frame obtained through a file sequence or an avi. Initially  (before
	* a call to subtraction::nextFrame() it is NULL.
 */
	frame_description *timed_frame;
	IplImage* raw_frame;
	IplImage* face_estimation;
	IplImage** raw_frame_splitter;
	IplImage* processed_frame;
	IplImage** processed_frame_splitter;
	IplImage* foreground_frame;
	IplImage* final_frame;
	IplImage* face_body_masks;
	IplImage* prediction_frame;
	IplImage* internal_foreground_frame;	
	IplImage* region_level_frame;	
	vector<CvRect *>* face_vector;
	vector<CvRect *>* body_vector;	
	
public :
	/**
* \fn subtraction(const char *xml_file);
* \brief A constructor that initializes the subtraction object with the location of an XML configuration file.RegionLevel class. 
* \param xml_file The location of the xml_file. Potentially it throws an exception (std::string) 
*/
	DLLEXPORT subtraction(const char *xml_file);
	DLLEXPORT ~subtraction();
/**
* \fn bool subtraction::isReady();
*
* \brief If ready the ubtraction object has started processing frames. Because of learning actions taken as a result
* of prediction after a number of subtraction::frame  can one start actually using the results.
*/
	bool isReady();
	
/**
* \fn vector<CvRect *>*	subtraction::getFaces();
* \brief It  gives a seqence of rectangles containing potentially faces.
*/

	DLLEXPORT void getParts();
	
	void getZoomedParts();

/**
* \fn 		confParameters *subtraction::getParams();
* \brief It  gives the parameters read from the xml file.
* \return xml parameters
*/
	
	DLLEXPORT confParameters *getParams();

/**
* \fn 	bool subtraction::nextFrame();
* \brief When not true it means frames (except current frame) are not available for output.
* \return true if it got a frame
*/
	DLLEXPORT bool nextFrame();
	DLLEXPORT void clearFacesAndBodies();
	DLLEXPORT void grabFrame();
	DLLEXPORT void extraProcess();
private :
	DLLEXPORT void getFaceEstimation();
};

#endif
