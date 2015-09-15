#include "NTUAPredictor.h"
#include <stdio.h>
#include <iostream>

using namespace std;
//
// NTUA predictor implementation
//

NTUAPredictor::NTUAPredictor(separ_pixellevel_conf_t *params) : PixelLevel(params)
{	
	this->ntua_model=params->ntua_model;
	this->old_frame=this->new_frame=this->temp_diff=NULL;
	this->history = new IplImage* [this->ntua_model->pastN]; //aray of past frames	
	this->ready=0;
	this->counter=this->ntua_model->pastN;	
}

NTUAPredictor::~NTUAPredictor()
{
    int i;
	cvReleaseImage(&this->new_frame);
	cvReleaseImage(&this->old_frame);
	cvReleaseImage(&this->temp_diff);

	for(i=0;i<this->ntua_model->pastN;i++)
	{		
		if(this->history[i]) cvReleaseImage(&this->history[i]);	
	}
	delete [] this->history;	
}

//test if ready
bool NTUAPredictor::isReady()
{
	return (this->ready==1);
}

//updates with frames until ready
void NTUAPredictor::updateWithFrame(IplImage* frame)
{	
	PixelLevel::updateWithFrame(frame);
	IplImage *swap;
	if(!this->temp_diff) this->initializeAll(frame);
	if(this->goskip == this->pixellevel_params->skip)
	{
		if(!this->old_frame)
		{
			this->old_frame=cvCloneImage(this->current_frame);
#ifdef FP_LOG
	printf("the old \n");
#endif
		}
		else
		{
			if(!this->new_frame)
			{
				this->new_frame=cvCloneImage(this->current_frame);
#ifdef FP_LOG
	printf("the new \n");
#endif
			}
			else
			{
#ifdef FP_LOG
	printf("swapping \n");
#endif
				swap=this->old_frame;
				this->old_frame=this->new_frame;
				this->new_frame=swap;
				cvCopy(this->current_frame,this->new_frame);
			}

#ifdef FP_LOG
	printf("diff \n");
#endif
			//tri_image_diff(this->new_frame,this->old_frame,this->foreground,this->ntua_model->diffthreshold,&this->my_info);
			image_u8_rgb_diff(this->new_frame,this->old_frame,this->foreground,this->pixellevel_params->diffthreshold);
			cvThreshold(this->foreground,this->temp_diff,0,1,CV_THRESH_BINARY);
			if(this->counter > 0)
			{		
				cvCopy(this->temp_diff,this->history[--this->counter]);
				if(this->counter==0) this->ready=1;
				else return;
			}	
			else
			{
#ifdef FP_LOG
	printf("Push \n");
#endif
				this->pushFront(this->temp_diff,this->history);	
			}
		}
	}
}

void NTUAPredictor::pushFront(IplImage *some_frame,IplImage **past)
{
		//we push the new frame in place 0
		// the others are shifted to the past by 1
		IplImage *temp=past[this->ntua_model->pastN-1];
		for(int i=this->ntua_model->pastN-2;i>=0;i--) past[i+1]=past[i];
		cvCopy(some_frame,temp);
		past[0]=temp;
}

//compute the prediction
void NTUAPredictor::computePrediction()
{	
	//find k-stable forgrounds
	cvZero(this->temp_diff);
	for(int k=0;k<this->ntua_model->pastN;k++) image_u8_rgb_add(this->temp_diff,this->history[k]);
	cvThreshold(this->temp_diff,this->temp_diff,this->ntua_model->stableK,255,CV_THRESH_BINARY_INV);
	//filter from current foreground the k-stable
	cvAnd(this->foreground,this->temp_diff,this->foreground);	
}


void NTUAPredictor::predict()
{
#ifdef FP_LOG
	printf("Predicting \n");
#endif
	this->computePrediction();	
	cvCopy(this->new_frame,this->prediction,this->foreground);
	this->computeForeground(false);
}

void NTUAPredictor::initializeAll(IplImage *frame)
{		
	this->temp_diff=cvCloneImage(this->foreground);		
	for(int i=0;i<this->ntua_model->pastN;i++)
	{
		this->history[i]=cvCloneImage(this->foreground);
	}
}
