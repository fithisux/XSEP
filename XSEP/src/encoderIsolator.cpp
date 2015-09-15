#include "encoderIsolator.h"
#include "GrabberThread.h"
#include "AustrianThread.h"
#include "SpanishThread.h"
pt::mutex encoder_general_lock;

encoderIsolator::encoderIsolator(encoder_conf_t *pt,int aStamp)
{
	stateOfEncoder * the_state=new stateOfEncoder(pt);
	this->glob_state=the_state;	
	this->grabber_th=new grabberThread(the_state);
	this->austrian_th=new austrianThread(the_state);
	this->spanish_th=new spanishThread(the_state);
	((pt::thread *)this->grabber_th)->start();
	((pt::thread *)this->austrian_th)->start();
	((pt::thread *)this->spanish_th)->start();

}
encoderIsolator::~encoderIsolator()
{
	delete (grabberThread*) this->grabber_th;
	delete (austrianThread*) this->austrian_th;
	delete (spanishThread*) this->spanish_th;
	delete (stateOfEncoder*) this->glob_state;
}

encoder_sms * encoderIsolator::getNames()
{
	encoder_sms * el;

	encoder_general_lock.lock();
	el= ((stateOfEncoder *)this->glob_state)->rtc_mesg_queue->pop_SMS();
	encoder_general_lock.unlock();

	return el;
}

void encoderIsolator::recycle_SMS(encoder_sms *el)
{	
	((stateOfEncoder *)this->glob_state)->rtc_mesg_queue->recycle_SMS(el);
}
