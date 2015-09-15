#include "confParameters.h"
#include "confParametersXMLOps.h"

confParameters::confParameters(const char *fname)
{
	xmlDocPtr doc;
	LIBXML_TEST_VERSION;
	this->conf_extradsp=NULL;
	this->conf_facedetection=NULL;
	this->conf_pixellevel=NULL;
	this->conf_regionlevel=NULL;
	this->conf_windows=NULL;
	this->conf_timeserver=NULL;
	this->conf_device=NULL;
    doc = xmlReadFile(fname, NULL, 0);
    if (doc == NULL) throw std::string("Could not open file")+std::string(fname);
	printf("parsing doc \n");	
	xmlNodePtr root_element  = xmlDocGetRootElement(doc);
	printf("Root Element, name: %s\n", root_element->name);
	xmlNodePtr cur_node = NULL;

	for (cur_node = root_element->children; cur_node; cur_node = cur_node->next)
	{
        	if(!strcmp( (const char *) cur_node->name,"windows"))
			{				
				this->conf_windows = create_separ_windows_conf(cur_node);
			}
			else if(!strcmp( (const char *) cur_node->name,"extradsp"))
			{
				this->conf_extradsp= create_separ_extradsp_conf(cur_node);				
			}
			else if(!strcmp( (const char *) cur_node->name,"pixellevel"))
			{
				this->conf_pixellevel= create_pixellevel_conf(cur_node);				
			}
			else if(!strcmp( (const char *) cur_node->name,"regionlevel"))
			{
				this->conf_regionlevel = create_regionlevel_conf(cur_node);
			}
			else if(!strcmp( (const char *) cur_node->name,"facelevel"))
			{
				this->conf_facelevel = create_facelevel_conf(cur_node);
			}			
			else if(!strcmp( (const char *) cur_node->name,"facedetection"))
			{
				this->conf_facedetection = create_facedetection_conf(cur_node);
			}
			else if(!strcmp( (const char *) cur_node->name,"listOfDevice"))
			{
				this->conf_device=separ_device_conf(cur_node);
			}			
			else if(!strcmp( (const char *) cur_node->name,"timeserver"))
			{
				this->conf_timeserver=create_timeserver_conf(cur_node);				
			}
			else if(!strcmp( (const char *) cur_node->name,"shadow"))
			{
				this->conf_shadow=create_shadow_conf(cur_node);				
			}
	}	
	printf("done with separ\n");	
	xmlFreeDoc(doc);
	xmlCleanupParser();
	printf("done with parser\n");		
}

confParameters::~confParameters()
{	
	delete this->conf_windows;
	if(this->conf_extradsp) delete this->conf_extradsp;
	if(this->conf_regionlevel) delete this->conf_regionlevel;
	if(this->conf_facelevel) delete this->conf_facelevel;
	if(this->conf_shadow) delete this->conf_shadow;
	destroy_pixellevel_conf(&this->conf_pixellevel);
	destroy_face_detection_conf(&this->conf_facedetection);
	destroy_device_conf(&this->conf_device);
	if(this->conf_timeserver) destroy_timeserver_conf(&this->conf_timeserver);	
}






 








