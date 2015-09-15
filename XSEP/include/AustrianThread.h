#ifndef AUSTRIANTHREAD_H_
#define AUSTRIANTHREAD_H_
#include "encoderSpecifics.h"
//#define AUSTRIAN_LOG
class austrianThread : public pt::thread
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
	austrianThread(stateOfEncoder *astate);
	~austrianThread();
};
#endif
