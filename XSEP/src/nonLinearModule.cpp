#include "nonLinearModule.h"


bool nonLinearModule::isReady(){return this->classifier->isReady();}

void nonLinearModule::getClasses()
{
	printf("Forming cores\n");
	this->classifier->formCores();
	printf("Classifying \n");
	this->classifier->classifyAll();
	printf("We are ready \n");
	this->classes= this->classifier->sorted;
	for(int i=0;i < this->numOfCores;i++)
	{
		if(this->classes[i].size() > 0)
		{
			this->key_frames_index[i]=this->classes[i].at(0)->Id;
		}
		else this->key_frames_index[i]=-1;
	}	
}

vector<KMeansTriple *>* nonLinearModule::getXML(char **xml_name)
{
	this->getClasses();
	FILE *fp=fopen(this->xml_file,"w");

	fprintf(fp,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	fprintf(fp,"<cores>\n");
	for(int i=0;i < this->numOfCores;i++)
	{
		if( this->key_frames_index[i] >= 0)
		{
			fprintf(fp,"<classes id=\"%d\" key-frame=\"%d\" >\n",i,this->key_frames_index[i]);
			for(int j=0;j<classes[i].size();j++)
			{
				fprintf(fp,"<frame id=\"%d\" />\n",classes[i].at(j)->Id);
			}
			fprintf(fp,"</classes>\n",i,this->key_frames_index[i]);
		}
	}
	fprintf(fp,"</cores>\n");
	fclose(fp);
	*xml_name=this->xml_file;
	return classes;
}
nonLinearModule::nonLinearModule(const char *avirtavi,int anumOfCores,int anumOfBins)
{	
	this->numOfCores=anumOfCores;
	this->numOfBins=anumOfBins;
	this->classes=NULL;	
		
	printf("Start with %s \n",avirtavi);

	this->virtual_avi=new char [strlen(avirtavi)+1];
	strcpy(this->virtual_avi,avirtavi);
	
	this->xml_file=new char[strlen(this->virtual_avi)+1];
	strcpy(this->xml_file,this->virtual_avi);
	char *z=strstr(this->xml_file,"avi");
	z[0]='n';z[1]='l';z[2]='m';
	
	this->training_set=0;

	ifstream *areader=new ifstream(this->virtual_avi);	
	char buff[256];	
	
	IplImage *trash_image;
	IplImage *resize_image;
	int val;
	char* whathappened;

	resize_image=NULL;
	do
	{
		areader->getline((buff),256);
		if(strlen(buff))
		{
			trash_image= cvLoadImage(buff);
			if(!resize_image)
			{
				resize_image=cvCreateImage(cvSize(704,576),IPL_DEPTH_8U,3);
			}
			cvResize(trash_image,resize_image);

			val=remove(buff);
			if( val != 0 )
			{
				whathappened=strerror(val);
				printf( "Could not delete file %s because %s\n",buff,whathappened);
				exit(0);
			}	

			cvSaveImage(buff,resize_image);
			cvReleaseImage(&trash_image);	
			this->training_set++;
		}
		else break;
	}while(true);

	if(resize_image)cvReleaseImage(&resize_image);
	areader->close();
	delete areader;

	this->training_index=0;
	this->key_frames_index=new int[numOfCores];
	for(int i=0;i<this->numOfCores;i++)this->key_frames_index[i]=-1;
	this->classifier=new KMeans(numOfCores,this->training_set,numOfBins);	
}

bool nonLinearModule::processFrames()
{
	ifstream *areader=new ifstream(this->virtual_avi);
	IplImage *img;
	char buff[256];	
	do
	{
		areader->getline((buff),256);
		if(strlen(buff))
		{
			img=cvLoadImage(buff);
			this->classifier->updateCore(img,NULL);			
			++this->training_index;
			cvReleaseImage(&img);
		}
		else break;
	}while(true);

	areader->close();
	delete areader;

	if(this->training_index==this->training_set)
	{
		printf("done with frames\n");
		return true;
	}
	else
	{
		printf("Not done with frames. Aborting ... \n");
		return false;
	}
}

nonLinearModule::~nonLinearModule()
{	
	delete this->classifier;
	delete [] this->key_frames_index;
	delete[] this->virtual_avi;
	delete[] this->xml_file;
}


