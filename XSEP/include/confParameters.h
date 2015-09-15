#ifndef confParameters_h
#define confParameters_h

#include "confParametersDefs.h"
#include <string>
using namespace std;

/**
*
\class confParameters
\brief A class containing configuration parameters
and methods to obtain them from an xml file
*/
class  confParameters
{
public :
		separ_windows_conf_t *conf_windows;
		separ_extradsp_conf_t *conf_extradsp;
		separ_pixellevel_conf_t *conf_pixellevel;
		separ_regionlevel_conf_t *conf_regionlevel;
		separ_facelevel_conf_t *conf_facelevel;
		separ_device_conf_t *conf_device;
		separ_facedetection_conf_t *conf_facedetection;
		separ_timeserver_conf_t *conf_timeserver;
		separ_shadow_conf_t *conf_shadow;
public :

	/** \fn confParameters(const char *fname)
		\brief It loads an xml file with parameters
		\param fname The file to load
		\throw In case of file not found or parsing problems it throws a std::string
		exception with the suitable message describing the problem
	*/
	confParameters(const char *fname);
	~confParameters();
		
};

#endif
