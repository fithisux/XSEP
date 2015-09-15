#include "confParametersDefs.h"


#include "cxtypes.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"

//FaceDetection
DllExport separ_facedetection_conf_t *   create_face_detection_conf()
{
	separ_facedetection_conf_t *x=new separ_facedetection_conf_t;
	x->cascade=NULL;
	return x;
}

DllExport void   destroy_face_detection_conf(separ_facedetection_conf_t **x)
{
	if((*x)->cascade) delete[] (*x)->cascade;
	delete (*x);
	*x=NULL;
}

//Camera

DllExport separ_device_camera_conf_t *  create_device_camera_conf()
{
	separ_device_camera_conf_t *x=new separ_device_camera_conf_t;
	x->address=x->format=NULL;
	return x;
}

DllExport void  destroy_device_camera_conf(separ_device_camera_conf_t **x)
{
	if((*x)->address) delete[] (*x)->address;
	if((*x)->format) delete[] (*x)->format;
	delete (*x);
	*x=NULL;
}


//Sequence
DllExport separ_device_sequence_conf_t *   create_device_sequence_conf()
{
	separ_device_sequence_conf_t *x=new separ_device_sequence_conf_t;
	x->image_prefix=x->image_type=NULL;
	return x;
}

DllExport void   destroy_device_sequence_conf(separ_device_sequence_conf_t **x)
{
	if((*x)->image_prefix) delete[] (*x)->image_prefix;
	if((*x)->image_type) delete[] (*x)->image_type;
	delete (*x);
	*x=NULL;
}


//AVI
DllExport separ_device_avi_conf_t *   create_device_avi_conf()
{
	separ_device_avi_conf_t *x=new separ_device_avi_conf_t;
	x->file=NULL;
	return x;
}

DllExport void   destroy_device_avi_conf(separ_device_avi_conf_t **x)
{
	if((*x)->file) delete[] (*x)->file;
	delete (*x);
	*x=NULL;
}


//DEVICE
DllExport separ_device_conf_t *   create_device_conf()
{
	separ_device_conf_t *x=new separ_device_conf_t;
	x->avi_conf=NULL;
	x->camera_conf=NULL;
	x->sequence_conf=NULL;
	return x;
}


DllExport void   destroy_device_conf(separ_device_conf_t **x)
{
	if((*x)->avi_conf) destroy_device_avi_conf(& (*x)->avi_conf);
	if((*x)->sequence_conf) destroy_device_sequence_conf(& (*x)->sequence_conf);
	if((*x)->camera_conf) destroy_device_camera_conf(& (*x)->camera_conf);
	delete (*x);
	(*x)=NULL;
}
//Time server
DllExport separ_timeserver_conf_t *  create_timeserver_conf()
{
	separ_timeserver_conf_t *x=new separ_timeserver_conf_t;
	x->logfile=x->address=NULL;
	return x;
}



DllExport void  destroy_timeserver_conf(separ_timeserver_conf_t **x)
{
	if((*x)->logfile) delete[] (*x)->logfile;
	if((*x)->address) delete[] (*x)->address;
	delete (*x);
	*x=NULL;
}

//Pixel level
DllExport separ_pixellevel_conf_t *   create_pixellevel_conf()
{
	separ_pixellevel_conf_t *x=new separ_pixellevel_conf_t;
	x->params=NULL;
	x->bayes_model=NULL;
	x->dkosmo_model=NULL;
	x->mog_model=NULL;
	x->ntua_model=NULL;
	x->som_model=NULL;
	x->com_model=NULL;
	return x;
}


DllExport void   destroy_pixellevel_conf(separ_pixellevel_conf_t **x)
{
	if((*x)->bayes_model) delete (*x)->bayes_model;
	if((*x)->dkosmo_model) delete (*x)->dkosmo_model;
	if((*x)->mog_model) delete (*x)->mog_model;
	if((*x)->ntua_model) delete (*x)->ntua_model;
	if((*x)->som_model) delete (*x)->som_model;
	if((*x)->com_model) delete (*x)->com_model;
	if((*x)->params) delete (*x)->params;
	if((*x)) delete (*x);
	*x=NULL;
}

