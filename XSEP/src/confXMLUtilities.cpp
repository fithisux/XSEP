#include "confXMLUtilities.h"

DllExport int  utilities_get_integer(xmlNode * a_node)
{
	char *val1,*val2;
	int iValue;
	utilities_current_processing(a_node);
	val1 = (char *) xmlGetProp(a_node,(xmlChar *) "iValue");
	if(!val1)
	{
			printf("Not an iValue in %s. Aborting... \n",a_node->name);
			exit(0);		
	}		
	iValue = (int) strtol(val1,&val2,10);
	if(strlen(val2)>0)
	{
			printf("Not an integer in %s. Aborting... \n",a_node->name);
			exit(0);
	}
	xmlFree( (xmlChar*) val1);		
	return iValue;
}

DllExport double  utilities_get_double(xmlNode * a_node)
{
	char *val1;
	double dValue;
	utilities_current_processing(a_node);
	val1 = (char *) xmlGetProp(a_node,(xmlChar *) "fValue");
	if(!val1)
	{
			printf("Not an fValue in %s. Aborting... \n",a_node->name);
			exit(0);		
	}		
	dValue = atof(val1);	
	xmlFree( (xmlChar*) val1);
	return dValue;
}

DllExport char*  utilities_get_string(xmlNode * a_node)
{
	char *val1,*sValue;
	utilities_current_processing(a_node);		
	val1 = (char *) xmlGetProp(a_node,(xmlChar *) "sValue");
	if(!val1)
	{
			printf("Not an sValue in %s. Aborting... \n",a_node->name);
			exit(0);		
	}
	sValue= new char [strlen(val1)+1];
	strcpy(sValue,val1);
	xmlFree( (xmlChar*) val1);
	return sValue;
}

DllExport bool  utilities_get_bool(xmlNode * a_node)
{
	char *val1;
	bool bValue;
	utilities_current_processing(a_node);
	val1 = (char *) xmlGetProp(a_node,(xmlChar *) "bValue");
	if(!val1)
	{
			printf("Not an bValue in %s. Aborting... \n",a_node->name);
			exit(0);		
	}
	if( !strcmp(val1,"true") ) bValue=true;
	else if( !strcmp(val1,"false") ) bValue=false;
	else	
	{
			printf("Not a bool in %s. Aborting... \n",a_node->name);
			exit(0);
	}
	xmlFree( (xmlChar*) val1);
	return bValue;
}

DllExport void  utilities_get_scalar(xmlNode * a_node,double *vals)
{
	xmlNodePtr cur_node = NULL;
	int check=4;

	printf("Checking for weights \n");
	
    for (cur_node = a_node->children;cur_node;cur_node = cur_node->next)
	{        
		if(!strcmp( (const char *) cur_node->name,"w0"))
		{
			check--;
			vals[0]=utilities_get_double(cur_node);
		}
		if(!strcmp( (const char *) cur_node->name,"w1"))
		{
			check--;
			vals[1]=utilities_get_double(cur_node);
		}		
		else if(!strcmp( (const char *) cur_node->name,"w2"))
		{
			check--;
			vals[2]=utilities_get_double(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"w3"))
		{
			check--;
			vals[3]=utilities_get_double(cur_node);
		}		
    }
	if(check)
	{
		printf("Incorrect args in utilities_get_scalar \n");
	}
}

DllExport void  utilities_current_processing(xmlNode * a_node){printf("Current processing %s \n",a_node->name);}

DllExport bool  checkIfActive(xmlNode * a_node)
{
	bool returnIt;
	utilities_current_processing(a_node);
	xmlChar *val=xmlGetProp(a_node,(const xmlChar *) "state");
	if(!val)
	{
		printf("Cannot determine if active %s %d \n",__FILE__,__LINE__);
		exit(0);
	}
	returnIt=strcmp("false",(const char *)val);
	xmlFree(val);	
	return returnIt;
}
