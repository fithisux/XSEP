#ifndef CONF_XML_UTILITIES_H
#define CONF_XML_UTILITIES_H

extern "C"
{
	#include <libxml/parser.h>
	#include <libxml/tree.h>
	#include "string.h"
}

#include "XSEP.h"

DllExport int utilities_get_integer(xmlNode * a_node);
DllExport double utilities_get_double(xmlNode * a_node);
DllExport char* utilities_get_string(xmlNode * a_node);
DllExport bool utilities_get_bool(xmlNode * a_node);
DllExport void utilities_current_processing(xmlNode * a_node);
DllExport void utilities_get_scalar(xmlNode * a_node,double *vals);
DllExport bool checkIfActive(xmlNode * a_node);
#endif
