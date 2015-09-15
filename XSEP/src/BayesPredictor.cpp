#include "BayesPredictor.h"
#include <stdio.h>
#include <iostream>

using namespace std;
//
// FrameWiener predictor implementation
//

BayesPredictor::BayesPredictor(separ_pixellevel_conf_t *params) : PixelLevel(params)
{	
	this->bg_model=NULL;
	this->ready=0;	
	this->bayes_model = (CvFGDStatModelParams *) params->bayes_model;
	this->bayes_model->minArea=params->params->minArea;
}

BayesPredictor::~BayesPredictor()
{
    cvReleaseBGStatModel( &this->bg_model );    
    delete this->bayes_model;
    this->bayes_model=NULL;
}

//test if ready
bool BayesPredictor::isReady()
{
	return (this->ready==1);
}

//updates with frames until ready
void BayesPredictor::updateWithFrame(IplImage* frame)
{	
	PixelLevel::updateWithFrame(frame);
	if(!bg_model)
	{
			bg_model = cvCreateFGDStatModel(frame,this->bayes_model);
#ifdef BAYES_LOG
			printf("Initializing model \n");
#endif
			this->ready=1;
	}
	if(this->goskip == this->pixellevel_params->skip)
	{		
#ifdef BAYES_LOG
	printf("Updating \n");
#endif				
		cvUpdateBGStatModel(frame,this->bg_model);
	}
}


//compute the prediction
void BayesPredictor::computePrediction(){}

void BayesPredictor::predict()
{
	if(this->ready==1)
	{
#ifdef BAYES_LOG
	printf("Predicting \n");
#endif		
		cvCopy(this->bg_model->background,this->prediction);
		cvThreshold(this->bg_model->foreground,this->foreground,1,255,CV_THRESH_BINARY);
		PixelLevel::computeForeground(false);
	}
}

