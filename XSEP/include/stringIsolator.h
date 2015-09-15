#ifndef STRING_ISOLATOR_H
#define STRING_ISOLATOR_H
#include "XSEP.h"
#include "stdio.h"

typedef enum {START_CAPTURE,END_CAPTURE,BATCH_ENCODED} Encoder_Event_Type;

typedef struct _encoder_sms
{
	Encoder_Event_Type anev;
	char file_name[1024];
}encoder_sms;


class stringIsolator
{
public :		
	encoder_sms * pop_SMS();
	void recycle_SMS(encoder_sms *anev);
	void insert_SMS(const char *some_fname,Encoder_Event_Type anev);
	~stringIsolator();
	stringIsolator();
private:
	void *working;
	void *residual;
};
#endif


