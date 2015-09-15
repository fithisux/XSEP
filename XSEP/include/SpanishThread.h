#ifndef SPANISHTHREAD_H_
#define SPANISHTHREAD_H_
#include "encoderSpecifics.h"
//#define SPANISH_LOG
class spanishThread : public pt::thread
{
	protected :

	bool firstTime;
	
	pt::ipstream *client;
	pt::ipstmserver *svr;
	char *current_timestamp;
	frame_description *swap;
	stateOfEncoder *the_state;
	bool write_frame();	
	virtual void execute();
	void resetMe();
	public:		
	spanishThread(stateOfEncoder *astate);
	~spanishThread();
};
#endif
