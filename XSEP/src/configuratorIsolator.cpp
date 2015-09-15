#include "configuratorIsolator.h"
#include "confParametersXMLOps.h"

encoder_params_t *create_encoder_params()
{
	encoder_params_t * x = new encoder_params_t;
	x->video_path=NULL;
	return x;
}
void destroy_encoder_params(encoder_params_t **params)
{
	encoder_params_t * x=*params;
	if(x->video_path) delete[]x->video_path;
	*params=NULL;
}

encoder_conf_t *create_encoder_conf()
{
	encoder_conf_t *pt=new encoder_conf_t;
	pt->camera=NULL;
	pt->params=NULL;
	pt->timeserver_conf=NULL;
	return pt;
}

void destroy_encoder_conf(encoder_conf_t **conf)
{
	encoder_conf_t *pt=*conf;
	if(pt->camera) destroy_device_camera_conf(&pt->camera);
	if(pt->timeserver_conf) destroy_timeserver_conf(&pt->timeserver_conf);
	if(pt->params) destroy_encoder_params(&pt->params);
	delete pt;
	pt=NULL;
}


