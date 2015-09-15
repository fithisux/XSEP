#include "SOMPredictor.h"
//
// NTUA predictor implementation
//
#include "highgui.h"

SOMPredictor::SOMPredictor(separ_pixellevel_conf_t *params) : PixelLevel(params)
{	
	weight_pool=new boost::pool<>(sizeof(som_temporal_weight_t));
	this->som_conf.fg_class_threshold=params->som_model->fg_class_threshold;
	this->som_conf.rarity_threshold=params->som_model->rarity_threshold;
	this->som_conf.generating_threshold=params->som_model->generating_threshold;
	this->som_conf.joining_threshold=params->params->diffthreshold;
	this->som_conf.irrelevance_threshold=this->som_conf.joining_threshold;
	this->som_conf.irrelevance_threshold /= (this->som_conf.generating_threshold +2);
	this->som_conf.the_A=params->som_model->the_A;
	this->som_conf.the_B=params->som_model->the_B;
	this->som_conf.weight_pool=weight_pool;	
	this->background_features=NULL;	
	this->ready=0;
	this->new_frame=NULL;
	this->firstTime=true;
	this->timer=som_create_timer(&this->som_conf);		
}

SOMPredictor::~SOMPredictor()
{
#ifdef SOMP_LOG
	printf("Delete SOM_PREDICTOR \n");
#endif

   cvReleaseImage(&this->new_frame);

#ifdef SOMP_LOG
	printf("Delete series \n");
#endif

	for(int i=0;i<this->numOfClusters;i++)som_release_series(&this->background_features[i]);

#ifdef SOMP_LOG
	printf("Series deleted \n");
#endif

	delete[] this->background_features;
	this->background_features=NULL;

	som_release_timer(&this->timer);	
	weight_pool->release_memory();
	delete weight_pool;
}

//test if ready
bool SOMPredictor::isReady()
{
	return (this->ready==1);
}

//updates with frames until ready
void SOMPredictor::updateWithFrame(IplImage* frame)
{	
	PixelLevel::updateWithFrame(frame);
	if(!this->new_frame) this->initializeAll(frame);

#ifdef SOMP_LOG
	printf("Updating SOM \n");
#endif
			
	if(this->goskip == this->pixellevel_params->skip)
	{		
		this->ready=1;
		convertByte2Float(this->current_frame,this->new_frame);		
		this->convertToVector();
		this->mustTikTakToe();
	}
}

void SOMPredictor::mustTikTakToe()
{
#ifdef SOMP_LOG
	printf("TikTakToe @ time %d \n",this->timer->time);
#endif		
	som_increase_timer(this->timer);
	som_temporal_series_t**cores;
	cores=this->background_features;
	for(int i=0;i<this->numOfClusters;i++)som_remove_stale_weights(*cores++);
}

void SOMPredictor::predict()
{
#ifdef SOMP_LOG
	printf("Predicting with SOM \n");
#endif
	
	this->computePrediction();
	cvCopy(this->current_frame,this->prediction,this->foreground);
	this->computeForeground(false);
}

void SOMPredictor::initializeAll(IplImage *frame)
{	
#ifdef SOMP_LOG
	printf("SOM Initialization done \n");
#endif					
	this->new_frame=cvCreateImage(cvGetSize(frame),IPL_DEPTH_32F,frame->nChannels);	
	this->numOfClusters=frame->width*frame->height;	
	this->background_features = new som_temporal_series_t*[this->numOfClusters];		
	int k=0;
	for(int i=0;i<this->foreground->height;i++)
	{
		for(int j=0;j<this->foreground->width;j++)
		{
			this->background_features[k++] = som_create_series(this->timer,&this->my_info,j,i);
		}
	}
}

void SOMPredictor::convertToVector()
{
	unsigned char  *runner2;
	float  *ptr1;		
	unsigned char  *ptr2;	
	som_temporal_series_t**cores;
	som_temporal_weight_t *weight;
	CvScalar temp=cvScalarAll(0);
	bool resus;

	ptr1=(float  *) this->new_frame->imageData;
	ptr2=(unsigned char  *) this->foreground->imageData;
	cores=this->background_features;
	for(int i=0;i<this->foreground->height;i++)
	{
		runner2=ptr2;
		for(int j=0;j<this->foreground->width;j++)
		{			
			for(int l=0;l<this->new_frame->nChannels;l++) temp.val[l]= *ptr1++;					

			resus=som_add_value(*cores,&temp);
			/*
			if( (j==125) && (i==240) )
			{
				printf("Go\n");
				for(int l=0;l<this->new_frame->nChannels;l++) printf(" %f ",temp.val[l]);
				printf("Length %d \n",(*cores)->length);
				if(resus) printf("Result true \n");else printf("Result false \n");
				printf("\n");
			}
			*/
			cores++;
			*runner2++= ( (resus) ? 255 : 0);			
		}
		ptr2+=this->foreground->widthStep;		
	}
}

