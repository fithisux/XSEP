//#include "FrameWienerPredictor.h"
#ifndef kmeans_h
#define kmeans_h

#include<vector>
#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
using namespace std;


typedef struct KMeansTripleStruct
{
	int Id;
	double Diff;
	int classID;
}KMeansTriple;

class  KMeans
{	

private :

	float **cores;
	float **old_cores;
	float **candyset;
	int numOfBins;
	int *itemNum;
	float** rangesR,** rangesG,** rangesB;
	CvHistogram* histR, *histG, *histB;
	IplImage *planeR,*planeB,*planeG;
	int numOfCores;
	int numOfSamples;
	float *imagehist;
	int index;
	int componentLength;
	KMeansTriple findClass(IplImage *frame);
	
	public :
	vector<KMeansTriple *> *sorted;	
	void classifyAll();
	void formCores();
	int getNumOfCores() { return this->numOfCores;}
	int getNumOfSet() { return this->numOfSamples;}
	KMeans(int anumOfCores,int anumOfSamples,int numOfBins);
	~KMeans();
	void updateCore(IplImage *frame,IplImage *mask);	
	bool isReady();
};
#endif

