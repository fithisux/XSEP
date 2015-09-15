#ifndef AXIS_TRANSACTION_H
#define AXIS_TRANSACTION_H
//#define AXIS_LOG
#define LIMIT_TOKEN_LENGTH 256

#include "frameUtilities.h"
/** class axisTransaction
	\brief Creates an object that manages the transaction to the axis camera

	This class is based upon the HawkNL library since it is low level to achieve high throughput
*/



class  axisTransaction
{
public :
/** \var headers
	\brief A pointer to a vector of string containing the headers returned by the
	specification of the HTTP 1.0 protocol.
*/
	
/** \var body
	\brief A char * buffer that contains the body of the response according to the
	specification of the HTTP 1.0 protocol.
*/
	
/** \var body_length
	\brief the length of the body. @see axisTransaction::body . It actually contains 
	the	Content-Length header data.
*/
	
/** \fn	axisTransaction(const char * address);
	\brief This is the constructor of the class and is initialized by a string containing
	the address of the camera server in the form

	e.g.  147.102.19.96:80
*/
	axisTransaction(const char * address,int port);
/** \fn	void sendRequest(char *mesg)
	\brief This method sends a request according to the developer reference of axis camera

	e.g.  "GET /bitmap/1/image.bmp HTTP/1.0 \r\n"
*/
	bool sendRequest(char *mesg);
/** \fn	void getResponse();
	\brief This method explicitely asks for a respond from the camera server according to the axis
	developer reference.

	In reality this function fills the @see AxisTransaction::body with 
	@see AxisTransaction::body_length data. 
*/

	
/** \fn	isAlive();
	\brief Queries the socket if it is alive. 
	
	  This a brute force attempt to query a socket. It can be used to see if a stream
	of MJPEG data is finished. However it can be done more reliably by counting frames
	until the framerate*duration images are received
*/
	//void changeZoom(int x);
	bool isAlive();
/** \fn	~axisTransaction();
	\brief The destructor of the class	
*/
	~axisTransaction();	
	bool safeGetResponse();
	long long first_time;
	long long second_time;
	int frame_counter;

	frame_description* getFrame();	
	void setFrame(frame_description*aframe);
private :		
	frame_description* frame;
    void *my_stream;
	int my_port;
	bool firstTime;	
	void count_fps();
	bool recoverTimeStamp(const char *ahead);
	void getResponse();	
};
#endif
