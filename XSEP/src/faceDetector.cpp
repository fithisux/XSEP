#include "faceDetector.h"

face_detect_ctx::face_detect_ctx(separ_facedetection_conf_t *afaceconf,bool pyramdo)
{
	this->faceconf=afaceconf;
	this->face_shape=cvSize(afaceconf->facewidth,afaceconf->faceheight);
	this->scale=1;
	this->do_pyramids=pyramdo;
	this->cascade=(CvHaarClassifierCascade*)cvLoad(afaceconf->cascade);
	this->temp_storage=NULL;
	this->small_image=NULL;
	this->body_vector= new vector<CvRect *>(0);
	this->face_vector= new vector<CvRect *>(0);
}

face_detect_ctx::~face_detect_ctx()
{
	if(this->temp_storage) cvReleaseImage(&this->temp_storage);
	if(this->small_image) cvReleaseImage(&this->small_image);

	vector<CvRect *>::iterator it;
	
	if(this->face_vector->size() > 0)
	{
		for(it=this->face_vector->begin();it!=this->face_vector->end();) delete *it++;
		this->face_vector->erase(this->face_vector->begin(),this->face_vector->end());		
	}
	
	if(this->body_vector->size() > 0)
	{
		for(it=this->body_vector->begin();it!=this->body_vector->end();) delete *it++;
		this->body_vector->erase(this->body_vector->begin(),this->body_vector->end());		
	}
	delete this->face_vector;
	delete this->body_vector;
	cvReleaseHaarClassifierCascade( &this->cascade );
	this->faceconf=NULL;
}

void face_detect_ctx::updateWithImageAndRect(IplImage *some_image,CvRect * place)
{
	this->size=0;
	if( (place->height < this->face_shape.height) || 
		(place->width < this->face_shape.width) ) return;
	if(!this->temp_storage) this->temp_storage=cvCloneImage(some_image);
	cvCvtColor(some_image,this->temp_storage,CV_BGR2HSV);
	if(!this->small_image)
	{
		 if( this->do_pyramids )			
		 {
			this->small_image = cvCreateImage( cvSize(some_image->width/2,some_image->height/2), IPL_DEPTH_8U, 3 );			
			scale = 2;
		 }
		 else this->small_image= cvCloneImage(some_image);
	}
	else
	{
         if( this->do_pyramids )			
		 {
			cvPyrDown(some_image, small_image, CV_GAUSSIAN_5x5 );
			scale = 2;
		 }
		 else cvCopy(some_image,this->small_image);
	}

	cvSetImageROI(small_image,*place);
	this->detect_and_mark_humans(place);
	cvResetImageROI(small_image);
}

void face_detect_ctx::detect_and_mark_humans(CvRect *place)
{
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* faces=cvHaarDetectObjects(this->small_image,this->cascade,
		storage,1.1,3,CV_HAAR_DO_CANNY_PRUNING,this->face_shape);
	CvRect face_rect, body_rect;
	CvRect *candidate_rect;
	vector<CvRect *>::iterator it1 = this->face_vector->begin();
	vector<CvRect *>::iterator it2 = this->body_vector->begin();
	int help_width;
	int help_x;

	for(int i = 0; i < faces->total; i++ )
	{
		candidate_rect = (CvRect *) cvGetSeqElem( faces, i);
		candidate_rect->x+=place->x;
		candidate_rect->y+=place->y;
		printf("Found a face \n");
		if(this->reduceFalsePositives(candidate_rect))
		{
			printf("Which is human face \n");
			help_width = (candidate_rect->width / 7);
			face_rect.width = candidate_rect->width- 2 * help_width;
			face_rect.height=candidate_rect->height;
			face_rect.x= candidate_rect->x+help_width;		
			face_rect.y= candidate_rect->y;

			if(it1==this->face_vector->end())
			{
				it1 = this->face_vector->insert(it1,new CvRect);
			}
			memcpy(*it1++,&face_rect,sizeof(CvRect));

			body_rect.y = face_rect.y+face_rect.height;
			body_rect.height = place->y + place->height - body_rect.y;					
			help_x = face_rect.x-face_rect.width;
			body_rect.x= ( ( help_x < place->x ) ? place->x : help_x);
			help_x = face_rect.x+2*face_rect.width;
			body_rect.width = ( ( help_x > place->x+ place->width ) ? place->x+ place->width : help_x);
			body_rect.width-=body_rect.x;

			if(it2==this->body_vector->end())
			{
				it2 = this->body_vector->insert(it2,new CvRect);
			}
			memcpy(*it2++,&body_rect,sizeof(CvRect));
			this->size++;
		}
	}
	cvReleaseMemStorage( &storage );
}

bool face_detect_ctx::reduceFalsePositives(CvRect *some_rect)
{
	int counter=0;
	unsigned char val;
	
	unsigned char *line_runner=(unsigned char *) this->temp_storage->imageData;
	unsigned char *runner;

	int rect_right_x=some_rect->x + some_rect->width;
	int rect_left_x=some_rect->x;
	int rect_low_y=some_rect->y+some_rect->height;
	int rect_up_y=some_rect->y;

	double face_thresh = some_rect->height * some_rect->width;
	face_thresh *= this->faceconf->huepercentage;
	face_thresh /= 100;

	printf("Checking %u %u \n",this->faceconf->uphue,this->faceconf->downhue);
	
	for(int i=0;i<this->temp_storage->height;i++)
	{
		runner=line_runner;
		for(int j=0;j<this->temp_storage->width;j++)
		{
			val = *runner;
			runner+=3;
			if( (j>=rect_left_x) && 
				(j<=rect_right_x) &&
				(i>=rect_up_y) && 
				(i<=rect_low_y) &&
				(val < this->faceconf->uphue) &&
				(val > this->faceconf->downhue)) counter++;			
		}
		line_runner+=this->temp_storage->widthStep;
	}

	printf("Counter %d \n",counter);
	if(counter >= face_thresh) return true; else return false;
}
