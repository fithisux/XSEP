#include "successiveKMeans.h"
#include <stdio.h>

successiveKMeans::successiveKMeans(int asize)
{
	this->size = asize; // we use this->size number of cores
	this->cores = new IplImage* [asize]; // the cores (per-pixel center of mass)
	this->counter = NULL;//updates per core
}

successiveKMeans::~successiveKMeans()
{
	for(int i=0;i<this->size;i++)
	{
		cvReleaseImage(&cores[i]);
	}
	delete this->cores;
	delete this->counter;
}


// If not ready , it initializes the cores
// else it updates the nearest core to frame and updates the
// number of updates for this particular core
void successiveKMeans::updateCore(IplImage *frame)
{
	static int len=0;
	unsigned char *s,*v;
	int the_class;

	//Is it ready?
	if(! this->isReady())
	{
		//If not initialize the next uninitialized core
		this->cores[len++]=cvCloneImage(frame);
		if(len == this->size)
		{
			this->counter = new int[this->size];
			for(int i=0;i<this->size;i++) this->counter[i]=1;
		}
	}
	else
	{
		//It is ready, for this reason it updates per pixel
		//center of mass for the nearest core
		the_class=this->findClass(frame);
		if(the_class == -1)
		{
			printf("could not classify \n");
			return;
		}
		else	printf("Class %d \n",the_class);

		v= (unsigned char *) frame->imageDataOrigin;
		s= (unsigned char *) this->cores[the_class]->imageDataOrigin;

		printf("Finding the center of mass \n");
		for(int i=0;i<frame->imageSize;i++)
		{

			(*s) = (unsigned char) (this->counter[the_class]*(*s)+(*v))/(this->counter[the_class]+1.0);
			s++;v++;
		}
		this->counter[the_class]++;
	}
}


//It tries to find out the class
//that frame belongs to.
//It does it using the L2 norm. It sorts the results
//and select the nearest core. If cores are empty
//it returns -1
int successiveKMeans::findClass(IplImage *frame)
{
	int index=-1;
	double temp_cost1,temp_cost2;

	for(int i=0;i<this->size;i++)
	{
		if(i==0)
		{
			temp_cost1=cvNorm(frame,this->cores[i],CV_L2,NULL);
			index=0;
		}
		else
		{
			temp_cost2=cvNorm(frame,this->cores[i],CV_L2,NULL);
			if(temp_cost2 < temp_cost1)
			{
				index=i;
				temp_cost1=temp_cost2;
			}
		}
	}
	return index;
}

//Checks if the KMeans is ready	,in other words
//if all the this->size cores are initially filled
bool successiveKMeans::isReady()
{
	return (this->counter != NULL);
}
