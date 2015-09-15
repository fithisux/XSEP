#ifndef bgUtilities_h
#define bgUtilities_h

#include <direct.h>	
#include <stdlib.h>
#include <stdio.h>

#include "cxcore.h"
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"

#include <vector>
#include<iostream>
#include<fstream>
using namespace std;

#include "XSEP.h"

typedef enum _NORMS { NORM_INFINITY,NORM_SQUARE,NORM_HSV,NORM_L1,NORM_SHADE} NORMS;

typedef struct _tri_info
{
	CvScalar flags;
	NORMS norm;
	bool use_hsv;
}tri_info;


#define abs_diff(x,y) ((x > y) ? (x-y) : (x-y))

/** \file bgUtilities.h
	\brief Various utilities for background subtraction and detection
*/

/**
\fn void createCircle(IplImage *frame,CvPoint *center_ptr,double radius)
\brief This function draws a circle on an image given its ceneter and radius
\param frame An IPL_DEPTH_8U image with 3 channels
\param center_ptr A pointer to a CvPoint containing the centre of the circle
\param radius The radius of the circle.
*/
   void createCircle(IplImage *frame,CvPoint *center_ptr,double radius);

DllExport  void  image_u8_rgb_diff(IplImage *img1,IplImage *img2,IplImage *img3,unsigned char thresh);
/**
\fn void deInterlace(IplImage *frame)
\brief This function de-interlaces an image by extending odd rows to even rows
\param frame The frame to de-interlace.
*/
   void deInterlace(IplImage *frame);

/**
\fn void cleverDeInterlace(IplImage *frame)
\brief This function de-interlaces an image by using  modified ELA algorithm
\param frame The frame to de-interlace.
*/
   void  cleverDeInterlace(IplImage *frame);

/**
\fn bool convertByte2Float(IplImage *byte_img,IplImage *float_img)
\brief This function converts a IPL_DEPTH_8U image to a IPL_DEPTH_32F image 
both images must have the same number of channels, the same width and the same height
\param byte_img The IPL_DEPTH_8U image
\param float_img The IPL_DEPTH_32F image
\return true on success false on failure. Failure means that both images do not 
have the same number of channels or the same width or the same height , or there is a
violation to the type specifiction of the images
*/
 void convertByte2Float(IplImage *byte_img,IplImage *float_img);

/**
\fn bool convertByte2Float(IplImage *byte_img,IplImage *float_img)
\brief This function converts a IPL_DEPTH_32F image to a IPL_DEPTH_8U image
\Both images must have the same number of channels, the same width and the same height
\param byte_img The IPL_DEPTH_8U image
\param float_img The IPL_DEPTH_32F image
\return true on success false on failure. Failure means that both images do not 
have the same number of channels or the same width or the same height
*/
 void convertFloat2Byte(IplImage *float_img,IplImage *byte_img); 

DllExport IplImage *  polymnia_crop_image(IplImage *src,CvRect *place);
DllExport void  polymnia_copy_crop_image(IplImage *src,CvRect *place,IplImage *dst);
 
/**
\fn int rgb_diff(unsigned char *a,unsigned char *b)
\brief This function implements safely (without conversions) the L^{infinity} distance between
two RGB vectors with 8 bits per color.
\param a The first vector
\param b The second vector
\return The norm
*/  
  void tri_image_diff(IplImage *img1,IplImage *img2,IplImage *t,double threshold,tri_info * diff_info);
/**
\fn float rgb_diff(float *a,float *b)
\brief This function implements safely (without conversions) the L^{infinity} distance between
two RGB vectors with 32bits (a float) per color.
\param a The first vector
\param b The second vector
\return The norm
*/

  void detect_sources(IplImage * img1,IplImage * img2,IplImage *injector);
  int sum_cyclic(IplImage *img,int runner);
  unsigned char u8_rgb_diff(unsigned char *a,unsigned char *b,int n);	
  void image_u8_rgb_add(IplImage *img1,IplImage *img2);

  double tri_diff(CvScalar *a,CvScalar *b,tri_info* diff_info);
  double rgb_tri_diff(CvScalar *a,CvScalar *b,tri_info * diff_info);
  double hsv_tri_diff(CvScalar *a,CvScalar *b,tri_info * diff_info);

  void fithis_hsv2rgb(CvScalar *a,CvScalar *b);
  void getValue(IplImage *image,unsigned char *a,CvScalar *val);
  void setValue(IplImage *image,unsigned char *a,CvScalar *val);
/**
\fn CvRect GetNonZeroRect(IplImage* in)
\brief This function returns a rectangle containing the non-zero pixels of the image
\param in The image
\return The bounding rectangle
*/
  DllExport CvRect  GetNonZeroRect(IplImage* in);

 double norm_infty(CvScalar *a,CvScalar *b);
 int identifyDataElement(IplImage*img);
 void compareRects(CvRect *rect1,CvRect *rect2,CvRect *rect_dst);
 CvSize getCoverage(vector<CvRect *>*rect_vec1,vector<CvRect *>*rect_vec2);
 inline int integral_floor(int a,int b);
 void block2smaller_32F(IplImage *in,IplImage *out,int block_size);
 void deBlock2bigger_8U(IplImage *in,IplImage *out,int block_size);
 void deBlock2bigger_32F(IplImage *in,IplImage *out,int block_size);
 unsigned char cyclic_abs_diff_8u(unsigned char x,unsigned char y);
 unsigned char abs_diff_8u(unsigned char x,unsigned char y); 
 DllExport int  polymnia_virtual2realAVI(const char *virt_avi,const char *real_avi);
 DllExport void  polymnia_clearvirtualAVI(const char *virt_avi);
#endif
