#ifndef FITHIS_CONNECTED_REGIONS_H
#define FITHIS_CONNECTED_REGIONS_H

//#define FITHIS_CONNECTED_REGIONS_DEBUG
/*!
\file fithisConnectedRegions.h

	\brief Contains general declarations for connectedComponents.

	This file contains the necessary data structures and methods for implementing 
	segmentation and filtering based on connected components.
*/
#include "cxcore.h"
#include "cv.h"
#include "bgUtilities.h"

/*! \enum FithisConnectivity
	\brief Possible connectivity

	The commands in this particular enum describe the connectivity when finding connected regions
*/

typedef enum { FITHIS_4_CROSS,FITHIS_4_X,FITHIS_8} FithisConnectivity;

/*! \enum fithisConnectedCommand
	\brief A command executed in the contet of Connected Regions

	The commands in this particular enum describe the commands that can be executed on an
	 image.
*/


typedef enum 
{
	FITHIS_ULTRA_SEGMENT,
	FITHIS_QUICK_SEGMENT,
	FITHIS_FILTER,
}fithisConnectedCommand;


typedef struct _FithisMemElem
{
	CvPoint pt;
	unsigned char *image_ptr;
	int *mask_ptr;
	int orient;
	_FithisMemElem* next;
} FithisMemElem;

typedef struct _FithisMosaic
{
	int seg_Id;
	CvScalar seg_colour;
	CvRect seg_desc;
	CvPoint gravity_centre;
	int foreground;
	_FithisMosaic *next;
} FithisMosaic;

/*! \class fithisConnectedRegions
    \brief A hopefully fast and versatile class for connected components.

    The fithisConnectedRegionsClass came up as a solution to the longstanding need for a
	versatile class implementing connected components functionality in the context of Polymnia.
	The principles behind it is efficiency, vesratility and ease of use.
*/
class  _declspec(dllexport) fithisConnectedRegions
{	
private : 
	unsigned char pixel_data[4];
	unsigned char temp_data[4];
	unsigned char **image_buff;
	int **mask_buff;
	IplImage *mask,* current_frame;
	FithisMosaic* working_seg_mosaic;
	FithisMosaic* cached_seg_mosaic;
	int label;
	int allocs;
	int threshold;
	bool underflow;
	int minArea;
	int area;
	IplImage *mask_selector;
	bool isSucc;
	int templates_size;
	CvPoint *templates;
	FithisConnectivity connectivity;
	FithisMemElem *working_route;
	FithisMemElem *cached_route;
	fithisConnectedCommand the_cmd;
public :
/*! \fn fithisConnectedRegions(IplImage *start_image)

	\brief The destructor of fithisConnectedRegions::fithisConnectedRegions

	This destructor constructs an instance of fithisConnectedRegions. It takes an image as an argument to 
	initialize internal data structures. It is not necessary for the subsequent images to be of this kind
	in height,width and channels.

	@param start_image an image for initialization purposes.All images must be IPL_DEPTH_8U
*/

	fithisConnectedRegions(IplImage *start_image);
	~fithisConnectedRegions();	

/*! \fn fithisExecuteCommand(fithisConnectedCommand cmd,int *threshold)

	\brief Execute the command on the image set by fithisConnectedRegions::setWorkingFrame. 

	When calling fithisConnectedRegions::setWorkingFrame we update the working image. We must include a threshold
	that controls when pixels are considered neighbors. In the case when we filter the image
	with the commands FITHIS_FILTER, FITHIS_DOMINANT, FITHIS_FILTER_SELECTED, we do not 
	have to include the threshold. 

	@param cmd a command to be executed fithisConnectedRegions::fithisConnectedCommand
*/
	void fithisExecuteCommand(fithisConnectedCommand cmd);

/*! \fn setConnectivity(FithisConnectivity conn)

	\brief Set the connectivity of the regions

	Currently only 4- and 8-connected regions are considered.
	@param conn the connectivity of regions see FithisConnectivity	
*/

	void setConnectivity(FithisConnectivity conn);

	/*! \fn getSegMosaic()

	\brief Get bounding rectangles for the components

	This function gives minimum bounding rectangles of the components as described in
	 the mask. The caller must copy this information but the structure returned must not be touched 
	 because  it is internally altered at subsequent calls to fithisConnectedRegions::fithisExecuteCommand. The series of
	 bounding rectangles is a NULL terminated linked list. 
	*/
	FithisMosaic * getSegMosaic();

	/*! \fn setWorkingFrame(IplImage * image)

	\brief Set the working frame for processing.

	The working frame must be updated always before calling fithisConnectedRegions::fithisExecuteCommand. Even if it is the same.

	@param image the image to be processed
	*/	
	void setWorkingFrame(IplImage * image);
/*! \fn setMinimumArea(int sz);

	\brief Set the minimum area of the regions

	Areas with size smaller or equal to fithisConnectedRegions::setMinimumArea are discarded (filtering) or absorbed (segmentation).
	@param sz the area bound
*/
	void setMinimumArea(int sz);

/*! \fn getMask()

	\brief Get the mask after processing

	The mask is a IPL_DEPTH_32U image with width and height equal to those of the last frame and 
	1 channel ,set by fithisConnectedRegions::setWorkingFrame. Every pixel has as a label the number of connected region it belongs.
	In case the pixel is equal to zero in case of filtering it means the image has a 0 pixel there or
	that the image belongs to a region with area smaller than the minimum area size fithisConnectedRegions::setMinimumArea. In the case of
	segmentation it must be non-zero.
*/

	IplImage * getMask(){return this->mask;}

	/*! \fn setIndicator(IplImage *an_indicator)

	\brief Set a mask for executing FITHIS_FILTER_SELECTED.

	The indicator should be an IPL_DEPTH_8U image with 1 channel and width/height 
	equal to the working frame fithisConnectedRegions::setWorkingFrame. If it is null, the call is equivalent to FITHIS_FILTER.
*/
	void setIndicator(IplImage *an_indicator);
	void setThreshold(int thresh){this->threshold=thresh;}
	int howManyComponents(){return this->label;}
	void compareRelative(bool val){this->isSucc=val;}
private :

	//memelement management
	FithisMemElem *newElement(CvPoint *pt);
	FithisMemElem *getNextPoint();
	void fithisUseMemElem(FithisMemElem *m);
	void fithisRecycleMemElem(FithisMemElem *m);	
	bool isCorrectIndex(CvPoint *pos);

	void getIt(unsigned char *ptr){for(int l=0;l<this->current_frame->nChannels;l++) this->pixel_data[l]=ptr[l];}
	void setIt(unsigned char *ptr){for(int l=0;l<this->current_frame->nChannels;l++) ptr[l]=this->pixel_data[l];}
	//almost-fast eliminates small regions, 
	//works with maximums or relatives	
	void fithisGrowConnectedRegion(CvPoint * pos);
	//very-fast taking into consideration relatives and eliminates
	//small regions
	void fithisGrowSegment(CvPoint * pos);
	//ultra-fast taking into consideration maximums, no elimination
	//is possible with dusts
	void fithisDust(CvPoint * pos);
	void fillConnectedRegionByMask(CvPoint* pos);
	//filters and image only on nonzero regions, if pixel is 
	//zero then mask is zero
	void fithisFilterConnectedRegion(CvPoint *pos);
	//Finds the mosaics of the image skipping zeros
	//and takes into consideration the inversion induced
	//fillConnectedRegionByMask
		
	bool isRelative(unsigned char *a,unsigned char *b);
		
	//mosaics
	void putBackMosaics();
	void giveMosaics(int num);
	void cleanUpMosaic();
	
	//managers
	void ResetManagers();
	void manageElement(FithisMemElem *m);
	void fillWhenFull(unsigned char * img_val,int* mask_val); //fill when the threshold on area is exceeded
};

#endif
