#ifndef CONDENSTATIONCORE_H_
#define CONDENSTATIONCORE_H_

//#define CONDENSATION_CLUSTER

#include "bgUtilities.h"


typedef struct _condensationConf
{
	tri_info *diff_info;
	double threshold;
}condensationConf;

typedef struct _condensationCoreConf
{
	condensationConf * acconf;
	int length;
	CvScalar **swap;
	int neighborNum;
}condensationCoreConf;

typedef struct _condensationCore
{
	public :	
	condensationCoreConf *conf;
	CvScalar **chain;	
	int counter;					
}condensationCore;

typedef struct _condensationCoreAggregate
{	
	condensationCore * core_bg;
	condensationCore * core_fg;	
}condensationCoreAggregate;


	condensationCore* createCC(condensationCoreConf *aconf);
	void destroyCC(condensationCore** CC);
	void pushFrontCC(condensationCore* CC,CvScalar *p);
	bool isFullCC(condensationCore* CC);	
	void getMeanCC(condensationCore* CC,CvScalar *p,int length);
	void emptyItCC(condensationCore* CC);	
	bool findKNeighborsCC(condensationCore* CC,CvScalar *p);
	bool areNeighborsCC(condensationCore* CC);
	bool updateWithPairCC(condensationCore* CC,CvScalar *p);
	void substituteCoresCC(condensationCore* srcCC,condensationCore* dstCC);
	condensationCoreAggregate* createCCAggregate(condensationCoreConf *a_bg_conf,condensationCoreConf *a_fg_conf);
	void destroyCCAggregate(condensationCoreAggregate** CCag);
	bool classifyCCAggregate(condensationCoreAggregate* CCag,CvScalar *p);
	void predictCCAggregate(condensationCoreAggregate* CCag,CvScalar *pred);
	condensationCoreConf * createCCConf(int the_N,int the_K,condensationConf *some_conf);
	void destroyCCConf(condensationCoreConf ** aconf);
		
#endif /*CONDENSTATIONCORE_H_*/
