#include "subtraction.h"
#include "cxcore.h"

#include "PixelLevel.h"
#include "BayesPredictor.h"
#include "MOGPredictor.h"
#include "NTUAPredictor.h"
#include "DKOSMOPredictor.h"
#include "SOMPredictor.h"
#include "COMPredictor.h"

DLLEXPORT subtraction::subtraction(const char *xml_file)
{
	try
	{
		params= new confParameters(xml_file);
	}
	catch(std::string s)
	{
		throw s;
		cout << s;
	}
	cout << "Initializing the rest \n";
	this->face_vector=new vector<CvRect *>(0);
	this->body_vector=new vector<CvRect *>(0);
	this->face_body_masks=NULL;
	this->filt=NULL;
	this->face_filt=NULL;
	this->ready=false;
	this->isFinished=false;
	this->agrabber=NULL;
	this->timed_frame=NULL;	
	this->region_level_frame=NULL;

	//Putting parameters
	if(params->conf_device->avi_conf) this->agrabber=new frameGrabber(params->conf_device->avi_conf);
	else if(params->conf_device->sequence_conf) this->agrabber=new frameGrabber(params->conf_device->sequence_conf);
	else this->agrabber=new frameGrabber(params->conf_device->camera_conf);
		
	this->raw_frame=this->processed_frame=
	this->prediction_frame=this->foreground_frame=
	this->internal_foreground_frame=this->final_frame=
	this->face_estimation=NULL;
	
	this->processed_frame_splitter=this->raw_frame_splitter=NULL;

	if(params->conf_pixellevel->mog_model)
	{
		cout << "MOGPredictor selected \n";
		this->apred = dynamic_cast<PixelLevel *> (new MOGPredictor(params->conf_pixellevel));
	}
	else if(params->conf_pixellevel->ntua_model)
	{
		cout << "NTUAPredictor selected \n";		
		this->apred = dynamic_cast<PixelLevel *> (new NTUAPredictor(params->conf_pixellevel));
	}
	else if(params->conf_pixellevel->bayes_model)
	{
		cout << "BAYESPredictor selected \n";
		this->apred = dynamic_cast<PixelLevel *> (new BayesPredictor(params->conf_pixellevel));
	}
	else if(params->conf_pixellevel->dkosmo_model)
	{
		cout << "DKOSMOPredictor selected \n";
		this->apred = dynamic_cast<PixelLevel *> (new DKOSMOPredictor(params->conf_pixellevel));
	}
	else if(params->conf_pixellevel->som_model)
	{
		cout << "SOMPredictor selected \n";
		this->apred = dynamic_cast<PixelLevel *> (new SOMPredictor(params->conf_pixellevel));
	}
	else if(params->conf_pixellevel->com_model)
	{
		cout << "COMPredictor selected \n";
		this->apred = dynamic_cast<PixelLevel *> (new COMPredictor(params->conf_pixellevel));
	}
	else
	{
		printf("Invalid algorithm. Aborting ... \n");
		exit(0);
	}
		
	this->facedet=NULL;
	if(params->conf_facedetection)
	{
		this->facedet=new face_detect_ctx(params->conf_facedetection,false);
	}
	
	this->region_level=NULL;
	this->face_level=NULL;
	
	if(params->conf_regionlevel)
	{
		printf("Region lenel ON!!! \n");
		this->region_level=new contourDetector(params->conf_regionlevel);
		if(params->conf_facelevel) this->face_level=new contourDetector(params->conf_regionlevel);
	}
	else printf("Region lenel OFF!!! \n");
				
	this->deshade=NULL;
	if(params->conf_shadow)
	{
		printf("DeShadow ON!!! \n");
		this->deshade=new shadowClass(params->conf_shadow);
	}
	else printf("DeShadow OFF!!! \n");

	printf("subtraction LAUNCHED!!!");
	this->counter=-1;

	if(params->conf_facelevel)
	{
		this->face_hist_sizes=new int[3];
		this->face_hist_ranges=new float *[3];
		for(int l=0;l<3;l++)
		{
			this->face_hist_sizes[l]=params->conf_facelevel->numbins;
			this->face_hist_ranges[l]=new float[2];
			this->face_hist_ranges[l][0]=0;
			this->face_hist_ranges[l][1]=255;
		}
		this->face_hist=cvCreateHist(3,this->face_hist_sizes,CV_HIST_ARRAY,this->face_hist_ranges,1);
	}
	else
	{
		this->face_hist=NULL;
		this->face_hist_sizes=NULL;
		this->face_hist_ranges=NULL;	
	}
}

DLLEXPORT bool subtraction::nextFrame()
{	
	this->extraProcess();
#ifdef SUBTRACTION_LOG
	cout << "Frame " << this->counter <<" "<<this->isFinished<<endl;
#endif

	if(this->isFinished)
	{
		this->ready=false;
		return false;
	}	
#ifdef SUBTRACTION_LOG
	printf("Frame number %d \n",this->counter);	
#endif	
#ifdef SUBTRACTION_LOG
	printf("Executing pixel level \n");
#endif

	apred->updateWithFrame(this->processed_frame);
	
	if(!apred->isReady())
	{
#ifdef SUBTRACTION_LOG
		printf("Predictor not ready \n");
#endif
		return true;
	}
	apred->predict();		

	if(!this->internal_foreground_frame)
	{
		this->internal_foreground_frame = cvCreateImage(cvGetSize(this->processed_frame),IPL_DEPTH_8U,1);
	}
	cvCopy(apred->foreground,this->internal_foreground_frame);	

	IplImage *deshaded_frame;
	if(this->deshade) 
	{
		this->deshade->shadow_identification_new(this->processed_frame,apred->foreground);
		deshaded_frame=this->deshade->getRealFG();
		cvCopy(deshaded_frame,this->internal_foreground_frame);
	}

	if(!this->face_estimation) this->face_estimation=cvCloneImage(this->internal_foreground_frame);
	if(params->conf_facelevel) this->getFaceEstimation();
	if(this->region_level)
	{
#ifdef SUBTRACTION_LOG
	printf("Present region level");
#endif
		region_level->updateWithFrameAndMarked(this->processed_frame,this->internal_foreground_frame);
		if(!this->region_level_frame) this->region_level_frame=cvCloneImage(this->internal_foreground_frame);
		cvCopy(this->region_level->current_separation,this->internal_foreground_frame);	
		if(params->conf_facelevel)
		{
			face_level->updateWithFrameAndMarked(this->processed_frame,this->face_estimation);
			cvCopy(this->face_level->current_separation,this->face_estimation);
		}
	}
	else
	{
#ifdef SUBTRACTION_LOG
	printf("Not present region level");
#endif
		//cvResize(apred->prediction,this->prediction_frame);
	}

	if(params->conf_facelevel) cvOr(this->internal_foreground_frame,this->face_estimation,this->internal_foreground_frame);	
	if(!this->foreground_frame)
	{
		this->foreground_frame=cvCreateImage(cvGetSize(this->raw_frame),IPL_DEPTH_8U,1);
	}

	cvResize(this->internal_foreground_frame,this->foreground_frame,CV_INTER_LINEAR);
	cvThreshold(this->foreground_frame,this->foreground_frame,0,255,CV_THRESH_BINARY);
	
	if(!this->prediction_frame) this->prediction_frame=cvCloneImage(this->raw_frame);
	cvZero(this->prediction_frame);
	cvCopy(this->raw_frame,this->prediction_frame,this->foreground_frame);	

	ready=true;	
	return true;
}

DLLEXPORT void subtraction::getParts()
{
	vector<CvRect *> *face_vec;
	vector<CvRect *> *body_vec;
	vector<CvRect *>::iterator it1,it2;
	CvRect *rect;	

	if(!this->face_filt)
	{
		this->face_filt = new fithisConnectedRegions(this->foreground_frame);			
		this->face_filt->setMinimumArea(1);
	}
	else this->face_filt->setWorkingFrame(this->foreground_frame);
	this->face_filt->fithisExecuteCommand(FITHIS_FILTER);
	this->face_filt->fithisExecuteCommand(FITHIS_FILTER);

	FithisMosaic *runner=this->face_filt->getSegMosaic();
	
/*
	FithisMosaic *temp=runner;
	IplImage *x=cvCloneImage(this->foreground_frame);	
	cvZero(x);
	while(temp)
	{
		cvSetImageROI(x,temp->seg_desc);
		cvSet(x,cvScalarAll(255));
		cvResetImageROI(x);
		temp=temp->next;
	}
	cvCopy(x,this->foreground_frame);
	cvReleaseImage(&x);
	return;
*/

#ifdef SUBTRACTION_LOG
		printf("checking for existing humans \n");
#endif

	while(!this->face_vector->empty()) this->face_vector->pop_back();
	while(!this->body_vector->empty()) this->body_vector->pop_back();
	if(!this->facedet) return;
	face_vec=this->facedet->face_vector;
	body_vec=this->facedet->body_vector;
	
	while(runner)
	{
#ifdef SUBTRACTION_LOG
		printf("searching for face \n");
#endif		
		this->facedet->updateWithImageAndRect(this->raw_frame,&runner->seg_desc);
#ifdef SUBTRACTION_LOG
		printf("searching completed, arranging \n");
#endif
		for(int kk=0;kk<this->facedet->size;kk++)
		{
			this->face_vector->push_back(face_vec->at(kk));
			this->body_vector->push_back(body_vec->at(kk));
		}
		cvRectangle(this->prediction_frame,
						cvPoint(runner->seg_desc.x,runner->seg_desc.y),
						cvPoint(runner->seg_desc.x+runner->seg_desc.width,
								runner->seg_desc.y+runner->seg_desc.height),
						CV_RGB(0,0,255),3);		
		//Filter only real humans
#ifdef SUBTRACTION_LOG
		printf("Filter real humans \n");
#endif
		runner=runner->next;
	}
	
	if(!this->face_body_masks)
	{
		this->face_body_masks = cvCreateImage(cvGetSize(this->raw_frame),IPL_DEPTH_8U,1);
	}
	cvZero(this->face_body_masks);

	for(it1=this->face_vector->begin(),it2=this->body_vector->begin();it1!=this->face_vector->end();)
	{
		rect = *it1++;
		cvSetImageROI(this->face_body_masks,*rect);
		cvSet(this->face_body_masks,cvScalarAll(255));
		rect = *it2++;
		cvSetImageROI(this->face_body_masks,*rect);
		cvSet(this->face_body_masks,cvScalarAll(255));					
	}
	cvResetImageROI(this->face_body_masks);
	cvAnd(this->face_body_masks,this->foreground_frame,this->face_body_masks);

#ifdef SUBTRACTION_LOG
		printf("recover the faces \n");
#endif

	//Recover fully the faces
	for(it1=this->face_vector->begin();it1!=this->face_vector->end();)
	{
		rect = *it1++;
		cvSetImageROI(this->face_body_masks,*rect);
		cvSet(this->face_body_masks,cvScalarAll(255));									
	}
	cvResetImageROI(this->face_body_masks);
	if(!this->final_frame) this->final_frame=cvCloneImage(this->raw_frame);
	cvZero(this->final_frame);	
	cvCopy(this->raw_frame,this->final_frame,this->face_body_masks);	
}

void subtraction::getFaceEstimation()
{
	if(!params->conf_facelevel) return;

	vector<CvRect *>::iterator it1;	
	
	if(!this->raw_frame_splitter)
	{
		this->raw_frame_splitter=new IplImage *[3];
		for(int l=0;l<3;l++)
		{
			this->raw_frame_splitter[l]=cvCreateImage(cvGetSize(this->raw_frame),IPL_DEPTH_8U,1);
		}
	}

	if(!this->processed_frame_splitter)
	{
		this->processed_frame_splitter=new IplImage *[3];
		for(int l=0;l<3;l++)
		{
			this->processed_frame_splitter[l]=cvCreateImage(cvGetSize(this->processed_frame),IPL_DEPTH_8U,1);
		}
	}

	CvRect *rect;
	
	if(this->face_vector->size()>0)
	{
		cvZero(this->face_body_masks);
		cvSplit(this->raw_frame,
				this->raw_frame_splitter[0],
				this->raw_frame_splitter[1],
				this->raw_frame_splitter[2],NULL);

		for(it1=this->face_vector->begin();it1!=this->face_vector->end();)
		{
			rect = *it1++;
			cvSetImageROI(this->face_body_masks,*rect);
			cvSet(this->face_body_masks,cvScalarAll(255));	
			cvResetImageROI(this->face_body_masks);
		}		
		cvAnd(this->face_body_masks,this->foreground_frame,this->face_body_masks);
		cvClearHist(this->face_hist);
		cvCalcHist(this->raw_frame_splitter,this->face_hist,0,this->face_body_masks);
		cvNormalizeHist(this->face_hist,255);
	}

	cvSplit(this->processed_frame,
			this->processed_frame_splitter[0],
			this->processed_frame_splitter[1],
			this->processed_frame_splitter[2],NULL);		
	cvCalcBackProject(this->processed_frame_splitter,this->face_estimation,this->face_hist);
	cvThreshold(this->face_estimation,this->face_estimation,params->conf_facelevel->intensity,255,CV_THRESH_BINARY );		
}

void subtraction::grabFrame()
{
	if(this->isFinished) return;
	else this->counter++;		
	
	this->timed_frame=this->agrabber->getFrame();
	if(!this->timed_frame)
	{
		this->isFinished=true;
		this->raw_frame=NULL;
		return;
	}
	this->raw_frame=(IplImage *)this->timed_frame->getImage();			
#ifdef SUBTRACTION_LOG
	printf("done grabbing \n");
#endif	
}

void subtraction::extraProcess()
{	
#ifdef SUBTRACTION_LOG
	printf("extra processing \n");
#endif
	if(this->isFinished) return;

#ifdef SUBTRACTION_LOG
	printf("skipping section \n");
#endif

	CvSize for_resize;

	do
	{
		this->grabFrame();
		if(this->isFinished) return;
	}while(this->counter < params->conf_extradsp->framedelay);
	
	if(!this->processed_frame)
	{
		if(params->conf_extradsp->block_size > 1)
		{
			for_resize=cvGetSize(this->raw_frame);
			for_resize.height/=3;
			for_resize.width/=3;
			this->processed_frame=cvCreateImage(for_resize,IPL_DEPTH_8U,3);
		}
		else this->processed_frame=cvCloneImage(this->raw_frame);
	}
	
#ifdef SUBTRACTION_LOG
	printf("skipping section \n");
#endif

	if(params->conf_extradsp->block_size > 1)
	{
		cvResize(this->raw_frame,this->processed_frame,CV_INTER_LINEAR);
	}
	else cvCopy(this->raw_frame,this->processed_frame);
	
#ifdef SUBTRACTION_LOG
	printf("de interlacing section \n");
#endif

	switch(params->conf_extradsp->deInterlace)
	{
		case 1 : deInterlace(this->processed_frame);
#ifdef SUBTRACTION_LOG
			printf("just de interlace \n"); 
#endif
			break;
		case 2 : cleverDeInterlace(this->processed_frame);
#ifdef SUBTRACTION_LOG
			printf("clever de interlace \n"); 
#endif
			break;		
		default : ;
#ifdef SUBTRACTION_LOG
			printf("do not interlace \n");
#endif
	}

#ifdef SUBTRACTION_LOG
	printf("segmentation section \n");
#endif
	
	if(params->conf_extradsp->segmentation_area>1)
	{
#ifdef SUBTRACTION_LOG
		printf("segment image \n");
#endif
		if(!this->filt)
		{
			this->filt = new fithisConnectedRegions(this->processed_frame);
			this->filt->setThreshold(params->conf_extradsp->segmentation_area);
			this->filt->setMinimumArea(params->conf_extradsp->segmentation_threshold);
		}
		else filt->setWorkingFrame(this->processed_frame);
		this->filt->fithisExecuteCommand(FITHIS_ULTRA_SEGMENT);
	}
#ifdef SUBTRACTION_LOG
	printf("done extra processing \n");
#endif	
}

DLLEXPORT confParameters *subtraction::getParams(){return this->params;}

DLLEXPORT bool subtraction::isReady(){return this->ready;}

DLLEXPORT subtraction::~subtraction()
{	
	vector<CvRect *>::iterator it;
	
#ifdef SUBTRACTION_LOG
	printf("deleting deshadow \n");
#endif	
	if(this->deshade) delete this->deshade;
#ifdef SUBTRACTION_LOG
	printf("deshadow deleted \n");
#endif	
#ifdef SUBTRACTION_LOG
	printf("deleting grabber \n");
#endif	
	delete this->agrabber;
#ifdef SUBTRACTION_LOG
	printf("grabber deleted \n");
#endif

#ifdef SUBTRACTION_LOG
	printf("deleting predictor \n");
#endif			
	delete this->apred;
#ifdef SUBTRACTION_LOG
	printf("predictor deleted \n");
#endif		

	
#ifdef SUBTRACTION_LOG
	printf("deleting images \n");
#endif		
	if(this->prediction_frame) cvReleaseImage(&this->prediction_frame);
	if(this->internal_foreground_frame) cvReleaseImage(&this->internal_foreground_frame);
	if(this->face_body_masks) cvReleaseImage(&this->face_body_masks);
	if(this->foreground_frame) cvReleaseImage(&this->foreground_frame);
	if(this->raw_frame) cvReleaseImage(&this->raw_frame);
	if(this->processed_frame) cvReleaseImage(&this->processed_frame);
	if(this->final_frame) cvReleaseImage(&this->final_frame);	
	if(this->timed_frame) delete this->timed_frame;
	if(this->region_level_frame) cvReleaseImage(&this->region_level_frame);
#ifdef SUBTRACTION_LOG
	printf("images deleted\n");
#endif	


	if(this->filt) delete filt;
	if(this->face_filt) delete this->face_filt;		
#ifdef SUBTRACTION_LOG
	printf("params, filter and cacsade deleted\n");
#endif		

	if(this->region_level) delete this->region_level;
	if(this->face_level) delete this->face_level;

#ifdef SUBTRACTION_LOG
	printf("closing objects.xml if available \n");
#endif		

#ifdef SUBTRACTION_LOG
	printf("deleting fcedetector \n");
#endif	
	while(!this->face_vector->empty()) this->face_vector->pop_back();
	while(!this->body_vector->empty()) this->body_vector->pop_back();
	delete this->face_vector;
	delete this->body_vector;
	if(this->facedet) delete facedet;
#ifdef SUBTRACTION_LOG
	printf("facedetector deleted \n");
#endif

	if(params->conf_facelevel)
	{
		delete[] this->face_hist_sizes;
		for(int l=0;l<3;l++) delete []this->face_hist_ranges[l];
		delete []this->face_hist_ranges;
		cvReleaseHist(&this->face_hist);	
	}

	if(this->face_estimation) cvReleaseImage(&this->face_estimation);	
	if(this->raw_frame_splitter)
	{
		for(int l=0;l<3;l++) cvReleaseImage(&this->raw_frame_splitter[l]);
		delete this->raw_frame_splitter;
	}
	if(this->processed_frame_splitter)
	{
		for(int l=0;l<3;l++) cvReleaseImage(&this->processed_frame_splitter[l]);
		delete this->processed_frame_splitter;
	}


#ifdef SUBTRACTION_LOG
	printf("deleting params, filters and cascade \n");
#endif		
	delete this->params;
}

