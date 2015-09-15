#include "confParametersXMLOps.h"

#include "confParametersDefs.h"


#include "cxtypes.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"


//Region Level
separ_regionlevel_conf_t *create_regionlevel_conf(xmlNode* a_node)
{
    xmlNodePtr cur_node = NULL;
	int check=2;

	printf("Checking for regionlevel \n");
	if(!checkIfActive(a_node)) return NULL;
	separ_regionlevel_conf_t * x = new separ_regionlevel_conf_t;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{        
		if(!strcmp( (const char *) cur_node->name,"colourthres"))
		{
			check--;
			x->colourthres=utilities_get_integer(cur_node);
		}
		if(!strcmp( (const char *) cur_node->name,"recoverarea"))
		{
			check--;
			x->recover_area=utilities_get_integer(cur_node);
		}				
    }
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//Region Level
separ_facelevel_conf_t *create_facelevel_conf(xmlNode* a_node)
{
    xmlNodePtr cur_node = NULL;
	int check=2;

	printf("Checking for facelevel \n");
	if(!checkIfActive(a_node)) return NULL;
	separ_facelevel_conf_t * x = new separ_facelevel_conf_t;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{        
		if(!strcmp( (const char *) cur_node->name,"numbins"))
		{
			check--;
			x->numbins=utilities_get_integer(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"intensity"))
		{
			check--;
			x->intensity=utilities_get_integer(cur_node);
		}		
    }
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//WINDOWS
separ_windows_conf_t * create_separ_windows_conf(xmlNode* a_node)
{
    xmlNodePtr cur_node = NULL;
	int check=6;	
	separ_windows_conf_t *x=new separ_windows_conf_t;

	printf("Checking for windows \n");
    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{
		if(!strcmp( (const char *) cur_node->name,"facedetection"))
		{
			check--;			
			x->facedetection=utilities_get_bool(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"pixellevel"))
		{
			check--;
			x->pixellevel=utilities_get_bool(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"playback"))
		{
			check--;
			x->playback=utilities_get_bool(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"regionlevel"))
		{
			check--;
			x->regionlevel=utilities_get_bool(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"faceestimation"))
		{
			check--;
			x->regionlevel=utilities_get_bool(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"savemask"))
		{
			check--;
			x->savemask=utilities_get_bool(cur_node);
		}
    }
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//Extra DSP
separ_extradsp_conf_t * create_separ_extradsp_conf(xmlNode* a_node)
{
	xmlNodePtr cur_node = NULL,tempNode;
	int check=4,tempcheck=0;
	separ_extradsp_conf_t *x = new separ_extradsp_conf_t;

	printf("Checking for extradsp \n");

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{
    	if(!strcmp( (const char *) cur_node->name,"deInterlace"))
		{
			check--;
			x->deInterlace=utilities_get_integer(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"framedelay"))
		{
			check--;
			x->framedelay=utilities_get_integer(cur_node);
		}
		else   	if( ! strcmp((const char *) cur_node->name,"block_size"))
        {
        	check--;
			x->block_size = utilities_get_integer(cur_node);
        }
		else if(!strcmp( (const char *) cur_node->name,"segmentation"))
		{
			check--;
			tempcheck=2;
			for(tempNode=cur_node->children;tempNode;tempNode=tempNode->next)
			{
				if(!strcmp( (const char *) tempNode->name,"segarea"))
				{
					tempcheck--;
					x->segmentation_area=utilities_get_integer(tempNode);
				}
				else if(!strcmp( (const char *) tempNode->name,"segthresh"))
				{
					tempcheck--;
					x->segmentation_threshold=utilities_get_integer(tempNode);
				}			
			}			
		}
    }
    check+=tempcheck;
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//NTUA model
NTUAModelParams *create_NTUAModelParams(xmlNode* a_node)
{
	printf("start ntua_model \n");
	xmlNode *cur_node = NULL;
	NTUAModelParams *x=new NTUAModelParams;	
	if(!checkIfActive(a_node)) return NULL;
	int check=2;
    for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        	if( ! strcmp((const char *) cur_node->name,"pastN"))
        	{
        		x->pastN = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"stableK"))
        	{
        		x->stableK = utilities_get_integer(cur_node);
        		check--;
        	}        					        	        
    }
    PRINT_ON_SANITY_FAILURE(check);
	return x;	
}

//DKOSMO model
DKOSMOModelParams *create_DKOSMOModelParams(xmlNode* a_node)
{
	printf("start dkosmo_model \n");
	xmlNode *cur_node = NULL;	
	int check=2;
	if(!checkIfActive(a_node)) return NULL;	
	DKOSMOModelParams *x=new DKOSMOModelParams;

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        	if( ! strcmp((const char *) cur_node->name,"theN"))
        	{
        		x->theN = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"theK"))
        	{
        		x->theK = utilities_get_integer(cur_node);
        		check--;
        	}        	
     }
    
	PRINT_ON_SANITY_FAILURE(check);
	return x;	           
}

//SOM model
SOMModelParams *create_SOMModelParams(xmlNode* a_node)
{
	printf("start dkosmo_model \n");
	xmlNode *cur_node = NULL;
	if(!checkIfActive(a_node)) return NULL;
	int check=5;
	SOMModelParams *x=new SOMModelParams;
	
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        	if( ! strcmp((const char *) cur_node->name,"generating_threshold"))
        	{
        		x->generating_threshold = utilities_get_double(cur_node);
        		check--;
        	}
			else   	if( ! strcmp((const char *) cur_node->name,"fg_class_threshold"))
        	{
        		x->fg_class_threshold = utilities_get_integer(cur_node);
        		check--;
        	}
			else if( ! strcmp((const char *) cur_node->name,"the_A"))
        	{
        		x->the_A = utilities_get_double(cur_node);
        		check--;
        	}
			else if( ! strcmp((const char *) cur_node->name,"the_B"))
        	{
        		x->the_B = utilities_get_double(cur_node);
        		check--;
        	}						
			else   	if( ! strcmp((const char *) cur_node->name,"rarity_threshold"))
        	{
        		x->rarity_threshold = utilities_get_integer(cur_node);
        		check--;
        	}        	        	      			        	        
    }
    
	PRINT_ON_SANITY_FAILURE(check);
	return x;	           
}

//SOM model
COMModelParams *create_COMModelParams(xmlNode* a_node)
{
	printf("start dkosmo_model \n");
	xmlNode *cur_node = NULL;
	if(!checkIfActive(a_node)) return NULL;
	int check=3;
	COMModelParams *x=new COMModelParams;
	
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        	if( ! strcmp((const char *) cur_node->name,"training_cycles"))
        	{
				x->training_cycles = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"force_threshold"))
        	{
        		x->force_threshold = utilities_get_double(cur_node);
        		check--;
        	}
			else   	if( ! strcmp((const char *) cur_node->name,"rarity_threshold"))
        	{
        		x->rarity_threshold = utilities_get_integer(cur_node);
        		check--;
        	}        	        			        	        
    }
    
	PRINT_ON_SANITY_FAILURE(check);
	return x;	           
}

//Bayes model
void* create_BAYESModelParams(xmlNode * a_node)
{
	printf("start bayes_model \n");
	if(!checkIfActive(a_node)) return NULL;	
	CvFGDStatModelParams *x=new CvFGDStatModelParams;	
	xmlNode *cur_node = NULL;
	int check=11;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
        	if( ! strcmp((const char *) cur_node->name,"Lc"))
        	{
        		x->Lc = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"Lcc"))
        	{
        		x->Lcc = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"N1c"))
        	{
        		x->N1c = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"N2c"))
        	{
        		x->N2c = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"N1cc"))
        	{
        		x->N1cc = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"N2cc"))
        	{
        		x->N2cc = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"alpha1"))
        	{
        		x->alpha1 = utilities_get_double(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"alpha2"))
        	{
        		x->alpha2 = utilities_get_double(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"alpha3"))
        	{
        		x->alpha3 = utilities_get_double(cur_node);
        		check--;
        	}        	
        	else   	if( ! strcmp((const char *) cur_node->name,"delta"))
        	{
        		x->delta = utilities_get_double(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"T"))
        	{
        		x->T = utilities_get_double(cur_node);
        		check--;
        	}        				        	        	
        }
    }
    
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//mog model
void* create_MOGModelParams(xmlNode * a_node)
{
	printf("start fill_mog_model \n");
	if(!checkIfActive(a_node)) return NULL;
	CvGaussBGStatModelParams* x=new CvGaussBGStatModelParams;	
	xmlNode *cur_node = NULL;
	int check=6;
	bool active=true;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
    {
        	if( ! strcmp((const char *) cur_node->name,"win_size"))
        	{
        		x->win_size = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"n_gauss"))
        	{
        		x->n_gauss =  utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"bg_threshold"))
        	{
        		x->bg_threshold = utilities_get_double(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"std_threshold"))
        	{
        		x->std_threshold = utilities_get_double(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"weight_init"))
        	{
        		x->weight_init = utilities_get_double(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"variance_init"))
        	{
        		x->variance_init = utilities_get_double(cur_node);
        		check--;
        	}        			        	
    }

	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//Pixellevel params
separ_pixellevel_params_t* create_pixellevel_params(xmlNode* a_node)
{
	printf("start fill_pixellevel_conf \n");	
	xmlNode *cur_node = NULL;
	int check=7;
	separ_pixellevel_params_t *x=new separ_pixellevel_params_t;

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
        	if( ! strcmp((const char *) cur_node->name,"skip"))
        	{
        		x->skip = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"minArea"))
        	{
        		x->minArea = utilities_get_integer(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"mustErode"))
        	{
        		x->mustErode= utilities_get_bool(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"filterMotion"))
        	{
        		x->filterMotion= utilities_get_bool(cur_node);
        		check--;
        	}        	
        	else if( ! strcmp((const char *) cur_node->name,"diffthreshold"))
        	{
        		x->diffthreshold = utilities_get_double(cur_node);
        		check--;
        	}        	
        	else   	if( ! strcmp((const char *) cur_node->name,"use_hsv"))
        	{
				x->use_hsv= utilities_get_bool(cur_node);
        		check--;
        	}
        	else   	if( ! strcmp((const char *) cur_node->name,"weights"))
        	{
				utilities_get_scalar(cur_node,x->weights);
				check--;
        	}        	        	
        }		
	}
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}


//FaceDetection

separ_facedetection_conf_t *create_facedetection_conf(xmlNode* a_node)
{
    xmlNodePtr cur_node = NULL;
	int check=6;

	printf("Checking for facedetection \n");
	separ_facedetection_conf_t *x=create_face_detection_conf();
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{
		if(!strcmp( (const char *) cur_node->name,"cascade"))
		{
			check--;
			x->cascade = utilities_get_string(cur_node);
		}
		else if( ! strcmp((const char *) cur_node->name,"facewidth"))        	
		{				
			x->facewidth = utilities_get_integer(cur_node);        		
			check--;        	
		}			
		else   	if( ! strcmp((const char *) cur_node->name,"faceheight"))        	
		{			
			x->faceheight = utilities_get_integer(cur_node);        	
			check--;        
		}
		else   	if( ! strcmp((const char *) cur_node->name,"uphue"))        	
		{			
			x->uphue = utilities_get_integer(cur_node);        	
			check--;        
		}
		else   	if( ! strcmp((const char *) cur_node->name,"downhue"))        	
		{			
			x->downhue = utilities_get_integer(cur_node);        	
			check--;        
		}
		else   	if( ! strcmp((const char *) cur_node->name,"huepercentage"))        	
		{			
			x->huepercentage = utilities_get_integer(cur_node);        	
			check--;        
		}
    }
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}
//Camera

DLLEXPORT separ_device_camera_conf_t * create_device_camera_conf(xmlNode* a_node)
{
	printf("Checking for camera \n");
	if(!checkIfActive(a_node)) return NULL;
	xmlNodePtr cur_node = NULL;
	int check=4;
	separ_device_camera_conf_t *x = create_device_camera_conf();

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{       
		if(!strcmp( (const char *) cur_node->name,"address"))
		{
			check--;
			x->address = utilities_get_string(cur_node);
		}
		if(!strcmp( (const char *) cur_node->name,"port"))
		{
			check--;
			x->port = utilities_get_integer(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"format"))
		{
			check--;
			x->format = utilities_get_string(cur_node);
		}			
		else if(!strcmp( (const char *) cur_node->name,"duration"))
		{
			check--;
			x->duration= utilities_get_integer(cur_node);
		}					
    }

	PRINT_ON_SANITY_FAILURE(check);
	return x;
}

//Sequence

separ_device_sequence_conf_t *create_device_sequence_conf(xmlNode* a_node)
{
	printf("Checking for sequence \n");
	if(!checkIfActive(a_node)) return NULL;
	xmlNodePtr cur_node = NULL;
	int check=5;
	separ_device_sequence_conf_t *x=create_device_sequence_conf();

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{     		
		if(!strcmp( (const char *) cur_node->name,"image_prefix"))
		{
			check--;
			x->image_prefix = utilities_get_string(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"image_type"))
		{
			check--;
			x->image_type = utilities_get_string(cur_node);
		}			
		else if(!strcmp( (const char *) cur_node->name,"image_from"))
		{
			check--;
			x->image_from=utilities_get_integer(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"image_to"))
		{
			check--;
			x->image_to=utilities_get_integer(cur_node);
		}
		else if(!strcmp( (const char *) cur_node->name,"image_length_prefix"))
		{
			check--;
			x->image_length_prefix=utilities_get_integer(cur_node);
		}												
    }

	PRINT_ON_SANITY_FAILURE(check);
	return x;
}


//AVI

separ_device_avi_conf_t * create_device_avi_conf(xmlNode* a_node)
{
	printf("Checking for avi \n");
	if(!checkIfActive(a_node)) return NULL;
	xmlNodePtr cur_node = NULL;
	int check=1;
	separ_device_avi_conf_t *x=create_device_avi_conf();
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{
		if(!strcmp( (const char *) cur_node->name,"file"))
		{
			check--;
			x->file = utilities_get_string(cur_node);
		}
    }

	PRINT_ON_SANITY_FAILURE(check);
	return x;
}


//DEVICE



separ_device_conf_t * separ_device_conf(xmlNode * a_node)
{
	separ_device_conf_t *x=create_device_conf();
	xmlNode *cur_node = NULL;
	char *val;
	int check=1;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
	{
		if(check && !strcmp((const char *) cur_node->name,"device"))
        {
        		val = utilities_get_string(cur_node);
        		if( ! strcmp(val,"sequence"))
        		{
        			if(x->sequence_conf=create_device_sequence_conf(cur_node))
					{
						check=0;
						printf("found a sequence \n");
					}
        		}
        		else if( ! strcmp(val,"avi"))
        		{
        			if(x->avi_conf=create_device_avi_conf(cur_node))
					{
						check=0;
						printf("found an avi \n");
					}
        		}
        		else if( ! strcmp(val,"camera"))
        		{
					if(x->camera_conf=create_device_camera_conf(cur_node))
					{
						check=0;
                        printf("found a camera \n");
					}
        		}        		
        		delete[] val;
        }   
	}    

	PRINT_ON_SANITY_FAILURE(check);
	return x;
}


//Time server


DLLEXPORT separ_timeserver_conf_t * create_timeserver_conf(xmlNode * a_node)
{
	printf("Checking for timeserver \n");
	if(!checkIfActive(a_node)) return NULL;
	separ_timeserver_conf_t *x=new separ_timeserver_conf_t;

	xmlNode *cur_node = NULL;
	int check=3;

	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
        	if( ! strcmp((const char *) cur_node->name,"address"))
        	{
        		x->address = utilities_get_string(cur_node);
        		check--;
        	}
        	else if( ! strcmp((const char *) cur_node->name,"logfile"))
        	{
        		x->logfile = utilities_get_string(cur_node);
        		check--;
        	}        	
        	else   	if( ! strcmp((const char *) cur_node->name,"port"))
        	{
        		x->port= utilities_get_integer(cur_node);
        		check--;
        	}        	        	      	
		}
	}
	
	PRINT_ON_SANITY_FAILURE(check);
	return x;	   
}


//Pixel level


separ_pixellevel_conf_t * create_pixellevel_conf(xmlNode * a_node)
{	
	xmlNode *cur_node = NULL,*algo_node=NULL;
	char *val;
	int check=1;	
	separ_pixellevel_conf_t *x=create_pixellevel_conf();
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
	{
		if( ! strcmp((const char *) cur_node->name,"params"))
		{
			x->params=create_pixellevel_params(cur_node);
			if(!x->params)
			{
				check=1;				
				break;
			}
		}
		else if(check && !strcmp((const char *) cur_node->name,"listOfAlgorithms"))
		{
			for (algo_node = cur_node->children; algo_node; algo_node = algo_node->next)
			{				
				if(check && !strcmp((const char *) algo_node->name,"algorithm"))
				{
        			val = utilities_get_string(algo_node);
        			if( ! strcmp(val,"NTUAPredictor"))
        			{					
        				if(x->ntua_model=create_NTUAModelParams(algo_node))
						{
							check=0;
							printf("found ntua model \n"); 							
						}
        			}				
        			else if( ! strcmp(val,"BAYESPredictor"))
        			{					
						if(x->bayes_model = create_BAYESModelParams(algo_node))
						{
							check=0;
							printf("found bayes model \n");	       				        			   			
						}
        			}
        			else if( ! strcmp(val,"MOGPredictor"))
        			{        			
        				if(x->mog_model = create_MOGModelParams(algo_node))
						{
							check=0;
							printf("found mog model \n");    				
						}
        			}
					else if( ! strcmp(val,"DKOSMOPredictor"))
        			{        			
						if(x->dkosmo_model = create_DKOSMOModelParams(algo_node))
						{
							check=0;
							printf("found dkosmo model \n");    				
						}
        			}        		
        			else if( ! strcmp(val,"SOMPredictor"))
        			{        			
						if(x->som_model = create_SOMModelParams(algo_node))
						{
							check=0;
							printf("found som model \n");	       				        			
						}
        			}
					else if( ! strcmp(val,"COMPredictor"))
        			{        			
						if(x->com_model = create_COMModelParams(algo_node))
						{
							check=0;
							printf("found com model \n");	       				        			
						}
        			}     
        			delete[] val;
				}
			}
        }   
	}     	
	
	PRINT_ON_SANITY_FAILURE(check);
	
	return x;
}

encoder_params_t * create_encoder_params(xmlNode* a_node)
{
	printf("processing encoder params\n");
	xmlNode *cur_node = NULL;
	int check=6;	
	encoder_params_t *x=create_encoder_params();
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
	{
		if( ! strcmp((const char *) cur_node->name,"video_path"))
		{
			x->video_path=utilities_get_string(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"spanish_port"))
		{
			x->spanish_port=utilities_get_integer(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"video_save"))
		{
			x->video_save=utilities_get_bool(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"austrian_port"))
		{
			x->austrian_port=utilities_get_integer(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"encoder_number"))
		{
			x->encoder_number=utilities_get_integer(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"batch_size"))
		{
			x->batch_size=utilities_get_integer(cur_node);
			check--;
		}
	}     	
	
	PRINT_ON_SANITY_FAILURE(check);	
	return x;
}

encoder_conf_t * create_encoder_conf(xmlNode* a_node)
{
	xmlNode *cur_node = NULL;	
	int check=3;	
	encoder_conf_t *x=create_encoder_conf();
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) 
	{
		if( ! strcmp((const char *) cur_node->name,"device"))
		{
			x->camera=create_device_camera_conf(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"params"))
		{
			x->params=create_encoder_params(cur_node);
			check--;
		}
		else if( ! strcmp((const char *) cur_node->name,"timeserver"))
		{
			x->timeserver_conf=create_timeserver_conf(cur_node);
			check--;
		}
	}
 
	PRINT_ON_SANITY_FAILURE(check);	
	return x;
}

//Region Level
separ_shadow_conf_t *create_shadow_conf(xmlNode* a_node)
{
    xmlNodePtr cur_node = NULL;
	int check=6;

	printf("Checking for regionlevel \n");
	if(!checkIfActive(a_node)) return NULL;
	separ_shadow_conf_t * x = new separ_shadow_conf_t;
	for (cur_node = a_node->children; cur_node; cur_node = cur_node->next)
	{        
		if(!strcmp( (const char *) cur_node->name,"rS"))
		{
			check--;
			x->rS=utilities_get_integer(cur_node);
		}
		if(!strcmp( (const char *) cur_node->name,"rH"))
		{
			check--;
			x->rH=utilities_get_integer(cur_node);
		}
		if(!strcmp( (const char *) cur_node->name,"maxArea"))
		{
			check--;
			x->maxArea=utilities_get_integer(cur_node);
		}		
		else if(!strcmp( (const char *) cur_node->name,"filterrun"))
		{
			check--;
			x->filterrun=utilities_get_integer(cur_node);
		}
		else   	if( ! strcmp((const char *) cur_node->name,"alpha"))
        {
        	x->alpha = utilities_get_double(cur_node);
        	check--;
        }
		else   	if( ! strcmp((const char *) cur_node->name,"beta"))
        {
        	x->beta = utilities_get_double(cur_node);
        	check--;
        }		
    }
	PRINT_ON_SANITY_FAILURE(check);
	return x;
}
