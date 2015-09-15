#include "encoderIsolator.h"
#include "encoderConfigurator.h"
#include <windows.h>
#include <string>
using namespace std;


int main(int argc,char **argv)
{
	
	encoderIsolator *isol;
	encoder_sms *mm;
	encoderConfigurator *enc_conf;
    // TODO: Please replace the sample code below with your own.

	if(argc < 2)
	{
		printf("Usage NewEncoder <encoder.xml> \n");
		exit(0);
	}

	try
	{
		enc_conf=new encoderConfigurator(argv[1]);
	}	
	catch(std::string &s)
	{
		printf("Error %s \n",s.c_str());
		return 0;
	}
	
	isol = new encoderIsolator(enc_conf->encoder_configuration,enc_conf->time_offset);

	do
	{
		mm=isol->getNames();

		while(mm)
		{
			switch(mm->anev)
			{
			case START_CAPTURE : printf(">>> START_CAPTURE %s \n",mm->file_name);break;
			case END_CAPTURE : printf(">>> END_CAPTURE %s \n",mm->file_name);break;
			case BATCH_ENCODED : printf(">>> BATCH_ENCODED %s \n",mm->file_name);break;
			default: printf(">>> UNKNOWN EVENT");break;
			}
			isol->recycle_SMS(mm);
			mm=isol->getNames();
		}
		Sleep(10000);
	}while(true);
	
	return 0;
 }
