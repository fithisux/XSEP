#include "fithisConnectedRegions.h"
#include  <stdio.h>
#include "highgui.h"

fithisConnectedRegions::fithisConnectedRegions(IplImage *start_image)
{
    this->templates=NULL;
    this->threshold=0;
    this->underflow=false;
    this->mask=this->current_frame=NULL;
    this->image_buff = new unsigned char *[1];
	this->mask_buff = new int *[1];
	this->minArea=1;
	this->ResetManagers();
    this->setWorkingFrame(start_image);
    this->setConnectivity(FITHIS_4_CROSS);
	this->label=1;
	this->mask_selector=NULL;
	this->isSucc=false;
	this->allocs=0;
	this->working_route=this->cached_route=NULL;
	this->working_seg_mosaic=this->cached_seg_mosaic=NULL;	
}

fithisConnectedRegions::~fithisConnectedRegions()
{
	FithisMemElem *runner;

    if(this->templates) delete[] templates;
	if(this->mask) cvReleaseImage(&this->mask);
	
	while(this->working_route)
	{
		runner=this->working_route->next;
		delete this->working_route;
		this->working_route=runner;
	}

	while(this->cached_route)
	{
		runner=this->cached_route->next;
		delete this->cached_route;
		this->cached_route=runner;
	}

	this->cleanUpMosaic();
	if(this->image_buff) delete[] this->image_buff;
	if(this->mask_buff) delete[] this->mask_buff;
	this->mask_buff=NULL;
	this->image_buff=NULL;
}

void fithisConnectedRegions::fithisExecuteCommand(fithisConnectedCommand cmd)
{
	this->the_cmd=cmd;
	int * indicator = (int *) this->mask->imageData;	
	unsigned char *selector_runner=NULL;
	CvPoint pos;
	bool jumpit;
	this->label=1;
	this->underflow=false;
	int skip_line=0;
	cvZero(this->mask);
	
	if(this->mask_selector)
	{
		 skip_line=this->mask_selector->widthStep - this->mask_selector->width;
	}
	
	unsigned char *ff2=NULL;
	int skip_line_raster=0;
	
	
	if(this->the_cmd==FITHIS_FILTER)
	{
			ff2=(unsigned char *) this->current_frame->imageData;
			skip_line_raster=this->current_frame->widthStep-this->current_frame->width;
	}	
	
	for(int l=0;l<4;l++) temp_data[l]=0;
	
	if(this->mask_selector)
	{
		selector_runner=(unsigned char *) this->mask_selector->imageData;
	}
	
	for(int i=0;i<this->current_frame->height;i++)
	{
		for(int j=0;j<this->current_frame->width;j++)
		{		
			pos.x = j;pos.y = i;
			jumpit=false;
			if((this->the_cmd==FITHIS_FILTER) && !(*ff2++)) jumpit=true;
			if(*indicator++) jumpit=true;
			if(this->mask_selector)
			{
				if(!(*selector_runner++)) jumpit=true;
			}
			
			if(jumpit)continue;
			switch(cmd)
			{														
				case FITHIS_QUICK_SEGMENT :
					this->fithisGrowSegment(&pos);
					if(!this->underflow) this->label++;
					break;
				case FITHIS_FILTER :					
					this->fithisFilterConnectedRegion(&pos);
					this->label++;
					break;				
				case FITHIS_ULTRA_SEGMENT :					
					this->fithisDust(&pos);
					this->label++;
					break;					
				}
		}
		if(this->mask_selector) selector_runner+=skip_line;
		if(this->the_cmd==FITHIS_FILTER) ff2+=skip_line_raster;
	}
	this->label--;	
	if(this->mask_selector)
	{
		indicator=(int *) this->mask->imageData;
		ff2=(unsigned char *) this->current_frame->imageData;
		skip_line_raster=this->current_frame->widthStep-this->current_frame->nChannels*this->current_frame->width;
		for(int i=0;i<this->current_frame->height;i++)
		{
			for(int j=0;j<this->current_frame->width;j++)
			{		
				if(!(*indicator++))
				{
					for(int l=0;l<this->current_frame->nChannels;l++) ff2[l]=0;
				}
				ff2+=this->current_frame->nChannels;
			}
			ff2+=skip_line_raster;
		}		
	}
	
#ifdef FITHIS_CONNECTED_REGIONS_DEBUG	
	FithisMemElem *runner=this->cached_route;
	int recycled=0;		
	while(runner)
	{
		runner=runner->next;
		recycled++;
	}
	printf("...........................Report : Reserved %d Recycled %d \n",this->allocs,recycled);
#endif
}

void fithisConnectedRegions::fithisGrowSegment(CvPoint *pos)
{	
	int absorb_indy=0;
    bool absorbed=false;     	
    FithisMemElem* candidate = this->newElement(pos);
    FithisMemElem* center=candidate;
    this->getIt(candidate->image_ptr);
    *candidate->mask_ptr=this->label;
    this->fithisUseMemElem(candidate);
    
   	this->ResetManagers();	
	this->manageElement(candidate);	

	while(this->working_route)
	{	
		if(candidate=this->getNextPoint())
		{    
			if(*candidate->mask_ptr)
			{
				if( !absorbed && (*candidate->mask_ptr != this->label))
				{
					absorb_indy=*candidate->mask_ptr;
					for(int l=0;l<this->current_frame->nChannels;l++)
					{
						 this->temp_data[l]=candidate->image_ptr[l];
					}
					absorbed=true;
				}
				this->fithisRecycleMemElem(candidate);			
				continue;
			}
			
			if(this->isSucc) center=this->working_route;        
			if(!this->isRelative(candidate->image_ptr,center->image_ptr))
			{						
				this->fithisRecycleMemElem(candidate);
				continue;			
			}

    		this->setIt(candidate->image_ptr);
    		*candidate->mask_ptr=this->label;
    		this->fithisUseMemElem(candidate);
			this->manageElement(candidate);			
		}
	}
	
	if(absorbed)
	{
		 this->fillWhenFull(temp_data,&absorb_indy);
		 this->underflow=false;
	}
	else
	{
		this->fillWhenFull(NULL,&absorb_indy);
	}			
}


void fithisConnectedRegions::fithisDust(CvPoint *pos)
{		
	FithisMemElem* candidate = this->newElement(pos);    
    this->getIt(candidate->image_ptr);
    *candidate->mask_ptr=this->label;
    this->fithisUseMemElem(candidate);
	this->area=1;
	while(this->working_route)
	{	
		if(candidate=this->getNextPoint())
		{   
			if(*candidate->mask_ptr ||
				!this->isRelative(candidate->image_ptr,this->working_route->image_ptr))
			{				
				this->fithisRecycleMemElem(candidate);
				continue;					
			}
			this->area++;
			//printf("%u %u %u %u \n",pixel_data[0],pixel_data[1],pixel_data[2],pixel_data[3]);
			this->setIt(candidate->image_ptr);	
    		*candidate->mask_ptr=this->label;
    		this->fithisUseMemElem(candidate);			
		}
	}
}


void fithisConnectedRegions::fithisFilterConnectedRegion(CvPoint *pos)
{	
	if( (this->current_frame->depth != IPL_DEPTH_8U) || (this->current_frame->nChannels != 1) )
	{
		printf("Not a raster.Aborting... \n");
		exit(0);
	}
	
    FithisMemElem* candidate = this->newElement(pos);    
    
    this->ResetManagers();
	this->manageElement(candidate);			
    *candidate->mask_ptr=this->label;
    this->fithisUseMemElem(candidate);

	while(this->working_route)
	{	
		if(candidate=this->getNextPoint())
		{            			
			if(*candidate->mask_ptr || (*candidate->image_ptr != *this->working_route->image_ptr))
			{
				this->fithisRecycleMemElem(candidate);
				continue;
			}

			this->manageElement(candidate);	
    		*candidate->mask_ptr=this->label;
    		this->fithisUseMemElem(candidate);			
		}
	}
		
	this->fillWhenFull(temp_data,NULL);		
}

bool fithisConnectedRegions::isRelative(unsigned char *a,unsigned char *b)
{	
	//return true;
	unsigned char cost;
	for(int l=0;l<this->current_frame->nChannels;l++)
	{
		cost= (( a[l] > b[l] ) ? a[l]-b[l] : b[l]-a[l]);
		if(cost > this->threshold) return false;
	}
	return true;
}


void fithisConnectedRegions::setWorkingFrame(IplImage *image)
{
	CvSize asz;
	if(!image)
	{
		printf("NULL image ? Aborting .... \n");
		exit(0);
	}

	if(image->depth != IPL_DEPTH_8U)
	{
		printf("Only 1 byte/channel  images allowed. Aborting .... \n");
		exit(0);		
	}	
	
	this->current_frame=image;	
	
	asz=cvGetSize(this->current_frame);
	if(!this->mask) this->mask = cvCreateImage(asz,IPL_DEPTH_32S,1);
	else
	{		
		if((asz.width!=this->mask->width)||(asz.height != this->mask->height))
		{
			cvReleaseImage(&this->mask);			
			this->mask = cvCreateImage(asz,IPL_DEPTH_32S,1);
		}
	}
}

void fithisConnectedRegions::ResetManagers()
{		
	this->area=0;				
}

void fithisConnectedRegions::setConnectivity(FithisConnectivity conn)
{    
    if(this->templates) delete[] this->templates;
    this->connectivity=conn;
    
    if(this->connectivity == FITHIS_8) this->templates_size=8;
	else this->templates_size=4;
    this->templates = new CvPoint[this->templates_size];
    
    switch(this->connectivity)
	{
		case FITHIS_4_CROSS :

                this->templates[0]= cvPoint(0,-1);
                this->templates[1]= cvPoint(1,0);
                this->templates[2]= cvPoint(0, 1);
                this->templates[3]= cvPoint(-1,0);
                break;
		
		case FITHIS_4_X :

                this->templates[0]= cvPoint(-1,-1);
                this->templates[1]= cvPoint(1,-1);
                this->templates[2]= cvPoint(1,1);
                this->templates[3]= cvPoint(-1,1);
                break;
		
		case FITHIS_8 :

                this->templates[1]= cvPoint(0,-1);
                this->templates[3]= cvPoint(1,0);
                this->templates[5]= cvPoint(0,1);
                this->templates[7]= cvPoint(-1,0);

                this->templates[0]= cvPoint(-1,-1);
                this->templates[2]= cvPoint(1,-1);
                this->templates[4]= cvPoint(1,1);
                this->templates[6]= cvPoint(-1,1);
                break;
	}
}

void fithisConnectedRegions::cleanUpMosaic()
{
	FithisMosaic * seg_runner;

	this->putBackMosaics();
	while(this->cached_seg_mosaic)
	{
		seg_runner=this->cached_seg_mosaic;
		this->cached_seg_mosaic=this->cached_seg_mosaic->next;
		delete seg_runner;
	}
}

void fithisConnectedRegions::giveMosaics(int num)
{
	FithisMosaic * seg_runner;
	
	while(num--)
	{
		if(this->cached_seg_mosaic)
		{
			seg_runner=this->cached_seg_mosaic;
			this->cached_seg_mosaic=this->cached_seg_mosaic->next;			
		}
		else seg_runner=new FithisMosaic;
		seg_runner->next=this->working_seg_mosaic;
		this->working_seg_mosaic=seg_runner;
	}
}

void fithisConnectedRegions::putBackMosaics()
{
	FithisMosaic * seg_runner;

	while(this->working_seg_mosaic)
	{
		seg_runner=this->working_seg_mosaic;
		this->working_seg_mosaic=this->working_seg_mosaic->next;
		seg_runner->next=this->cached_seg_mosaic;
		this->cached_seg_mosaic=seg_runner;
	}
}

FithisMosaic * fithisConnectedRegions::getSegMosaic()
{
	this->putBackMosaics();
	FithisMosaic ** my_mosaic = new FithisMosaic * [this->label];
	FithisMosaic* ptr;
	int * mask_runner= (int *) this->mask->imageData;
	unsigned char * image_runner = (unsigned char *)  this->current_frame->imageData;
	int skip_line = this->current_frame->widthStep - this->current_frame->nChannels*this->current_frame->width;
	int tabela;

	this->giveMosaics(this->label);
	ptr= this->working_seg_mosaic;

	for(int k=0;k<this->label;k++)
	{
		my_mosaic[k]=ptr;
		ptr->foreground=1;
		ptr=ptr->next;
	}

	for(int i=0;i<this->mask->height;i++)
	{
		for(int j=0;j<this->mask->width;j++)
		{
			tabela = *mask_runner++;
			if(tabela == 0) continue;			
			ptr=my_mosaic[--tabela];
			if(ptr->foreground)
			{
				my_mosaic[tabela]=ptr;				
				ptr->seg_Id=tabela;
				ptr->seg_desc.x=ptr->seg_desc.width=j;
				ptr->seg_desc.y=ptr->seg_desc.height=i;
				ptr->foreground=0;
				getValue(this->current_frame,image_runner,&ptr->seg_colour);
			}
			else
			{
				if(ptr->seg_desc.x > j) ptr->seg_desc.x=j;
				else
				{
					if(ptr->seg_desc.width < j) ptr->seg_desc.width=j;
				}
				
				if(ptr->seg_desc.y > i) ptr->seg_desc.y=i;
				else
				{
					if(ptr->seg_desc.height < i) ptr->seg_desc.height=i;
				}
			}
			image_runner+=this->current_frame->nChannels;
		}
		image_runner+=skip_line;
	}

	for(int k=0;k<this->label;k++)
	{
		ptr=my_mosaic[k];
		ptr->seg_desc.width -=ptr->seg_desc.x;
		ptr->seg_desc.width++;
		ptr->seg_desc.height -=ptr->seg_desc.y;
		ptr->seg_desc.height++;
		ptr->gravity_centre.x = ptr->seg_desc.x + ptr->seg_desc.width /2;
		ptr->gravity_centre.y = ptr->seg_desc.y + ptr->seg_desc.height /2;
		ptr=ptr->next;
	}

	delete my_mosaic;
	return this->working_seg_mosaic;
}

void fithisConnectedRegions::setMinimumArea(int sz)
{	
	if(sz < 1)
	{
		printf("Minimum area for component must be a positive number. Aborting ...\n");
		exit(0);
	}
	
	if(this->minArea!=sz)
	{
			this->minArea=sz;
			if(this->image_buff) delete [] this->image_buff;
			if(this->mask_buff) delete [] this->mask_buff;
			this->image_buff = new unsigned char *[this->minArea];
			this->mask_buff = new int *[this->minArea];	
	}
	this->ResetManagers();
}

void fithisConnectedRegions::setIndicator(IplImage *an_indicator)
{

	if(an_indicator ==NULL)
	{
		this->mask_selector=NULL;
		return;
	} 
	if( 
		(an_indicator->width==this->current_frame->width) &&
		(an_indicator->height==this->current_frame->height) &&
		(an_indicator->depth==IPL_DEPTH_8U)
	  ) this->mask_selector=an_indicator;
	else
	{
		printf("Incompatible indicator. Aborting... \n");
		exit(0);
	}
}

void fithisConnectedRegions::fillWhenFull(unsigned char*img_val,int* mask_val)
{
	if(this->area<this->minArea)
	{	
		this->underflow=true;
		//printf("underflow for area %d with minarea %d\n",this->area,this->minArea);
		for(int i=0;i<this->area;i++)
		{
			if(img_val) 
			{
				for(int l=0;l<this->current_frame->nChannels;l++) this->image_buff[i][l]=img_val[l];
			}
			if(mask_val) *this->mask_buff[i]=*mask_val;
		}
		//printf("underflow taken into account\n");
	}
	else 	this->underflow=false;
	
}

void fithisConnectedRegions::manageElement(FithisMemElem *m)
{
	if(this->area < this->minArea)
	{
		this->image_buff[this->area]=m->image_ptr;
		this->mask_buff[this->area]=m->mask_ptr;
	}
	this->area++;
}

bool fithisConnectedRegions::isCorrectIndex(CvPoint *pos)
{
	if( (pos->x < 0) || (pos->x >=this->current_frame->width) ) return false;
	if( (pos->y <0) || (pos->y >=this->current_frame->height) ) return false;
	return true;
}

FithisMemElem *fithisConnectedRegions::newElement(CvPoint *pt)
{
	int *ff1=(int *) this->mask->imageData;	
	ff1+=(pt->y*this->mask->width+pt->x);
	
	if(*ff1) return NULL;
	FithisMemElem *m;
	if(this->cached_route != NULL)
	{
		m=this->cached_route;
		this->cached_route=this->cached_route->next;
	}
	else
	{
		 m=new FithisMemElem;
#ifdef FITHIS_CONNECTED_REGIONS_DEBUG		 
		 this->allocs++;
#endif		 
	}
	
	m->next=NULL;
	m->orient=0;
	m->pt.x=pt->x;
	m->pt.y=pt->y;
	m->mask_ptr= (int *) this->mask->imageData;
	m->mask_ptr =ff1;
	m->image_ptr=(unsigned char *) this->current_frame->imageData;
	m->image_ptr +=(pt->x*this->current_frame->nChannels + pt->y*this->current_frame->widthStep);
	return m;		
}

void fithisConnectedRegions::fithisUseMemElem(FithisMemElem *m)
{
	m->next=this->working_route;
	this->working_route=m;
}

void fithisConnectedRegions::fithisRecycleMemElem(FithisMemElem *m)
{
	m->next=this->cached_route;
	this->cached_route=m;	
}

FithisMemElem *fithisConnectedRegions::getNextPoint()
{
	FithisMemElem *m;

	if(!this->working_route) return NULL;
	if(this->working_route->orient==this->templates_size)
	{
		m=this->working_route->next;
		this->working_route->next=this->cached_route;		
		this->cached_route=this->working_route;
		this->working_route=m;	
		return NULL;
	}
	
	CvPoint pt;
	pt.x = this->working_route->pt.x + this->templates[this->working_route->orient].x;
	pt.y = this->working_route->pt.y + this->templates[this->working_route->orient].y;
	this->working_route->orient+=1;
	if(this->isCorrectIndex(&pt)) return this->newElement(&pt);
	else return NULL;
}
