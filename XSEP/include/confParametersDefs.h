#ifndef confParametersDefs_h
#define confParametersDefs_h

#include "XSEP.h"

#ifndef ERROR_INFO
#define ERROR_INFO __FILE__, __LINE__
#endif


#define PRINT_ON_SANITY_FAILURE(checkit) if(checkit >0) \
	{	\
		printf("XML misconfiguration %s %d \n",ERROR_INFO);\
		exit(0);\
	}\

typedef struct _separ_windows_conf_t
{
	bool pixellevel;
	bool regionlevel;
	bool playback;
	bool facedetection;
	bool faceestimation;
	bool savemask;
} separ_windows_conf_t;

typedef struct _separ_extradsp_conf_t
{
	int segmentation_area;
	int segmentation_threshold;
	int deInterlace;
	int framedelay;
	int block_size;
} separ_extradsp_conf_t;


typedef struct _ntua_predictor_t
{
	int pastN;
	int stableK;
} NTUAModelParams;


typedef struct _dkosmo_predictor_t
{
	int theN;
	int theK;
} DKOSMOModelParams;


typedef struct _som_predictor_t
{
	double generating_threshold;
	double the_A;
	double the_B;
	int rarity_threshold;
	int fg_class_threshold;
} SOMModelParams;

typedef struct _com_predictor_t
{
	int training_cycles;
	double force_threshold;
	int rarity_threshold;
} COMModelParams;

typedef struct _separ_pixellevel_params_t
{
	bool use_hsv;
	int skip;
	bool mustErode;
	bool filterMotion;
	int minArea;
	double weights[4];
	double diffthreshold;	
} separ_pixellevel_params_t;

typedef struct _separ_pixellevel_conf_t
{
	separ_pixellevel_params_t* params;
	NTUAModelParams *ntua_model;
	DKOSMOModelParams *dkosmo_model;
	void *bayes_model;
	void* mog_model;
	SOMModelParams * som_model;
	COMModelParams * com_model;
} separ_pixellevel_conf_t;

DllExport separ_pixellevel_conf_t * create_pixellevel_conf();
DllExport void destroy_pixellevel_conf(separ_pixellevel_conf_t **x);

typedef struct _separ_regionlevel_conf_t
{
	int recover_area;
	double colourthres;
} separ_regionlevel_conf_t;

typedef struct _separ_facelevel_conf_t
{
	int numbins;
	int intensity;
}separ_facelevel_conf_t;

typedef struct _separ_facedetection_conf_t
{
	char * cascade;	
	int facewidth;
	int faceheight;
	int uphue;
	int downhue;
	int huepercentage;
} separ_facedetection_conf_t;

DllExport separ_facedetection_conf_t *   create_face_detection_conf();
DllExport void   destroy_face_detection_conf(separ_facedetection_conf_t **x);

typedef struct _separ_device_camera_conf_t
{
	char *address;
	int port;
	char *format;
	int duration;
	int fps;
	int compression;
} separ_device_camera_conf_t;

DllExport separ_device_camera_conf_t *  create_device_camera_conf();
DllExport void  destroy_device_camera_conf(separ_device_camera_conf_t **x);

typedef struct _separ_device_sequence_conf_t
{
	char *image_prefix;
	int image_from;
	int image_to;
	int image_length_prefix;
	char *image_type;
} separ_device_sequence_conf_t;

DllExport  separ_device_sequence_conf_t *   create_device_sequence_conf();
DllExport void   destroy_device_sequence_conf(separ_device_sequence_conf_t **x);

typedef struct _separ_device_avi_conf_t
{
	char *file;	
} separ_device_avi_conf_t;

DllExport  separ_device_avi_conf_t *   create_device_avi_conf();
DllExport void   destroy_device_avi_conf(separ_device_avi_conf_t **x);

typedef struct _separ_device_conf_t
{
	separ_device_avi_conf_t * avi_conf;
	separ_device_camera_conf_t* camera_conf;
	separ_device_sequence_conf_t *sequence_conf;
} separ_device_conf_t;

DllExport  separ_device_conf_t *   create_device_conf();
DllExport void   destroy_device_conf(separ_device_conf_t **x);

typedef struct _separ_timeserver_conf_t
{
	char *logfile;	
	char *address;
	int port;
} separ_timeserver_conf_t;

DllExport separ_timeserver_conf_t *  create_timeserver_conf();
DllExport void  destroy_timeserver_conf(separ_timeserver_conf_t **x);

typedef struct _separ_shadow_conf_t
{
	float alpha;	
	float beta;	
	int rS;	
	int rH;
    int filterrun;
	int maxArea;
} separ_shadow_conf_t;

#endif



