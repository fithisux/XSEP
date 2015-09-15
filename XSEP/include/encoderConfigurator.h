#ifndef ENCODER_CONFIGURATOR
#define ENCODER_CONFIGURATOR

#include "configuratorIsolator.h"
#include "XSEP.h"

class DLLIMPORT encoderConfigurator
{
public:
	
	encoder_conf_t *encoder_configuration;
	int time_offset;
	DLLEXPORT encoderConfigurator(const char *fname);
	DLLEXPORT ~encoderConfigurator();	
};

#endif
