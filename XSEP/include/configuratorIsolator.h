#ifndef CONFIGURATOR_ISOLATOR
#define CONFIGURATOR_ISOLATOR
#include "confParametersDefs.h"
#include <stdio.h>

typedef struct _encoder_params_t
{
	char *video_path;
	bool video_save;
	int austrian_port,spanish_port;	
	int encoder_number,batch_size;		
}encoder_params_t;

encoder_params_t *create_encoder_params();
void destroy_encoder_params(encoder_params_t **params);


typedef struct _encoder_conf_t
{
	encoder_params_t *params;
	separ_device_camera_conf_t *camera;
	separ_timeserver_conf_t *timeserver_conf;
}encoder_conf_t;

encoder_conf_t *create_encoder_conf();
void destroy_encoder_conf(encoder_conf_t **conf);


#endif
