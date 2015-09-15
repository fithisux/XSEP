#include "encoderConfigurator.h"
#include "confParametersXMLOps.h"
#include <string>
using namespace std;

encoderConfigurator::encoderConfigurator(const char *fname)
{
	LIBXML_TEST_VERSION;
	
	xmlDocPtr doc = xmlReadFile(fname, NULL, 0);
	if (doc == NULL) throw std::string("Could not open file")+std::string(fname);
	printf("parsing doc \n");	
	xmlNodePtr  root_element  = xmlDocGetRootElement(doc);
	printf("Root Element, name: %s\n", root_element->name);				
	this->encoder_configuration = create_encoder_conf(root_element);
	if(!this->encoder_configuration->camera) throw std::string("Could not find a camera.");
	xmlFreeDoc(doc);
	xmlCleanupParser();
	printf("done with parser\n");
	this->time_offset=0;
	printf("getting offset\n");				
}

encoderConfigurator::~encoderConfigurator()
{
	destroy_encoder_conf(&this->encoder_configuration);
}

