#ifndef GRABBERTHREAD_H_
#define GRABBERTHREAD_H_
#include "encoderSpecifics.h"
#include "hiddenVideoWriter.h"
#include "axisTransaction.h"
//#define GRABBER_LOG
#include <pasync.h>    // for psleep()
#include <ptime.h>
#include <pstreams.h>
USING_PTYPES

class grabberThread : public pt::thread
{
	protected :
	char cam_command[1024];
	datetime base_time_stamp;
	char current_time_stamp[256];
	char current_directory[256];
	char current_frame[256];
	char current_prefix[256];
	frame_description *current_data;
	axisTransaction  *trans;
	int count_frames;
	stateOfEncoder *the_state;
	virtual void execute();
	void resetMe();
	void finalizeMe();
	public:
	grabberThread(stateOfEncoder *astate);
	~grabberThread();

	void sendStartOfCapture();
	void sendEndOfCapture();

};
#endif

