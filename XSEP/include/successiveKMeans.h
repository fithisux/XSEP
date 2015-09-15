#ifndef successive_kmeans_h
#define successive_kmeans_h
#include "NTUAPredictor.h"

class  successiveKMeans
{
		
	public :
	
	IplImage** cores;
	int size;
	int* counter;
	public :

	successiveKMeans(int size);
	~successiveKMeans();
	void updateCore(IplImage *frame);
	int findClass(IplImage *frame);
	bool isReady();
};
#endif
