#include "SpanishThread.h"

extern pt::mutex encoder_general_lock;

spanishThread::spanishThread(stateOfEncoder *astate):pt::thread(false)
{
	this->svr=new pt::ipstmserver();
	this->the_state=astate;
	this->firstTime=true;
	
	try
	{
		this->svr->bindall(astate->encoder_conf->params->spanish_port);
	}
	catch(pt::estream *e)
	{
		pt::perr.putf("FATAL : %s \n",pt::pconst(e->get_message()));
		throw e;
	}
	this->client=new pt::ipstream();
	this->swap=new frame_description();
}

spanishThread::~spanishThread()
{
	delete this->client;
	delete this->svr;
	delete this->swap;
}

void spanishThread::execute()
{
	int resus;
	pt::pout.putf("Spanish is running\n");
	bool haveStarted;
	bool hasConnected;
	bool haveFailed;
	while(true)
	{
		encoder_general_lock.lock();
		haveStarted=this->the_state->isActiveGrabber;
		encoder_general_lock.unlock();

		if(!haveStarted)
		{
			pt::psleep(10000);
			continue;
		}

		pt::pout.putf("Spanish waits for connection \n");						
		
		this->svr->serve(*this->client);		
		if(!this->client->get_active()) pt::psleep(10000);
		else
		{
			pt::pout.putf("Spanish notifies \n");

			encoder_general_lock.lock();
			this->the_state->isActiveSpanish=true;
			encoder_general_lock.unlock();
			
			pt::pout.putf("Spanish started \n");	
			haveFailed=false;
			while(this->client->get_active())
			{				
#ifdef SPANISH_LOG
				pout.putf("Spanish pushing \n");						
#endif
				while(true)
				{
					encoder_general_lock.lock();
					this->current_timestamp=this->the_state->spanish_frame->getTimeStamp();
					resus=strcmp(this->current_timestamp,this->swap->getTimeStamp());
					if(resus!=0)
					{						
						this->the_state->spanish_frame->copy(this->swap);
					}
					encoder_general_lock.unlock();				
					if(resus!=0) break;
					pt::psleep(20);
				}
#ifdef SPANISH_LOG
				printf("We stole something \n");
#endif				
				if(!this->write_frame())
				{
					pt::pout.putf("Connection broken \n");
					haveFailed=true;
				}

				encoder_general_lock.lock();
#ifdef SPANISH_LOG
				printf("We put it back \n");
#endif
				encoder_general_lock.unlock();
				if(haveFailed) break;
			}
			this->resetMe();			
			pt::pout.putf("Spanish stoped abruptly\n");			
			encoder_general_lock.lock();
			this->the_state->isActiveSpanish=false;
			encoder_general_lock.unlock();	
		}
	}
}

bool spanishThread::write_frame()
{
	char ss[200];

	try
	{		
#ifdef SPANISH_LOG		
		pout.putf("Spanish put one !!!!!\n");
#endif
		if(this->firstTime)
		{
			this->firstTime=false;
			this->client->put("HTTP/1.0 200 OK\r\n");
			this->client->put("Content-Type: multipart/x-mixed-replace;boundary=myboundary\r\n");
			this->client->put("\r\n");
		}		
		sprintf(ss,"--myboundary[%s]\r\n",this->swap->getTimeStamp());
		this->client->put(ss);
#ifdef SPANISH_LOG
		pout.putf("Spanish putting the meat %s!!!!! \n",this->swap->getTimeStamp());
#endif
		sprintf(ss,"Content-Length: %d\r\n\r\n",this->swap->getLength());
#ifdef SPANISH_LOG
		pout.putf("Spanish putting %d bytes!!!!! \n",this->swap->getLength());
#endif
		this->client->put("Content-Type: image/jpeg \r\n");
		this->client->put(ss);
		this->client->write(this->swap->getBlob(),this->swap->getLength());
#ifdef SPANISH_LOG
		pout.putf("Spanish data available!!!!! \n");
#endif
		this->client->put("\r\n");
		this->client->flush();	
	}
	catch(pt::estream *e)
	{
		pt::perr.putf("Error on writing %s\n",(const char *)e->get_message());
		delete e;
		return false;
	}
	pt::psleep(90);
	return true;
}

void spanishThread::resetMe()
{
	this->client->close();
	delete this->client;
	this->client=new pt::ipstream();
	this->firstTime=true;	
#ifdef SPANISH_LOG
	printf("We are finally back \n");
#endif	
	encoder_general_lock.lock();
	this->the_state->isActiveSpanish=false;
	encoder_general_lock.unlock();
}
