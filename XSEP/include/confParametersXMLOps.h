#ifndef confParametersOps_h
#define confParametersOps_h

#include "XSEP.h"
#ifndef ERROR_INFO
#define ERROR_INFO __FILE__, __LINE__
#endif


#define PRINT_ON_SANITY_FAILURE(checkit) if(checkit >0) \
	{	\
		printf("XML misconfiguration %s %d \n",ERROR_INFO);\
		exit(0);\
	}\


#include "confXMLUtilities.h"
#include "confParametersDefs.h"
#include "configuratorIsolator.h"

separ_windows_conf_t * create_separ_windows_conf(xmlNode* a_node);
separ_extradsp_conf_t * create_separ_extradsp_conf(xmlNode* a_node);
NTUAModelParams *create_NTUAModelParams(xmlNode* a_node);
DKOSMOModelParams *create_DKOSMOModelParams(xmlNode* a_node);
SOMModelParams *create_SOMModelParams(xmlNode* a_node);
COMModelParams *create_COMModelParams(xmlNode* a_node);
void* create_BAYESModelParams(xmlNode * a_node);
void* create_MOGModelParams(xmlNode * a_node);
separ_pixellevel_params_t* create_pixellevel_params(xmlNode* a_node);
separ_pixellevel_conf_t * create_pixellevel_conf(xmlNode * a_node);
separ_regionlevel_conf_t *create_regionlevel_conf(xmlNode* a_node);
separ_facelevel_conf_t *create_facelevel_conf(xmlNode* a_node);
separ_facedetection_conf_t *create_facedetection_conf(xmlNode* a_node);
DLLEXPORT separ_device_camera_conf_t *  create_device_camera_conf(xmlNode* a_node);
separ_device_sequence_conf_t *create_device_sequence_conf(xmlNode* a_node);
separ_device_avi_conf_t * create_device_avi_conf(xmlNode* a_node);
separ_device_conf_t * separ_device_conf(xmlNode * a_node);
DLLEXPORT separ_timeserver_conf_t *  create_timeserver_conf(xmlNode * a_node);
DLLEXPORT void  destroy_timeserver_conf(separ_timeserver_conf_t **x);
encoder_params_t * create_encoder_params(xmlNode* a_node);
encoder_conf_t * create_encoder_conf(xmlNode* a_node);
separ_shadow_conf_t *create_shadow_conf(xmlNode* a_node);
#endif
