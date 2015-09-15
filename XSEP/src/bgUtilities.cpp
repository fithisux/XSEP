#include "bgUtilities.h"
#include <stdio.h>


DllExport IplImage * polymnia_crop_image(IplImage *src,CvRect *place)
{	
	IplImage *dst = cvCreateImage(cvSize(place->width,place->height),IPL_DEPTH_8U,3);
	int data_unit = place->width* 3;
	int data_offset = place->x * 3;

	unsigned char* src_ptr = (unsigned char *) src->imageData;
	src_ptr += (place->y * src->widthStep);
	src_ptr += (place->x * 3);

	unsigned char *dst_ptr = (unsigned char *) dst->imageData;

	for(int l=0;l<place->height;l++)
	{
		memcpy(dst_ptr,src_ptr,data_unit);
		src_ptr += src->widthStep;
		dst_ptr += dst->widthStep;
	}
	return dst;
}

DllExport void polymnia_copy_crop_image(IplImage *src,CvRect *place,IplImage *dst)
{
	int data_unit = place->width* 3;
	int data_offset = place->x * 3;

	unsigned char* src_ptr = (unsigned char *) src->imageData;
	src_ptr += (place->y * src->widthStep);
	src_ptr += (place->x * 3);

	unsigned char *dst_ptr = (unsigned char *) dst->imageData;

	for(int l=0;l<place->height;l++)
	{
		memcpy(dst_ptr,src_ptr,data_unit);
		src_ptr += src->widthStep;
		dst_ptr += dst->widthStep;
	}
}

 inline int integral_floor(int a,int b)
 {
 	int x= a /b;
 	if(a % b) return ++x;
 	else return x;
 }
 
unsigned char abs_diff_8u(unsigned char x,unsigned char y)
{
	return ( (x > y) ? (x-y) : (y-x) );
}

unsigned char cyclic_abs_diff_8u(unsigned char x,unsigned char y)
{
	unsigned char z= abs_diff_8u(x,y);
	unsigned char w=180-z;
	return min(z,w);
}

inline unsigned char u8_rgb_diff(unsigned char *a,unsigned char *b,int n)
{
	unsigned char d,cost=0;
	d= abs_diff_8u(*a++,*b++);
	if(d > cost) cost=d;
	d= abs_diff_8u(*a++,*b++);
	if(d > cost) cost=d;
	d= abs_diff_8u(*a++,*b++);
	if(d > cost) cost=d;
	return cost;
}


DllExport void image_u8_rgb_diff(IplImage *img1,IplImage *img2,IplImage *img3,unsigned char thresh)
{
		unsigned char *ptr1,*ptr2,*ptr3;
		unsigned char *runner1,*runner2,*runner3;

		ptr1=(unsigned char *)img1->imageData;
		ptr2=(unsigned char *)img2->imageData;
		ptr3=(unsigned char *)img3->imageData;

		for(int i=0;i<img1->height;i++)
		{
			runner1=ptr1;
			runner2=ptr2;
			runner3=ptr3;
			for(int j=0;j<img2->width;j++)
			{
				*runner3++ = ( (u8_rgb_diff(runner1,runner2,3) > thresh) ? 255 : 0);
				runner1+=3;
				runner2+=3;
			}
			ptr1+=img1->widthStep;
			ptr2+=img2->widthStep;
			ptr3+=img3->widthStep;
		}
}

void image_u8_rgb_add(IplImage *img1,IplImage *img2)
{
		unsigned char *ptr1,*ptr2;
		unsigned char *runner1,*runner2;

		ptr1=(unsigned char *)img1->imageData;
		ptr2=(unsigned char *)img2->imageData;
		
		for(int i=0;i<img1->height;i++)
		{
			runner1=ptr1;
			runner2=ptr2;
			for(int j=0;j<img2->width;j++)
			{
				(*runner1++) += (*runner2++);
				runner1+=3;
				runner2+=3;
			}
			runner1+=img1->widthStep;
			runner2+=img2->widthStep;
		}
}




	void block2smaller_32F(IplImage *in,IplImage *out,int block_size)
	{
		if( (in->depth != IPL_DEPTH_32F) || (out->depth != IPL_DEPTH_32F) )
  		{
  			printf("block2smaller_32F ILLEGAL DEPTH\n");
  			exit(0);
  		}
  		
  		if( (in->width / block_size != out->width) ||
  			(in->height / block_size != out->height) ||
  			(in->nChannels != out->nChannels) )
  		{
  			printf("Not compatible\n");
  			exit(0);  			
  		}
  		
		cvZero(out);

  		float * ptr_in= (float *) in->imageData;
  		float * ptr_out= (float *) out->imageData;
  		float *runner_in,*runner_out;
  		  		
  		int area= block_size * block_size;
		int skip_in_row= in->width * in->nChannels;
		int skip_out_row= out->width * out->nChannels;
		  		
  		for(int i=0;i<out->height;i++)
  		{  	
			for(int k=0;k<block_size;k++)
			{		
				runner_in=ptr_in;  		
				runner_out=ptr_out;
  				for(int j=0;j<out->width;j++)
				{				
					for(int l=0;l<block_size;l++)
					{
						for(int m=0;m<out->nChannels;m++) runner_out[m]+= *runner_in++;
					}
					runner_out += out->nChannels;
				}
				ptr_in+=skip_in_row;
			}
			runner_out=ptr_out;
			for(int n=0;n<skip_out_row;n++) *runner_out++ /= area;
			ptr_out+=skip_out_row;
		}
	}
	
	void deBlock2bigger_8U(IplImage *in,IplImage *out,int block_size)
	{
		if( (in->depth != IPL_DEPTH_8U) || (out->depth != IPL_DEPTH_8U) )
  		{
  			printf("deBlock2bigger_8U ILLEGAL DEPTH\n");
  			exit(0);
  		}
  		
  		if( (out->width / block_size != in->width) ||
  			(out->height / block_size != in->height) ||
  			(in->nChannels != out->nChannels) || 
  			(in->nChannels != 1))
  		{
  			printf("Not compatible\n");
  			exit(0);  			
  		}
  		
  		unsigned char * ptr_in= (unsigned char *) in->imageData;
  		unsigned char * ptr_out= (unsigned char *) out->imageData;
  		unsigned char *runner_in,*runner_out;
  		  		
  		int skip_in_row= in->widthStep;
		int skip_out_row= out->widthStep;
		  		
  		for(int i=0;i<in->height;i++)
  		{  	
			for(int k=0;k<block_size;k++)
			{		
				runner_in=ptr_in;  		
				runner_out=ptr_out;
  				for(int j=0;j<in->width;j++)
				{				
					for(int l=0;l<block_size;l++)
					{
						for(int m=0;m<out->nChannels;m++) *runner_out++ = runner_in[m];
					}
					runner_in += in->nChannels;
				}
				ptr_out+=skip_out_row;
			}
			ptr_in+=skip_in_row;
		}
	}
	
	void deBlock2bigger_32F(IplImage *in,IplImage *out,int block_size)
	{
		if( (in->depth != IPL_DEPTH_32F) || (out->depth != IPL_DEPTH_32F) )
  		{
  			printf("deBlock2bigger_32F ILLEGAL DEPTH \n");
  			exit(0);
  		}
  		
  		if( (out->width / block_size != in->width) ||
  			(out->height / block_size != in->height) ||
  			(in->nChannels != out->nChannels))
  		{
  			printf("Not compatible\n");
  			exit(0);  			
  		}
  		
  		float * ptr_in= (float *) in->imageData;
  		float * ptr_out= (float *) out->imageData;
  		float *runner_in,*runner_out;
  		  		
  		int skip_in_row= in->width * in->nChannels;
		int skip_out_row= out->width * out->nChannels;
		  		
  		for(int i=0;i<in->height;i++)
  		{  	
			for(int k=0;k<block_size;k++)
			{		
				runner_in=ptr_in;  		
				runner_out=ptr_out;
  				for(int j=0;j<in->width;j++)
				{				
					for(int l=0;l<block_size;l++)
					{
						for(int m=0;m<out->nChannels;m++) *runner_out++ = runner_in[m];
					}
					runner_in += in->nChannels;
				}
				ptr_out+=skip_out_row;
			}
			ptr_in+=skip_in_row;
		}
	}
	
 void createCircle(IplImage *frame,CvPoint *center_ptr,double radius)
{						
		CvPoint pp;
		float dist;
		int l;
		unsigned char * runner=(unsigned char *) frame->imageDataOrigin;

		cvSet(frame,cvScalar(255,255,255,255));
		for(pp.x=0,pp.y=0,l=0;l<(frame->imageSize/3);l++)
		{				
				dist = sqrtf(	(1.0*pp.x - 1.0*center_ptr->x)*
								(1.0*pp.x - 1.0*center_ptr->x)+ 
								(1.0*pp.y - 1.0*center_ptr->y)*
								(1.0*pp.y - 1.0*center_ptr->y));
				if(dist <= radius)
				{
					(*runner)=0;
					(*(runner+1))=255;
					(*(runner+2))=0;				
				}
				runner+=3;		
				pp.x = l % frame->width;
				pp.y = l / frame->width;
		}
		
}

void deInterlace(IplImage *frame)
{
	unsigned char * runner = (unsigned char *)frame->imageData;
	int eff_height = (frame->height >> 1) << 1;

	for(int l=1;l<eff_height;l+=2)
	{
		runner+=frame->widthStep;
		memcpy(runner,runner-frame->widthStep,frame->widthStep);
	}
}

 void cleverDeInterlace(IplImage *frame)
{
	unsigned char * runner = (unsigned char *) frame->imageData;
	int P,Q,theta;
	unsigned char *  C = new unsigned char[3];
	int eff_height = ((frame->height-1) >> 1) << 1;
	int eff_width = ((frame->width-1) >> 1) << 1;
	int line_skip = frame->widthStep- 3 *  eff_width ;
	int temp;


	for(int i=1;i<eff_height;i+=2)
	{		
		runner += frame->widthStep;
		
		for(int j=1;j<eff_width;j+=2)
		{	
			runner+=3;
			P = u8_rgb_diff(runner-frame->widthStep-3,runner+frame->widthStep,3);
			P+= u8_rgb_diff(runner-frame->widthStep,runner+frame->widthStep+3,3);

			Q = u8_rgb_diff(runner-frame->widthStep,runner+frame->widthStep-3,3);
			Q+= u8_rgb_diff(runner-frame->widthStep+3,runner+frame->widthStep,3);

			C[0]= u8_rgb_diff(runner-frame->widthStep-3,runner+frame->widthStep+3,3);			
			C[1]= u8_rgb_diff(runner-frame->widthStep,runner+frame->widthStep,3);				
			C[2]= u8_rgb_diff(runner-frame->widthStep+3,runner+frame->widthStep-3,3);
			
			
			if( P > Q) theta = ( ( C[0] < C[1] ) ? -1 : 0);
			else if (Q < P) theta = ( ( C[1] < C[2] ) ? 0 : 1);
			else
			{
				theta = ( ( C[0] < C[1] ) ? -1 : 0);
				if( C[2] < C[theta+1] ) theta=1;
			}			

			for(int k=0;k<3;k++)
			{
				temp =(*(runner-frame->widthStep+3*theta));
				temp+=(*(runner+frame->widthStep-3*theta));
				(*runner) = temp / 2;			
				runner++;
			}
		}
		runner+=line_skip;
	}

	delete C;
}
  
double norm_infty(CvScalar *a,CvScalar *b)
{
	double cost=0;
	double d;

	for(int l=0;l<4;l++)
	{
		//cost+=fabs(a->val[l] - b->val[l]);
		d=fabs(a->val[l] - b->val[l]);
		if(d > cost) cost=d;
	}
	return cost;
}

double tri_diff(CvScalar *a,CvScalar *b,tri_info * diff_info)
{
	if(diff_info->use_hsv) return hsv_tri_diff(a,b,diff_info);
	else return rgb_tri_diff(a,b,diff_info);	
}

double rgb_tri_diff(CvScalar *a,CvScalar *b,tri_info * diff_info)
{
	double cost=0;
	double z=0;

	for(int l=0;l<4;l++)
	{
		z = fabs( (a->val[l]-b->val[l]) * diff_info->flags.val[l]);
		if(z > cost) cost=z;
	}

	return cost;
}

double hsv_tri_diff(CvScalar *a,CvScalar *b,tri_info * diff_info)
{
	double z1=fabs(a->val[0]-b->val[0]);
	double z2= fabs(180-z1);
	double z;
	double cost = 255 *diff_info->flags.val[0]* ( MIN(z1,z2) ) / 90;
	
	for(int l=1;l<4;l++)
	{
		z = fabs( (a->val[l]-b->val[l]) * diff_info->flags.val[l]);
		if(z > cost) cost=z;
	}
	return cost;	
}

 void getValue(IplImage *image,unsigned char *a,CvScalar *val)
{	
	for(int i=0;i<image->nChannels;i++)
	{
		switch(image->depth)
		{
			case IPL_DEPTH_8U :
				val->val[i]= (unsigned char) *a++;
				break;
			case IPL_DEPTH_8S :
				val->val[i]= (char) *a++;;
				break;
			case IPL_DEPTH_32S :
				val->val[i]= *((int *) a);
				a+=4;
				break;
			case IPL_DEPTH_32F :
				val->val[i]= *((float *) a);
				a+=4;
				break;
			case IPL_DEPTH_64F :
				val->val[i]= *((double *) a);
				a+=8;
				break;
		}
	}
	for(int i=image->nChannels;i<4;i++) val->val[i]=0;
}

 void setValue(IplImage *image,unsigned char *a,CvScalar *val)
{	
	for(int i=0;i<image->nChannels;i++)
	{
		switch(image->depth)
		{
			case IPL_DEPTH_8U :
				*a++ = (unsigned char) val->val[i];
				break;
			case IPL_DEPTH_8S :
				*((char *) a) = (char) val->val[i] ;
				a++;
				break;
			case IPL_DEPTH_32S :
				*((int *) a) = (int) val->val[i];
				a+=4;
				break;
			case IPL_DEPTH_32F :
				*((float *) a)=(float) val->val[i];
				a+=4;
				break;
			case IPL_DEPTH_64F :
				*((double *) a) = (double) val->val[i] ;
				a+=8;
				break;
		}
	}
}
 

/*returns a rectangle containing the non-zero pixels of the image*/ 
DllExport CvRect GetNonZeroRect(IplImage* in)
{
	CvRect rect = cvRect(-1,-1,-1,-1);
	
	unsigned char * runner = (unsigned char *) in->imageDataOrigin;

	int skip_line = in->widthStep-in->width;

	for (int i = 0; i < in->height;i++)
	{
		for(int j=0;j<in->width;j++)
		{	
			if(*runner++)
			{
				if(rect.x==-1)
				{
					rect.x=rect.width=j;
				}
				else if(rect.width < j) 
				{
					rect.width=j;
				}
				else if(j < rect.x) 
				{
					rect.x=j;
				}

				if(rect.y==-1)
				{
					rect.y=rect.height=i;
				}
				else if(rect.height < i) 
				{
					rect.height=i;
				}
				else if(i < rect.y) 
				{
					rect.y=i;
				}
			}
		}
		runner+=skip_line;
	}

	if(rect.x!=-1)
	{
		rect.width=rect.width-rect.x;
	}
	else rect.x=rect.width=0;

	if(rect.y!=-1)
	{
		rect.height=rect.height-rect.y;
	}
	else rect.y=rect.height=0;

	return rect;
}

  void convertByte2Float(IplImage *byte_img,IplImage *float_img)
{
	if( (byte_img->width != float_img->width) ||
		(byte_img->height != float_img->height) ||
		(byte_img->nChannels != float_img->nChannels) ||
		(byte_img->depth != IPL_DEPTH_8U) ||
		(float_img->depth != IPL_DEPTH_32F))
	{
		printf("convertByte2Float error.Aborting ... \n");
		exit(0);
	}

	unsigned char * ptr1= (unsigned char *)byte_img->imageData;        	        	
	float * ptr2 = (float *) float_img->imageData;
	unsigned char * runner1;

	for(int i=0;i<byte_img->height;i++)
	{
		runner1=ptr1;
		for(int j=0;j<byte_img->width;j++)
		{
			for(int k=0;k<byte_img->nChannels;k++)
			{
				*ptr2++ = (float) (*runner1++);
			}
		}
		ptr1+=byte_img->widthStep;
	}
}

 void convertFloat2Byte(IplImage *float_img,IplImage *byte_img)
{

	if( (byte_img->width != float_img->width) ||
		(byte_img->height != float_img->height) ||
		(byte_img->nChannels != float_img->nChannels) ||
		(byte_img->depth != IPL_DEPTH_8U) ||
		(float_img->depth != IPL_DEPTH_32F))
	{
		printf(" types %d %d --- %d %d \n",float_img->depth,float_img->nChannels,
											byte_img->depth,byte_img->nChannels);
		
		printf("convertByte2Float error.Aborting ... \n");
		exit(0);
	}

	unsigned char * ptr1= (unsigned char *)byte_img->imageData;        	        	
	float * ptr2 = (float *) float_img->imageData;
	unsigned char * runner1;

	for(int i=0;i<byte_img->height;i++)
	{
		runner1=ptr1;
		for(int j=0;j<byte_img->width;j++)
		{
			for(int k=0;k<byte_img->nChannels;k++)
			{
				*runner1++ = cvRound(*ptr2++);
			}			
		}
		ptr1+=byte_img->widthStep;
	}
}

  void tri_image_diff(IplImage *img1,IplImage *img2,IplImage *t,double threshold,tri_info * diff_info)
{
	int data_element;
	CvScalar a=cvScalarAll(0),b=cvScalarAll(0);

	switch(img1->depth)
	{
			case IPL_DEPTH_8U : case IPL_DEPTH_8S :
				data_element=1;
				break;
			case IPL_DEPTH_32S : case IPL_DEPTH_32F :
				data_element=4;
				break;
			case IPL_DEPTH_64F :
				data_element=8;
				break;
	}

	data_element *=img1->nChannels ;

	unsigned char * ptr1= (unsigned char *) img1->imageData;
	unsigned char * ptr2= (unsigned char *) img2->imageData;
	unsigned char *mask_ptr = (unsigned char*) t->imageData;

	unsigned char *runner1,*runner2,* mask_runner;

	for(int i=0;i<img1->height;i++)
	{
		mask_runner=mask_ptr;
		runner1=ptr1;
		runner2=ptr2;
		for(int j=0;j<img1->width;j++)
		{
			getValue(img1,runner1,&a);
			getValue(img2,runner2,&b);
			runner1+=data_element;
			runner2+=data_element;			
			*mask_runner++ = ( (tri_diff(&a,&b,diff_info) >= threshold) ? 255 : 0);
		}
		runner1+=img1->widthStep;
		runner2+=img2->widthStep;
		mask_ptr+=t->widthStep;
	}
}

void detect_sources(IplImage * img1,IplImage * img2,IplImage *injector)
{
	int runner=0;
	
	printf("Detecting sources \n");
	int hydro_div,hydro_evol;
	cvZero(injector);
	int skip_to_line = 1 + img1->widthStep - img1->width;
		
	runner+=img1->widthStep;
	
	for(int i=0;i<img1->height-2;i++)
	{
		runner++;
		for(int	j=0;j<img1->width-2;j++)
		{
			 hydro_div=sum_cyclic(img2,runner) - sum_cyclic(img1,runner) ;
			 hydro_evol = (int) ((unsigned char) img2->imageData[runner]) -
			 			  (int) ((unsigned char) img1->imageData[runner]);			 
			 injector->imageData[runner++] = ( (hydro_div+hydro_evol) ? 255 : 0 );
		}
		runner+=skip_to_line;
	}			
}

int sum_cyclic(IplImage *img,int runner)
{
	int w=0;
	
	for(int i=-1;i<2;i++)
	{
		for(int j=-1;j<2;j++)
	 	{
	  		if( ! ( (i==0) && (j==0)) )
	  		{
				w += (unsigned char) img->imageData[runner+j * img->widthStep+i];
	  		}
	 	}
	}
	
	return ( (w > 0) ? 1 : 0);
}

 void compareRects(CvRect *rect1,CvRect *rect2,CvRect *rect_dst)
 {
 	int x1,x2;
 	int w1,w2;
 	
 	w1= rect1->x + rect1->width;
 	w2 = rect2->x + rect2->width;
 	
 	x1 = max(rect1->x,rect2->x);
 	x2 = min(w1,w2);
 	
 	rect_dst->x=x1;
 	if(x1 < x2) rect_dst->width=x2-x1;else rect_dst->width=0;
 	
 	w1= rect1->y + rect1->height;
 	w2 = rect2->y + rect2->height;
 	
 	x1 = max(rect1->y,rect2->y);
 	x2 = min(w1,w2);
 	
 	rect_dst->x=x1;
 	if(x1 < x2) rect_dst->height=x2-x1;else rect_dst->height=0; 	
 }
 
 CvSize getCoverage(vector<CvRect *>*rect_vec1,vector<CvRect *>*rect_vec2)
 {
 	vector<CvRect *>::iterator it1,it2;
 	CvRect my_rect;
 	CvSize pp = cvSize(0,0);
 	
 	for(it1=rect_vec1->begin();it1 != rect_vec1->end();it1++)
 	{
 		pp.width += ( (*it1)->width * (*it1)->height);
 		
	 	for(it2=rect_vec2->begin();it2 != rect_vec2->end();it2++)
 		{
 			compareRects(*it1,*it2,&my_rect);
 			pp.height += (my_rect.width * my_rect.height);
 		}
 	}
 	return pp; 	
 }
 
 	
void fithis_hsv2rgb(CvScalar *a,CvScalar *b)
{	
	int i;
	double f, p, q, t;

	a->val[0] /= 60;			// sector 0 to 5
	i = floor( a->val[0] );
	f = a->val[0] - i;			// factorial part of h
	p = a->val[2] * ( 1 - a->val[1] );
	q = a->val[2] * ( 1 - a->val[1] * f );
	t = a->val[2] * ( 1 - a->val[1] * ( 1 - f ) );

	switch( i )
	{
		case 0:
			b->val[0] = a->val[2];
			b->val[1] = t;
			b->val[2] = p;
			break;
		case 1:
			b->val[0] = q;
			b->val[1] = a->val[2];
			b->val[2] = p;
			break;
		case 2:
			b->val[0] = p;
			b->val[1] = a->val[2];
			b->val[2] = t;
			break;
		case 3:
			b->val[0] = p;
			b->val[1] = q;
			b->val[2] = a->val[2];
			break;
		case 4:
			b->val[0] = t;
			b->val[1] = p;
			b->val[2] = a->val[2];
			break;
		default:		// case 5:
			b->val[0] = a->val[2];
			b->val[1] = p;
			b->val[2] = q;
			break;
	}
}

int identifyDataElement(IplImage*img)
{
	switch(img->depth)
	{
	 	case IPL_DEPTH_8U:
	 	case IPL_DEPTH_8S:  return 1;
	 	case IPL_DEPTH_16S: return 2;break;
	 	case IPL_DEPTH_32S:
	 	case IPL_DEPTH_32F: return 4;break;
	 	case IPL_DEPTH_64F: return 8;break;
	 	default : return -1;
	 }
 }

DllExport int polymnia_virtual2realAVI(const char *virt_avi,const char *real_avi)
{
	char buff[256];
	CvVideoWriter* awriter=NULL;
	ifstream *areader=new ifstream(virt_avi);
	IplImage *img;
	CvSize asz=cvSize(704,576);
	IplImage *img2put=NULL;

	printf("going to compress\n");
	int length=0;
	do
	{
		areader->getline((buff),256);
		if(strlen(buff))
		{
			printf("compress %s\n",buff);
			img=cvLoadImage(buff);
			if(!img2put)
			{
				img2put=cvCreateImage(asz,IPL_DEPTH_8U,3);
			}
			cvResize(img,img2put,CV_INTER_LINEAR);
			if(!awriter)
			{
				awriter=cvCreateVideoWriter(real_avi,0,25,asz);
			}
			length++;
			cvWriteFrame(awriter,img2put);
			cvReleaseImage(&img);
		}
		else break;
	}while(true);

	if(img2put) cvReleaseImage(&img2put);
	cvReleaseVideoWriter(&awriter );
	areader->close();
	delete areader;
	return length;
}

DllExport void polymnia_clearvirtualAVI(const char *virt_avi)
{
	char buff[256];
	char stamp_buff[256];
	char folder_buff[256];
	int val;
	int len;
	char *whathappened;
	ifstream *areader=new ifstream(virt_avi);	
	printf("going to cleanup\n");
	do
	{
		areader->getline((buff),256);
		if(strlen(buff))
		{
			val=remove(buff);
			if( val != 0 )
			{
				whathappened=strerror(val);
				printf( "Could not delete file %s because %s\n",buff,whathappened);
				exit(0);
			}			
		}
		else break;
	}while(true);

	delete areader;

	printf("deleting %s\n",virt_avi);
	val=remove(virt_avi);
	if(val != 0)
	{
		whathappened=strerror(val);
		printf( "Could not delete file %s because %s\n",virt_avi,whathappened);
		exit(0);		
	}			

	char *found_file=strstr(virt_avi,"file");

	len= (found_file - virt_avi);

	if(len>255)
	{
		printf("too big pathname %s \n",virt_avi);
		exit(0);
	}
	memcpy(buff,virt_avi,len);
	buff[len]='\0';

	char *found_first=strstr(virt_avi,"__");
	found_first+=2;
	char *found_last=strstr(found_first,"__");

	len = found_last-found_first;
	memcpy(stamp_buff,found_first,len);
	stamp_buff[len]='\0';

	sprintf(folder_buff,"%sa%s",buff,stamp_buff);
	
	printf("deleting %s\n",folder_buff);
	val=_rmdir(folder_buff);

	if( val != 0 )
	{
		
		whathappened=strerror(val);
		printf( "Could not delete folder %s because %s\n",buff,whathappened);
		exit(0);
	}			
}
