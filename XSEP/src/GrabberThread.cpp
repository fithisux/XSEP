#include "GrabberThread.h"
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

extern pt::mutex encoder_general_lock;

grabberThread::grabberThread(stateOfEncoder *astate):pt::thread(false)
{
	this->the_state=astate;
	this->count_frames=0;
	this->trans=NULL;
	this->current_data=NULL;
	int index;
	index=sprintf(this->cam_command,"GET /axis-cgi/mjpg/video.cgi?fps=25 HTTP/1.0 \r\n");
	index+=sprintf(&this->cam_command[index],"From: vanag@telecom.ntua.gr\r\n");
	index+=sprintf(&this->cam_command[index],"User-Agent: HawkNL\r\n\r\n");
}

grabberThread::~grabberThread()
{
	if(this->trans) delete this->trans;
	this->current_data=NULL;
}

void grabberThread::execute()
{
	pt::pout.putf("Grabber is running\n");
	bool checkThread;
	FILE *fp=NULL;
    int year,month,day,hours, mins, secs, msecs;
    int diff_hours,diff_mins,diff_secs,diff_msecs;
    datetime timespot;
	int batch_size=this->the_state->encoder_conf->params->batch_size;

	while(true)
	{
		encoder_general_lock.lock();
		this->the_state->isActiveGrabber=true;
		encoder_general_lock.unlock();

		if(!this->trans)
		{
			this->trans=new axisTransaction(this->the_state->encoder_conf->camera->address,this->the_state->encoder_conf->camera->port);
			this->current_data=this->trans->getFrame();
			this->base_time_stamp=pt::now();
            decodedate(this->base_time_stamp, year,month,day);
            decodetime(this->base_time_stamp, hours, mins, secs, msecs);
            sprintf(this->current_prefix,"cam%d_%.4d_%.2d_%.2d_%.2d_%.2d",
					this->the_state->encoder_conf->params->encoder_number,
					year,month,day,hours,mins);
            sprintf(this->current_directory,"%s\\%s",
					this->the_state->encoder_conf->params->video_path,
					this->current_prefix);
			mkdir(this->current_directory);
			if(!this->trans->sendRequest(this->cam_command))
			{
				printf("Camera did not respond\n");
				break;
			}
			pt::pout.putf("Camera responded\n");
		}

		this->trans->safeGetResponse();
		this->current_data->setTimeStamp();

        decodetime(this->base_time_stamp, diff_hours,diff_mins,diff_secs,diff_msecs);

        timespot=pt::now();
        decodetime(timespot, hours, mins, secs, msecs);
		decodedate(timespot, year,month,day);

		this->base_time_stamp=timespot;

        if( (diff_hours!=hours) || (diff_mins!=mins))
        {
            sprintf(this->current_prefix,"cam%d_%.4d_%.2d_%.2d_%.2d_%.2d",
					this->the_state->encoder_conf->params->encoder_number,
					year,month,day,hours,mins);
            sprintf(this->current_directory,"%s\\%s",
					this->the_state->encoder_conf->params->video_path,
					this->current_prefix);
			mkdir(this->current_directory);
        }


		if(this->the_state->encoder_conf->params->video_save)
		{
			sprintf(this->current_frame,"%s\\%s_%.2d_%.3d.jpg",
				this->current_directory,
				this->current_prefix,
				secs,msecs);
			fp=fopen(this->current_frame,"wb");
			fwrite(this->current_data->getBlob(),1,this->current_data->getLength(),fp);
			fflush(fp);
			fclose(fp);
		}

		if(this->count_frames==1) this->sendStartOfCapture();
		else if(this->count_frames==batch_size)	this->finalizeMe();

		//Austrian action
		encoder_general_lock.lock();
		checkThread=this->the_state->isActiveAustrian;
		encoder_general_lock.unlock();

		if(checkThread)
		{
			encoder_general_lock.lock();
			this->current_data->copy(this->the_state->austrian_frame);
			encoder_general_lock.unlock();
		}

		//Spanish action
		encoder_general_lock.lock();
		checkThread=this->the_state->isActiveSpanish;
		encoder_general_lock.unlock();

		if(checkThread)
		{
			encoder_general_lock.lock();
			this->current_data->copy(this->the_state->spanish_frame);
			encoder_general_lock.unlock();
		}

#ifdef GRABBER_LOG
		printf("Austrian and Spanish checked\n");
#endif
	}
	printf("Caught a break, someone has failed\n");
	this->finalizeMe();
	this->resetMe();
}

void grabberThread::resetMe()
{
	encoder_general_lock.lock();
	this->the_state->isActiveGrabber=false;
	delete this->trans;
	this->trans=NULL;
	encoder_general_lock.unlock();
}


void grabberThread::finalizeMe()
{
	FILE *fp=NULL;
	this->count_frames=0;
	this->sendEndOfCapture();
}

void grabberThread::sendStartOfCapture()
{
	char ss[1024];
	int index;

	index=sprintf(ss,"%s",this->current_directory);

	encoder_general_lock.lock();
	this->the_state->rtc_mesg_queue->insert_SMS((const char *)ss,START_CAPTURE);
	encoder_general_lock.unlock();
}

void grabberThread::sendEndOfCapture()
{
	char ss[1024];
	int index;

	index=sprintf(ss,"%s",this->current_directory);

	encoder_general_lock.lock();
	this->the_state->rtc_mesg_queue->insert_SMS((const char *)ss,END_CAPTURE);
	encoder_general_lock.unlock();
}

