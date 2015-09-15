#ifndef face_detector_h
#define face_detector_h

#include "bgUtilities.h"
#include "confParametersDefs.h"

class face_detect_ctx
{
	IplImage *temp_storage;
	IplImage* small_image;	
	separ_facedetection_conf_t *faceconf;
	int scale;	
	CvHaarClassifierCascade* cascade;
	CvSize face_shape;
	bool do_pyramids;
public :
	vector<CvRect *>*face_vector;
	vector<CvRect *>*body_vector;
	int size;

	face_detect_ctx(separ_facedetection_conf_t *afaceconf,bool pyramdo);
	void updateWithImageAndRect(IplImage *some_image,CvRect * place);
	void detect_and_mark_humans(CvRect *place);
	bool reduceFalsePositives(CvRect *some_rect);
	~face_detect_ctx();
};

#endif
