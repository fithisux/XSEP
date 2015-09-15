#include "axisTransaction.h"
#include "generalConf.h"

axisTransaction::axisTransaction(const char *camera_url,int port)
{
	pt::ipaddress ad=pt::phostbyname(camera_url);
	this->my_port=port;
	this->first_time=this->second_time=pt::now(false);
	this->frame_counter=0;
	if(ad==pt::ipnone)
	{
		printf("IPNONE\n");
	}
	my_stream=new pt::ipstream(ad,port);
	this->frame=new frame_description();
	this->firstTime=true;
}


axisTransaction::~axisTransaction()
{
	if(this->frame) delete this->frame;
	this->frame=NULL;
	pt::ipstream *the_stream=(pt::ipstream *)my_stream;
	the_stream->close();
	delete the_stream;
	my_stream=NULL;
}

bool axisTransaction::sendRequest(char *mesg)
{
	pt::ipstream *the_stream=(pt::ipstream *)my_stream;
	try
	{
		if(the_stream->get_active()) the_stream->close();
		the_stream->open();
		printf("Go\n");
		printf("Mesage sent is %s \n",mesg);
   		the_stream->putline(mesg);
   		the_stream->flush();
	}
	catch(pt::estream* e)
    {
        pt::perr.putf("Error: %s\n", pt::pconst(e->get_message()));
        delete e;
		return false;
    }
	return true;
}

bool axisTransaction::recoverTimeStamp(const char *ahead)
{
	char *temp1,*temp2,*temp3;
	int timestamp_length;

	if(temp1=strstr(ahead,"--myboundary"))
	{
		if(temp1=strstr(ahead,"--myboundary["))
		{
			temp1=&temp1[13];
			if(temp2=strstr(temp1,"]"))
			{
				timestamp_length=temp2-temp1;
				timestamp_length++;
				temp3=this->frame->getTimeStamp();
				memcpy(temp3,temp1,timestamp_length);
				temp3[timestamp_length-1]='\0';
#ifdef AXIS_LOG
				printf("FOUND TIMESTAMP %s %s\n",temp3,ahead);
#endif
				return true;
			}
		}
		else
		{
#ifdef AXIS_LOG
			printf("FOUND NO TIMESTAMP \n");
			return false;
#endif
		}
	}
	return false;
}

void axisTransaction::getResponse()
{
#ifdef AXIS_LOG
	 printf("FNCT->getResponse\n");
#endif
	 pt::ipstream *the_stream=(pt::ipstream *)my_stream;
     pt::string rsp;
     pt::string white_space(" ");
     pt::string length;
     int the_pos;
     int chars;
	 int some_length;
	 bool found_length=false,found_stamp=false;
	 char *tst=this->frame->getTimeStamp();
	 sprintf(tst,"");
	 this->count_fps();

     do
	 {
		rsp=the_stream->line();
#ifdef AXIS_LOG
		printf("Reading %s\n",(const char *) rsp);
#endif
		if(!found_stamp && this->recoverTimeStamp((const char *) rsp)) found_stamp=true;
		else if(contains("Content-Length:",rsp,0))
		{
			the_pos=2+pt::pos(':',rsp);
			length=copy(rsp,the_pos);
			found_length=true;
#ifdef AXIS_LOG
			printf("Found Length \n");
#endif
			rsp=the_stream->line();
#ifdef AXIS_LOG
            printf("Trailing %s\n",(const char *) rsp); //read the trailing line
#endif
			break;
		}
	}while(true);

	if(found_length)
	{
		while((the_pos=pt::pos(' ',length)) != -1) del(length,the_pos,1);
		some_length=pt::stringtoi(length);
#ifdef AXIS_LOG
		printf("Actual length %d\n",some_length);
#endif
		this->frame->update(some_length);
#ifdef AXIS_LOG
		printf("Actual reading \n");
#endif
		chars=the_stream->read(this->frame->getBlob(),some_length);
#ifdef AXIS_LOG
		printf("Read chars %d from %d and data are %d\n",chars,this->frame->getLength(),some_length);
#endif
	}
	else
	{
#ifdef AXIS_LOG
		printf("Length without type. Bad !!! \n");
#endif
	}
}

bool axisTransaction::safeGetResponse()
{
	try
	{
		this->getResponse();
	}
	catch(pt::estream* e)
	{
		pt::perr.putf("Error: %s\n", pt::pconst(e->get_message()));
		delete e;
		return false;
	}
	return true;
}

void axisTransaction::count_fps()
{
	double diff,fps;
	this->second_time=pt::now(false);

	if(++this->frame_counter==100)
	{
		diff= (double) (this->second_time-this->first_time);
		fps= (1000.0 * this->frame_counter) / diff;
		pt::pout.putf("FPS IS : %f \n",fps);
		this->frame_counter=0;
		this->first_time=this->second_time;
	}
}

frame_description* axisTransaction::getFrame()
{
	return this->frame;
}

void axisTransaction::setFrame(frame_description*aframe)
{
	this->frame=aframe;
}
