#ifndef camera_module_h
#define camera_module_h
#include "cxcore.h"
#include "cv.h"
#include "highgui.h"
#include "Magick++/Blob.h"
#include "Magick++/Image.h"
#include "Magick++/Pixels.h"
#include "axisTransaction.h"
#include "frameUtilities.h"
#include <string>
using namespace std;
typedef enum _camera_support_format {_MJPG_,_JPG_,_BMP_,_INVALID_} camera_support_format;

//#define CAMERA_LOG
class _declspec(dllexport) cameraModule
{
private :
	int duration;
	int frame_rate;
	CvSize resolution;
	int compression;
	frame_description *current_frame;
	camera_support_format format;
	axisTransaction* trans;
	bool is_some_stream;
	bool first_frame;
	int frame_number;
	bool started;
	bool forever;
	bool grabFrame();
	Magick::Blob *my_blob;
	Magick::Image *my_image;
	void reconstructFrame();
public :

	cameraModule(const char * address,int port);
	~cameraModule();
	void changeZoom(int x);
	void setFormat(camera_support_format s);
	void setResolution(CvSize spec);
	void setCompression(int comp);
	void setDuration(int dur);
	void setFrameRate(int fr);
	bool sendRequest();
	void startCamera();
	frame_description* getFrame();
};


#endif
