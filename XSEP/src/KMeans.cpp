#include "KMeans.h"
#include <stdio.h>

KMeans::KMeans(int anumOfCores,int anumOfSamples,int anumOfBins)
{
	int i;

	this->numOfCores= anumOfCores; 
	this->numOfSamples=anumOfSamples;
	this->numOfBins=anumOfBins;
	this->itemNum=new int [this->numOfCores];
	this->componentLength = 3*this->numOfBins;
	this->planeB=this->planeG=this->planeR=NULL;
	this->cores = new float * [this->numOfCores];
	this->old_cores = new float * [this->numOfCores];
	for(i=0;i<this->numOfCores;i++)
	{
		this->cores[i] = new float[this->componentLength];		
		this->old_cores[i] = new float[this->componentLength];
		for(int j=0;j<this->componentLength;j++)
		{
			this->cores[i][j]=-2;
			this->old_cores[i][j]=-2;
		}
	}

	if(this->numOfSamples)
	{
		this->candyset = new float * [this->numOfSamples];
		for(i=0;i<this->numOfSamples;i++) this->candyset[i] = new float[this->componentLength];
	}
	else this->candyset=NULL;

	this->imagehist=new float[this->componentLength];	

	this->rangesR = new float * [1];
	this->rangesR[0] = new float[2];
	this->rangesR[0][0]=0;this->rangesR[0][1]=255;

	this->rangesG = new float * [1];
	this->rangesG[0] = new float[2];
	this->rangesG[0][0]=0;this->rangesG[0][1]=255;

	this->rangesB = new float * [1];
	this->rangesB[0] = new float[2];
	this->rangesB[0][0]=0;this->rangesB[0][1]=255;

	this->histR = cvCreateHist(1,&this->numOfBins,CV_HIST_ARRAY,rangesR,1);
    this->histG = cvCreateHist(1,&this->numOfBins,CV_HIST_ARRAY,rangesG,1);
    this->histB = cvCreateHist(1,&this->numOfBins,CV_HIST_ARRAY,rangesB,1);
	 
	this->sorted = new vector<KMeansTriple *> [this->numOfCores];
	this->index=0;
}

KMeans::~KMeans()
{
	int i;
	vector<KMeansTriple*>::iterator p;

	
	for(i=0;i<this->numOfCores;i++)
	{		
		for(p=this->sorted[i].begin();p!=this->sorted[i].end();) delete *p++;
		this->sorted[i].erase(this->sorted[i].begin(),this->sorted[i].end());
		delete this->cores[i];
		delete this->old_cores[i];
	}
	delete[] this->itemNum;
	delete[] this->sorted;
	delete[] this->cores;
	delete[] this->old_cores;
	for(i=0;i<this->numOfSamples;i++)
	{
		delete this->candyset[i];
	}
	if(this->numOfSamples) delete[] this->candyset;

	delete[] this->imagehist;

	cvReleaseHist(&this->histR);
	cvReleaseHist(&this->histG);
	cvReleaseHist(&this->histB);

	if(this->planeR) cvReleaseImage(&this->planeR);
	if(this->planeG) cvReleaseImage(&this->planeG);
	if(this->planeB) cvReleaseImage(&this->planeB);

	delete this->rangesR[0];
	delete[] this->rangesR;
	delete this->rangesG[0];
	delete[] this->rangesG;
	delete this->rangesB[0];
	delete[] this->rangesB;
}


// If not ready , it initializes the cores
// else it updates the nearest core to frame and updates the
// number of updates for this particular core
void KMeans::updateCore(IplImage *frame,IplImage *mask)
{	
	int i;

	if(!this->numOfSamples) return;

	if(this->index==0)
	{
		this->planeR = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		this->planeG = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		this->planeB = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	}
	
	cvSplit(frame,this->planeR,this->planeG,this->planeB,0);

	cvCalcHist(&this->planeR,this->histR,0,mask);
	cvCalcHist(&this->planeG,this->histG,0,mask);
	cvCalcHist(&this->planeB,this->histB,0,mask);

	cvNormalizeHist(this->histR,1.0);
	cvNormalizeHist(this->histG,1.0);
	cvNormalizeHist(this->histB,1.0);
	
	float *runner=this->candyset[this->index++];
	
	for(i=0;i<this->numOfBins;i++)
	{ 
		*runner++=cvQueryHistValue_1D(this->histR,i);
	}

	for(i=0;i<this->numOfBins;i++)
	{ 
     *runner++=cvQueryHistValue_1D(this->histG,i);
	}

	for(i=0;i<this->numOfBins;i++)
	{ 
     *runner++=cvQueryHistValue_1D(this->histB,i);
	}
}


void KMeans::formCores()
{
	int m,k,j,temp,MinIndex;
	int iteration_index=0;
	float error,Diff,MinDiff;

	printf("Form the cores \n");
	if(!this->numOfSamples) return;
	
	for(m=0;m<this->numOfCores;m++)
	{
		memcpy(this->cores[m],this->candyset[m],sizeof(float) * this->componentLength);
	}
	
	error=10000;
	
	while(error > 0.000001)
	{       
		printf("Iteration %d with error %f\n",iteration_index++,error);
		
		//store old cores
		for (m=0;m<this->numOfCores;m++)
		{			
            for (j=0;j<this->componentLength;j++)	   
			{
				this->old_cores[m][j]=this->cores[m][j];
				//and set to zero the values of the cores
				this->cores[m][j]=0;										  
			}
			//and set to zero the counter
			this->itemNum[m]=0;
		}
		
		//for every sample
		for(MinIndex=-1,k=0;k<this->numOfSamples;k++)
		{
			//check for every core
			for (MinDiff=10000,m=0;m<this->numOfCores;m++)
			{
				//the L^{1} distance from cores
				for (Diff=0,j=0;j<this->componentLength;j++)	   
				{
				   Diff +=(float) fabs(this->candyset[k][j]-this->old_cores[m][j]);
				}
                
				//and find the closest core
				if(MinDiff>Diff)
				{
					MinDiff=Diff;
					MinIndex=m;
				}			  
			}
			//we know the closest core and this sample belongs there
			this->itemNum[MinIndex]++;

			//update the sum of values for this particular core
			for (j=0;j<this->componentLength;j++)	   
			{
				this->cores[MinIndex][j]+=(float) this->candyset[k][j];				
			}
		}

		//after the pass we know the closest cores and sums , we need to update the cores
		for (error=0,m=0;m<this->numOfCores;m++)
		{
			temp = this->itemNum[m];
			for (j=0;j<this->componentLength;j++)	   
			{
				if(temp !=0) this->cores[m][j] /= (float) temp;
				else this->cores[m][j] = -2;
				error+=(float) fabs(this->old_cores[m][j]-this->cores[m][j]);  				
			}
		}
			
	}		
}

void KMeans::classifyAll()
{
	int m,j,MinIndex;
	float Diff,MinDiff;
	vector<KMeansTriple*>::iterator p;
	printf("Classifying all the samples \n");
	for(int k=0;k<this->numOfSamples;k++)
	{
		for (MinDiff=10000,m=0;m<this->numOfCores;m++)
		{
			for (Diff=0,j=0;j<this->componentLength;j++)	   
			{
			   Diff+=(float) fabs(this->candyset[k][j]-this->cores[m][j]);
			}     
			if(MinDiff>Diff)
			{
				MinDiff=Diff;
				MinIndex=m;
			}			  
		}		

		for(p=this->sorted[MinIndex].begin();p!=this->sorted[MinIndex].end();)
		{
			if((*p)->Diff >= MinDiff) break;
			else p++;
		}

		p=this->sorted[MinIndex].insert(p,new KMeansTriple);
			
		(*p)->Id=k;
		(*p)->Diff=MinDiff;
		(*p)->classID=MinIndex;
	}
}

//It tries to find out the class
//that frame belongs to.
//It does it using the L2 norm. It sorts the results
//and select the nearest core. If cores are empty
//it returns -1
KMeansTriple KMeans::findClass(IplImage *frame)
{	
	float Diff;
	KMeansTriple props;
	int m,j,i,temp;

	cvSplit(frame,this->planeR,this->planeG,this->planeB,0);
	cvCalcHist(&this->planeR,this->histR,0,NULL);
    cvCalcHist(&this->planeG,this->histG,0,NULL);
    cvCalcHist(&this->planeB,this->histB,0,NULL);
    
	for(i=0;i<this->numOfBins;i++)
	{ 
     this->imagehist[i]= cvQueryHistValue_1D(this->histR,i);     
	}

	for(temp=this->numOfBins,i=0;i<this->numOfBins;i++)
	{ 
     this->imagehist[i+temp]= cvQueryHistValue_1D(this->histG,i);     
	}

	for(temp+=this->numOfBins,i=0;i<this->numOfBins;i++)
	{ 
     this->imagehist[i+temp]= cvQueryHistValue_1D(this->histB,i);     
	}

	for(props.classID=-1,props.Diff=10000,m=0;m<this->numOfCores;m++)
	{
		for(Diff=0,j=0;j<this->componentLength;j++)	   
		{
		   Diff+=(float) fabs(this->imagehist[j]-this->cores[m][j]);
		}
                
		if(props.Diff>Diff)
		{
			props.Diff=Diff;
			props.classID=m;
		}
	}	

	props.Id=0;
	return props;
}
	
//Checks if the KMeans is ready	,in other words
//if all the this->numOfCores cores are initially filled
bool KMeans::isReady()
{
	return (this->index >= this->numOfSamples);
}
