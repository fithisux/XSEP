#include "condensationCore.h"

condensationCoreConf * createCCConf(int the_N,int the_K,condensationConf *some_conf)
{
	condensationCoreConf *aconf=new condensationCoreConf;
	aconf->acconf=some_conf;
	aconf->length=the_N;
	aconf->neighborNum=the_K;
	aconf->swap=new CvScalar * [aconf->length];
	return aconf;
}

void destroyCCConf(condensationCoreConf ** aconf)
{
	condensationCoreConf *conf=*aconf;
	delete [] conf->swap;
	conf->acconf=NULL;
	delete conf;
	*aconf=NULL;
}


void destroyCCConf(condensationCoreConf ** aconf);

condensationCore*  createCC(condensationCoreConf *aconf)
{
	condensationCore *CC=new condensationCore;
	CC->conf=aconf;
	if(!aconf)
	{
		printf("NULL \n");;
	}
	if(CC->conf->length <=0)
	{
		printf("Length must be non-zero\n");
		exit(0);
	}
		
	if(CC->conf->neighborNum <=0)
	{
		printf("number of neighbors must be non-zero\n");
		exit(0);
	}

	if(CC->conf->neighborNum > CC->conf->length)
	{
		printf("number of neighbors must less or equal to length\n");
		exit(0);
	}
	
	CC->chain = new CvScalar * [CC->conf->length];
	for(int i=0;i<CC->conf->length;i++) CC->chain[i]=new CvScalar;
	CC->counter=CC->conf->length;
	return CC;
}

void destroyCC(condensationCore** CC)
{
	for(int i=0;i<(*CC)->conf->length;i++) delete (*CC)->chain[i];
	delete[] (*CC)->chain;
	delete *CC;
	(*CC)=NULL;
}

void emptyItCC(condensationCore* CC)
{
	CC->counter=CC->conf->length;
}

bool findKNeighborsCC(condensationCore*CC ,CvScalar *p)
{
		int j=0;
		double diff;		
		for(int i=0;i<CC->conf->length;i++)
		{			
			//diff=norm_infty(CC->chain[i],p);
			diff=tri_diff(CC->chain[i],p,CC->conf->acconf->diff_info);
			if(diff < CC->conf->acconf->threshold)
			{
				if( (++j) == CC->conf->neighborNum) return true;			
			}
		}
		return false;		
}

void pushFrontCC(condensationCore* CC,CvScalar *p)
{
		if(CC->counter)
		{
			printf("Cannot push front if not full \n");
			exit(0);
		}
		//we push the new frame in place 0
		// the others are shifted to the past by 1
		CvScalar *temp = CC->chain[CC->conf->length-1];
		for(int i=CC->conf->length-2;i>=0;i--) CC->chain[i+1]=CC->chain[i];
		for(int l=0;l<4;l++) temp->val[l]= p->val[l];
		CC->chain[0]=temp;
}

void substituteCoresCC(condensationCore* srcCC,condensationCore* dstCC)
{
	if(srcCC->counter || dstCC->counter)
	{
		printf("Cannot substitute if not full \n");
		exit(0);
	}
	
	if(srcCC->conf->length > dstCC->conf->length)
	{
		printf("Cannot copy for bigger \n");
		exit(0);
	}

	CvScalar *temp_src,*temp_dst, **swap;
	int temp_count=dstCC->conf->length - srcCC->conf->length;								
	swap=dstCC->conf->swap;

	//work with the newer	
	for(int i=0;i<srcCC->conf->length;i++)
	{
		swap[i]=srcCC->chain[i]; //the src cores go first to swap
		//and we replace them with the first in dst
		srcCC->chain[i]=dstCC->chain[i+temp_count];
	}
	for(int i=0;i<temp_count;i++) swap[i+srcCC->conf->length]=dstCC->chain[i];	
	for(int i=0;i<dstCC->conf->length;i++) dstCC->chain[i]=swap[i];//swap goes to dst
}

bool isFullCC(condensationCore* CC){return (CC->counter == 0);}

void getMeanCC(condensationCore* CC,CvScalar *p,int length)
{
	if(CC->counter)
	{
		printf("Cannot get mean if not full \n");
		exit(0);
	}
	
	if(CC->conf->length < length)
	{
		printf("Cannot get mean for longer subsequence \n");
		exit(0);
	}

	for(int l=0;l<4;l++)
	{
		p->val[l]=0;	
		for(int i=0;i<length;i++) p->val[l]+=CC->chain[i]->val[l];
		p->val[l]/=length;		
	}	
}

bool areNeighborsCC(condensationCore* CC)
{
		double succ_diff;
#ifdef CONDENSATION_CLUSTER
		CvScalar mean;		
		getMeanCC(CC,&mean,CC->conf->length);
#endif

		for(int i=0;i<CC->conf->length-1;i++)
		{
			//succ_diff =norm_infty(CC->chain[i],CC->chain[i+1]);
			succ_diff =tri_diff(CC->chain[i],CC->chain[i+1],CC->conf->acconf->diff_info);
			if(succ_diff >= CC->conf->acconf->threshold) return false;
#ifdef CONDENSATION_CLUSTER
			//succ_diff =norm_infty(CC->chain[i],&mean);
			succ_diff =tri_diff(CC->chain[i],&mean,CC->conf->acconf->diff_info););
			if(succ_diff >= CC->conf->acconf->alpha) return false;			
#endif
		}		
		return true;
}

bool updateWithPairCC(condensationCore* CC,CvScalar *p)
{
	bool resus;
	CvScalar *temp;	
	if(CC->counter > 0)
	{
		temp=CC->chain[--CC->counter];
		for(int l=0;l<4;l++) temp->val[l]= p->val[l];
		return true;
	}
	else
	{
		resus = findKNeighborsCC(CC,p);		
		if(resus) 	pushFrontCC(CC,p);
		return resus;
	}
}

condensationCoreAggregate* createCCAggregate(condensationCoreConf *a_bg_conf,condensationCoreConf *a_fg_conf)
{
	condensationCoreAggregate * CCag = new condensationCoreAggregate;
	CCag->core_bg = createCC(a_bg_conf);
	CCag->core_fg = createCC(a_fg_conf);
	return CCag;
}

void destroyCCAggregate(condensationCoreAggregate** CCag)
{
	destroyCC( &(*CCag)->core_bg);
	destroyCC( &(*CCag)->core_fg);
	delete *CCag;
	*CCag=NULL;
}

bool classifyCCAggregate(condensationCoreAggregate* CCag,CvScalar *p)
{
	if( ! updateWithPairCC(CCag->core_bg,p))
	{
		if(isFullCC(CCag->core_fg)) pushFrontCC(CCag->core_fg,p);
		else updateWithPairCC(CCag->core_fg,p);
				
		if(isFullCC(CCag->core_fg) && areNeighborsCC(CCag->core_fg))
		{
			substituteCoresCC(CCag->core_fg,CCag->core_bg);				
		}
		else return true;
	}

	emptyItCC(CCag->core_fg);
	return false;
}

void predictCCAggregate(condensationCoreAggregate* CCag,CvScalar *pred)
{
	CvScalar mean;
	getMeanCC(CCag->core_bg,&mean,CCag->core_fg->conf->length);
	for(int l=0;l<4;l++) pred->val[l]=mean.val[l];	
}
