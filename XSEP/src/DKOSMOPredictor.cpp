#include "DKOSMOPredictor.h"
#include <stdio.h>
#include <iostream>

using namespace std;
//
// NTUA predictor implementation
//

DKOSMOPredictor::DKOSMOPredictor(separ_pixellevel_conf_t *params) : PixelLevel(params)
{		
	this->base_conf.threshold=params->params->diffthreshold;
	this->base_conf.diff_info=&this->my_info;	
	this->fg_core_conf= createCCConf(params->dkosmo_model->theK,params->dkosmo_model->theK,&this->base_conf);
	this->bg_core_conf= createCCConf(params->dkosmo_model->theN,params->dkosmo_model->theK,&this->base_conf);
	this->new_frame=NULL;
	this->features=NULL;	
	this->ready=0;
	this->dkosmo_firstTime=true;
	//this->my_info.norm=NORM_L1;
	this->counter=params->dkosmo_model->theN;
}

DKOSMOPredictor::~DKOSMOPredictor()
{
	for(int i=0;i<this->new_frame->height;i++)
	{		
		for(int j=0;j<this->new_frame->width;j++)destroyCCAggregate(&this->features[i][j]);		
	}			 

	for(int i=0;i<this->new_frame->height;i++)
	{
		  	delete[] this->features[i];
	  	  	delete[] this->tempVector[i];			
	}

  	delete[] this->features;
  	delete[] this->tempVector;
	destroyCCConf(&this->bg_core_conf);
	destroyCCConf(&this->fg_core_conf);
	if(this->new_frame) cvReleaseImage(&this->new_frame);
	printf("dkosmo des \n");
}

//test if ready
bool DKOSMOPredictor::isReady()
{
	return (this->ready==1);
}


//updates with frames until ready
void DKOSMOPredictor::updateWithFrame(IplImage* frame)
{	
	PixelLevel::updateWithFrame(frame);
	if(!this->new_frame){this->initializeAll(frame);}
	if(this->goskip == this->pixellevel_params->skip)
	{		
		convertByte2Float(this->current_frame,this->new_frame);		
		this->convertToVector();
		this->classifyIt();
		if(this->counter>0)
		{				
			if(--this->counter==0) this->ready=1;
			printf("we are ready \n");
		}		
	}
}


void DKOSMOPredictor::classifyIt()
{	
	bool resus;

	unsigned char * ptr= (unsigned char *)this->foreground->imageData;
	unsigned char * runner;

	for(int i=0;i<this->foreground->height;i++)
	{
		runner=ptr;
		for(int j=0;j<this->foreground->width;j++)
		{
			resus=classifyCCAggregate(this->features[i][j],&this->tempVector[i][j]);
			*runner++= ( (resus) ? 255 : 0);
		}
		ptr+=this->foreground->widthStep;
	}
}


//compute the prediction
void DKOSMOPredictor::computePrediction()
{
	CvScalar val;
	unsigned char *ptr=(unsigned char *) this->prediction->imageData;
	unsigned char * runner;
	for(int i=0;i<this->prediction->height;i++)
	{
		runner=ptr;
		for(int j=0;j<this->prediction->width;j++)
		{			
			predictCCAggregate(this->features[i][j],&val);
			for(int l=0;l<this->new_frame->nChannels;l++) *runner++= (unsigned char) val.val[l];
		}
		ptr+=this->prediction->widthStep;
	}
	if(this->my_info.use_hsv) cvCvtColor(this->prediction,this->prediction,CV_HSV2BGR);
}

void DKOSMOPredictor::predict()
{
	this->computePrediction();	
	this->computeForeground(false);
}

void DKOSMOPredictor::initializeAll(IplImage *frame)
{	
	printf("initialize it again \n");
	if(!this->new_frame) this->new_frame=cvCreateImage(cvGetSize(frame),IPL_DEPTH_32F,frame->nChannels);		
	this->features = new condensationCoreAggregate**[frame->height];	
	this->tempVector = new  CvScalar* [frame->height];
	
	for(int i=0;i<frame->height;i++)
	{
		this->features[i]=new condensationCoreAggregate*[frame->width];
	  	this->tempVector[i]=new CvScalar[frame->width];	  	  			
	}
	
	for(int i=0;i<frame->height;i++)
	{
		for(int j=0;j<frame->width;j++)
		{
			this->features[i][j] = createCCAggregate(this->bg_core_conf,this->fg_core_conf);
		}
	}
}

void DKOSMOPredictor::convertToVector()
{
	float  *ptr=(float  *) this->new_frame->imageData;	
	for(int i=0;i<this->new_frame->height;i++)
	{
		for(int j=0;j<this->new_frame->width;j++)
		{
			for(int l=0;l<this->new_frame->nChannels;l++)
			{
				tempVector[i][j].val[l]=*ptr++;
			}
		}
	}
}
