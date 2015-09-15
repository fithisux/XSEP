#include <stdio.h>
#include <iostream>
using namespace std;

#include "shadowclass.h"

shadowClass::shadowClass(separ_shadow_conf_t *some_conf)
{
	this->conf_shadow=some_conf;	
	this->filt= NULL;
	this->previous_frame=this->next_frame=
	this->deshaded=this->SHADOW_MASK=
	this->previous_separation=this->next_separation=NULL;
}

void shadowClass::shadow_identification_new(IplImage *frame,IplImage *separation) 
{
   //HERE STARTS THE PART FOR SHADOW DETECTION

	if(!this->next_separation)
	{
		this->next_separation = cvCloneImage(separation);
		this->previous_separation=cvCloneImage(separation);
		cvZero(this->previous_separation);
	}
	else
	{
		cvCopy(this->next_separation,this->previous_separation);
		cvCopy(separation,this->next_separation);
	}

	if(!this->next_frame)
	{
		this->next_frame = cvCloneImage(frame);
		cvCvtColor(frame,this->next_frame,CV_BGR2HSV);
		this->previous_frame=cvCloneImage(this->next_frame);		
	}
	else
	{
		cvCopy(this->next_frame,this->previous_frame);
		cvCvtColor(frame,this->next_frame,CV_BGR2HSV);
	}

	if(!this->SHADOW_MASK)
	{
		this->SHADOW_MASK = cvCloneImage(separation);
		this->deshaded = cvCloneImage(separation);
		this->filt=new fithisConnectedRegions(this->deshaded);
		filt->setMinimumArea(this->conf_shadow->maxArea);	
	}

	cvNot(this->previous_separation,this->deshaded);
	cvAnd(this->next_separation,this->deshaded,this->SHADOW_MASK);

	unsigned char *ptr1,*ptr2,*ptr3;
	unsigned char *runner1,*runner2,*runner3;
	
	ptr1=(unsigned char *)this->previous_frame->imageData;
	ptr2=(unsigned char *)this->next_frame->imageData;
	ptr3=(unsigned char *)SHADOW_MASK->imageData;

	for (int i=0;i<this->next_frame->height;i++)
	{
			runner1=ptr1;
			runner2=ptr2;
			runner3=ptr3;
			for (int j=0;j<this->next_frame->width;j++)
			{			
					if(*runner3)
					{
						*runner3=shadowidentification(runner1,runner2);
					}				
					runner3++;
					runner1+=3; // Increases 3
					runner2+=3; // Increases 3	
			}   
			ptr1+=this->previous_frame->widthStep;
			ptr2+=this->next_frame->widthStep;
			ptr3+=SHADOW_MASK->widthStep;
	}
	
	     	    
	//Maybe fithisConnectedRegions delays the real time process?
	this->filt->setWorkingFrame(this->SHADOW_MASK);
	for(int t=0;t<this->conf_shadow->filterrun;t++)	filt->fithisExecuteCommand(FITHIS_FILTER);
	cvNot(this->SHADOW_MASK,this->deshaded);
	cvAnd(this->next_separation,this->deshaded,this->deshaded);
	this->filt->setWorkingFrame(this->deshaded);
	for(int t=0;t<this->conf_shadow->filterrun;t++)	filt->fithisExecuteCommand(FITHIS_FILTER);
}

unsigned char shadowClass::shadowidentification(unsigned char *a,unsigned char *b)
{	
	double x,y;

	if(cyclic_abs_diff_8u(*a++,*b++)<=this->conf_shadow->rH)
	{
		if(abs_diff_8u(*a++,*b++)<=this->conf_shadow->rS)
		{	
			x=*a;
			y=*b;
			if( (x * this->conf_shadow->alpha <= y) && 
				(y <= x * this->conf_shadow->beta)) return 255;
		}
	}
	
	return 0;
}


shadowClass::~shadowClass()
{
		if(this->next_frame) cvReleaseImage( &this->next_frame);
		if(this->previous_frame) cvReleaseImage( &this->previous_frame);
		if(this->next_separation) cvReleaseImage( &this->next_separation);
		if(this->previous_separation) cvReleaseImage( &this->previous_separation);
		if(this->SHADOW_MASK) cvReleaseImage(&this->SHADOW_MASK);
		if(this->deshaded)  cvReleaseImage(&this->deshaded);
		this->conf_shadow=NULL;
		if(this->filt) delete this->filt;
}

IplImage * shadowClass::getRealFG()
{
	return this->deshaded;
}
IplImage * shadowClass::getShadow()
{
	return this->SHADOW_MASK;
}
