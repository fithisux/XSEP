#ifndef shadowclass_h
#define shadowclass_h
#include "bgUtilities.h"
#include "fithisConnectedRegions.h"
#include "confParameters.h"
#include "XSEP.h"

class shadowClass
{
public :
		separ_shadow_conf_t *conf_shadow;
		IplImage *previous_frame;
		IplImage *next_frame;
		IplImage *SHADOW_MASK;
		IplImage *previous_separation;
		IplImage *next_separation;
		IplImage *deshaded;
		fithisConnectedRegions *filt;
		unsigned char shadowidentification(unsigned char *a,unsigned char *b);

public :
		shadowClass(separ_shadow_conf_t *some_conf);
		//Class methods:
		void shadow_identification_new(IplImage *frame,IplImage *separation);
		IplImage * getRealFG();
		IplImage * getShadow();		
		//Destructor:
        ~shadowClass();
};

#endif

