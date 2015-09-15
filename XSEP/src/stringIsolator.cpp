#include "stringIsolator.h"
#include <string.h>

#include<vector>
using namespace std;

stringIsolator::stringIsolator()
{
		this->working=new vector<encoder_sms *>(0);
		this->residual=new vector<encoder_sms *>(0);
}

stringIsolator::~stringIsolator()
{
	vector<encoder_sms *>* avec;
	encoder_sms *temp;

	avec=(vector<encoder_sms *> *) this->working;
	while(avec->size()>0)
	{
		temp=avec->front();
		avec->erase(avec->begin());
		delete temp;
	}
	delete avec;

	avec=(vector<encoder_sms *> *) this->residual;
	while(avec->size()>0)
	{
		temp=avec->front();
		avec->erase(avec->begin());
		delete temp;
	}
	delete avec;		
}

void stringIsolator::insert_SMS(const char *some_fname,Encoder_Event_Type anev)
{
	vector<encoder_sms *>* avec;
	
	encoder_sms *some_ev;
	avec=(vector<encoder_sms *> *) this->residual;

	if(avec->size()==0)
	{
		avec->insert(avec->begin(),new encoder_sms);
	}
	some_ev=avec->front();
	avec->erase(avec->begin());
	some_ev->anev=anev;
	strcpy(some_ev->file_name,some_fname);
	avec=(vector<encoder_sms *> *) this->working;
	avec->push_back(some_ev);
}

encoder_sms * stringIsolator::pop_SMS()
{
	encoder_sms * temp;
	vector<encoder_sms *>* avec;
	avec=(vector<encoder_sms *> *) this->working;

	if(avec->size()==0) return NULL;
	temp = avec->front();
	avec->erase(avec->begin());
	return temp;
}

void stringIsolator::recycle_SMS(encoder_sms *anev)
{
	vector<encoder_sms *>* avec;
	avec=(vector<encoder_sms *> *) this->residual;
	avec->push_back(anev);
}
