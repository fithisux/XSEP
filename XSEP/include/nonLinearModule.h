#ifndef NONLINEAR_MODULE_H
#define NONLINEAR_MODULE_H

#include "KMeans.h"
#include "bgUtilities.h"

class nonLinearModule
{
	
private :
		
	char *xml_file;
	char *virtual_avi;

	KMeans *classifier;
	int frame_index;
	int training_index;
	int numOfCores;
	int numOfBins;
	int * key_frames_index;
	int training_set;
	vector<KMeansTriple *>  *classes;
	void getClasses();
	
public :
	
	vector<KMeansTriple *>* getXML(char **xml_name);
	int getLength(){return this->training_set;}
	nonLinearModule(const char *avirtavi,int anumOfCores,int anumOfBins);
	~nonLinearModule();
	IplImage ** recoverKeyFrames(int ** key_index);
	bool isReady();	
	bool processFrames();
	void enterImageAndRects(IplImage *frame);	
};

#endif
