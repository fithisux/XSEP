#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 
/**
\struct bmpFileHeader
\brief A struct containing the header of a BMP file

	See http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
	for details
*/

typedef struct bmpFileHeaderStruc
{
	char  name[3];
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} bmpFileHeader;

/**
\struct bmpFileInfoHeader
\brief A struct containing the info header of a BMP file

	See http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
	for details
*/
typedef struct bmpInfoHeaderStruc
{
	unsigned int biSize;
	unsigned int biWidth;
	unsigned int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;	
}bmpInfoHeader;	

/**
	class bmpParser

	\brief This class parses bmp blobs. 
	
	  This class is not used for files but for BLOBs (Binary Large OBjects). If  you
	  want to use it to parse files, just move the file to blob and parse the blob instead.
	  Run Length Encoding in BMP files is not implemented as it is rarely used.
	  Use some other format for this purpose, like PNG or JPEG. The information is based on URL
	  http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html.
*/

class  bmpParser
{
private :
	unsigned char *the_pallette,*byte_buffer;
	int pallette_length,byte_buffer_length;
	void readHeader(unsigned char *buff);
	void readInfo(unsigned char *buff);
	int drawIt();
public:
	
/** \var header
	\brief Contains the header of the file in the form of a struct.

	Use it after a call to @see bmpParser::analyzeHeader. It is initialized to NULL
*/
	bmpFileHeader *header;
/** \var header
	\brief Contains the info part of the file in the form of a struct.

	Use it after a call to @see bmpParser::analyzeHeader. It is initialized to NULL
*/
	bmpInfoHeader *info;

	/** \var the_image
	\brief Contains the image int the form of a two-dimensional array.

	The image contains info->biHeight rows of length 3*info->biWidth bytes.
	The data are in the form RGB.
*/
	unsigned char** the_image;

	/** \fn bmpParser()
		\brief The constructor of the class. It allocates necessary memory
		for the internal structures

		The purpose of this constructor is to generalize the usage of the 
		parser. The parser can be used either per image or it can be used for
		multiple frames sharing the same header @see bmpParser::header and info
		@see bmParser::info
	*/

	bmpParser();

	/** \fn analyzeHeader(char *buffer);
		\brief Recover the header in easy to manipulte structures.

		The purpose of this method is to recover the necessary properties from
		the first 54 bytes of buffer.The results are stored in header @see bmpParser::header 
		and info @see bmParser::info. The buffer must contain at least 54 bytes.
	*/

	void analyzeHeader(char *buffer);

	/** \fn analyzeHeader(char *buffer);
		\brief Recover the image from the buffer after skipping the first 54 bytes.

		The purpose of this method is to recover the image from the buffer using the information
		stored in  header @see bmpParser::header and info @see bmParser::info. 
		The buffer must contain at least 54 bytes.
	*/
	void analyzeBuffer(char *buffer);

	/** \fn ~bmpParser()
		\brief The destructor of the class.
	*/
	~bmpParser();	
};

