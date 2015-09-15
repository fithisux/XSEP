#include "MOGPredictor.h"
#include <stdio.h>
#include <iostream>

using namespace std;
//
// FrameWiener predictor implementation
//

MOGPredictor::MOGPredictor(separ_pixellevel_conf_t *params) : PixelLevel(params)
{	
	this->bg_model=NULL;
	this->ready=0;
	
	this->mog_model = (CvGaussBGStatModelParams *) params->mog_model;
	this->mog_model->minArea=this->pixellevel_params->minArea;
}

MOGPredictor::~MOGPredictor()
{
    cvReleaseBGStatModel( &this->bg_model );
    delete this->mog_model;
    this->mog_model=NULL;
}

//test if ready
bool MOGPredictor::isReady()
{
	return (this->ready==1);
}

//updates with frames until ready
void MOGPredictor::updateWithFrame(IplImage* frame)
{	
	PixelLevel::updateWithFrame(frame);
	if(!bg_model)
	{
			bg_model = cvCreateGaussianBGModel(frame,this->mog_model);
#ifdef MOG_LOG
			printf("Initializing model \n");
#endif
			this->ready=1;
	}
	if(this->goskip == this->pixellevel_params->skip)
	{		
#ifdef MOG_LOG
	printf("Updating \n");
#endif				
		cvUpdateBGStatModel(frame,this->bg_model);
	}
}


//compute the prediction
void MOGPredictor::computePrediction(){}

void MOGPredictor::predict()
{
	if(this->ready==1)
	{
#ifdef MOG_LOG
	printf("Predicting \n");
#endif		
		cvCopy(this->bg_model->background,this->prediction);
		cvThreshold(this->bg_model->foreground,this->foreground,0,255,CV_THRESH_BINARY);
		PixelLevel::computeForeground(false);
	}
}

