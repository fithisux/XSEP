// This is the main project file for VC++ application project 
// generated using an Application Wizard.

#include "frameGrabber.h"

//The main Example
int main(int argc,char **argv)
{
	separ_device_camera_conf_t * cam_conf = new separ_device_camera_conf_t;
	separ_device_sequence_conf_t * seq_conf = new separ_device_sequence_conf_t;
	cvNamedWindow("Fithis",1); //make a windows

	separ_timeserver_conf_t *time_conf= new separ_timeserver_conf_t;

	time_conf->address="127.0.0.1";
	time_conf->port=8083;
	time_conf->logfile="c:/hello.log";

	if(argc !=4 )
	{
		printf("Usage : AxisCommunication IP PORT [save/nosave] \n");
		exit(0);
	}

	cam_conf->address=argv[1];
	cam_conf->port=atoi(argv[2]);

	bool mustSave=false;

	if(!strcmp("save",argv[3])) mustSave=true;
	else if(!strcmp("nosave",argv[3])) mustSave=false;
	else
	{
		printf("You must put only save/nosave option\n");
		exit(0);
	}

	cam_conf->format="MJPG";
	cam_conf->fps=25;
	cam_conf->duration=0;
	cam_conf->compression=-1;

	seq_conf->image_from=0;
	seq_conf->image_to=1000;
	seq_conf->image_length_prefix=0;
	seq_conf->image_prefix="c:\\video1\\image";
	seq_conf->image_type="jpg";

	frameGrabber *agrabber=new frameGrabber(cam_conf);	

	frame_description *aframe;

	int i=0;
	char ss[100];
	while(aframe=agrabber->getFrame())
	{
		//printf("Id %s\n",aframe->getTimeStamp());
		if(mustSave)
		{
			sprintf(ss,"image%d.jpg",i++);//make the filename
			cvSaveImage(ss,(IplImage *) aframe->getImage());//and save it
		}
		cvShowImage("Fithis",(IplImage *) aframe->getImage());//print it to the window		
		cvWaitKey(10);
	}
	delete agrabber;
	cvDestroyWindow("Fithis");
	return 0;
}