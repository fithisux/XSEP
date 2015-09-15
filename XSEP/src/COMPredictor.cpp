#include "COMPredictor.h"
//
// NTUA predictor implementation
//

COMPredictor::COMPredictor(separ_pixellevel_conf_t *params) : PixelLevel(params)
{	
	weight_pool=new boost::pool<>(sizeof(center_of_mass_cluster_t));
	this->com_model=params->com_model;
	this->com_conf.force_threshold=this->com_model->force_threshold;
	this->com_conf.rarity_threshold=this->com_model->rarity_threshold;
	this->com_conf.com_pool=this->weight_pool;
	this->background_features=this->background_features=NULL;
	this->ready=0;
	this->timer=0;
	this->new_frame=NULL;
	this->firstTime=true;
}

COMPredictor::~COMPredictor()
{
#ifdef COMP_LOG
	printf("Delete COM_PREDICTOR \n");
#endif
  	cvReleaseImage(&this->new_frame);
#ifdef COMP_LOG
	printf("Delete series \n");
#endif
	for(int i=0;i<this->numOfClusters;i++) destroy_center_of_mass_array(&this->background_features[i]);
#ifdef COMP_LOG
	printf("Series deleted \n");
#endif

	delete[] this->background_features;
	this->background_features=NULL;	
	weight_pool->release_memory();
	delete weight_pool;
}

//test if ready
bool COMPredictor::isReady()
{
	return (this->ready==1);
}

//updates with frames until ready
void COMPredictor::updateWithFrame(IplImage* frame)
{	
	PixelLevel::updateWithFrame(frame);
	if(this->firstTime) this->initializeAll(frame);

#ifdef COMP_LOG
	printf("Updating SOM \n");
#endif
			
	if(this->goskip == this->pixellevel_params->skip)
	{		
		if(this->firstTime)
		{
			 this->firstTime=false;
			 this->ready=1;

		}
		else this->mustTikTakToe();
		if(!this->new_frame) this->new_frame=cvCreateImage(cvGetSize(this->current_frame),IPL_DEPTH_32F,this->current_frame->nChannels);
		convertByte2Float(this->current_frame,this->new_frame);					
		this->convertToVector();		
	}
}

void COMPredictor::mustTikTakToe()
{
#ifdef COMP_LOG
	printf("TikTakToe @ time %d \n",this->timer++);
#endif			
	for(int i=0;i<this->numOfClusters;i++) tiktaktoe_center_of_mass_array(this->background_features[i]);	
}

void COMPredictor::predict()
{
#ifdef COMP_LOG
	printf("Predicting with SOM \n");
#endif
	
	this->computePrediction();
	cvCopy(this->current_frame,this->prediction,this->foreground);
	this->computeForeground(false);
}

void COMPredictor::initializeAll(IplImage *frame)
{	
#ifdef COMP_LOG
	printf("COM Initialization done \n");
#endif				
	
	this->new_frame=cvCreateImage(cvGetSize(frame),IPL_DEPTH_32F,frame->nChannels);	
	this->numOfClusters=frame->width*frame->height;	
	this->background_features = new center_of_mass_array_t*[this->numOfClusters];		
	for(int i=0;i<this->numOfClusters;i++) this->background_features[i]= create_center_of_mass_array(&this->com_conf);
}

void COMPredictor::convertToVector()
{
	unsigned char  *runner2;
	float  *ptr1=(float  *) this->new_frame->imageData;		
	unsigned char  *ptr2=(unsigned char  *) this->foreground->imageData;		
	CvScalar temp=cvScalarAll(0);
	bool resus;
	int k=0;
	for(int i=0;i<this->foreground->height;i++)
	{
		runner2=ptr2;
		for(int j=0;j<this->foreground->width;j++)
		{			
			for(int l=0;l<this->new_frame->nChannels;l++) temp.val[l]= *ptr1++;			
			resus=add_value_center_of_mass_array(this->background_features[k++],&temp);
			*runner2++= ( (resus) ? 255 : 0);			
		}
		ptr2+=this->foreground->widthStep;		
	}
}
