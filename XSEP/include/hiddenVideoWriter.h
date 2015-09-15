#ifndef HIDDENVIDEOWRITER_H
#define HIDDENVIDEOWRITER_H
#include "frameUtilities.h"
class hiddenVideoWriter
{
	void *my_writer;
	char *file_name;
public :
	hiddenVideoWriter(const char *avi_name);
	void writeFrame2AVI(frame_description *el);
	~hiddenVideoWriter();
};

#endif

