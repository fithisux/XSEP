#ifndef COM_PREDICTOR_H
#define COM_PREDICTOR_H
#include "PixelLevel.h"
#include "centerOfMassCluster.h"
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool.hpp>
#include <stdio.h>
#include <iostream>
using namespace std;

//#define COMP_LOG
/**
*
\class FrameWienerPredictor
\brief A predictor class

It uses the global configuration class.
It constructs a dual predictor of past frames and 
past predictions. The past frames and past predictions 
have the same length.
*/

class  COMPredictor : public PixelLevel
{
	boost::pool<>* weight_pool;
public :

	int training_time;
	int timer;
	IplImage *new_frame;
	int numOfClusters;
	center_of_mass_cluster_conf_t com_conf;
	center_of_mass_array_t** background_features;
	COMModelParams *com_model;
	int ready;
	bool firstTime;
public :

/**
It holds the prediction based on params->past_length (confParameters::past_length)
last predictions (FrameWienerPrediction::prediction)
*/


public :
/*! \fn FrameWienerPredictor(confParameters* params)
 *  \brief A constructor.
 *  \param params a class that contains configuration parameters (confParameters).
 */
	COMPredictor(separ_pixellevel_conf_t *params);

/*! \fn bool isReady()
 *  \brief Indicates if the predictor has enough frames in order to discriminate
 *	foreground from background at the pixel level.It actually takes 2*params->past_length
 *	(confParameters::past_length) applications of the FrameWienerPredictor::updateWithFrame
 *  in order to be ready for predictions. See the paper on the WallFlower algorithm
 *  \return A bool to indicate that the predictor is ready
 */
	bool isReady();

/*! \fn void updateWithFrame(IplImage* frame)
 *  \brief It pushes a frame in the predictor. In the case the predictor is ready
 * (FrameWienerPredictor::isReady()), it triggers a normal prediction 
 * (FrameWienerPredictor::prediction) and an extra_prediction 
 * (FrameWienerPredictor::extra_prediction).The foreground is computed and then the 
 * normal prediction is pushed in the list of past predictions. In the case when the predictor
 * is not ready there are two cases. If the number of past frames is lower to 
 * params->past_length (confParameters::past_length) 
 * it is pushed in the list of past frames. Otherwise it creates a prediction 
 * FrameWienerPredictor::prediction which pushes in the list of past_predictions and then
 * it pushes the frame to the list of past frames
 * \param frame is the frame discribed above
 * 
 */
 	void updateWithFrame(IplImage* frame);
	void predict();
	void computePrediction(){};
	void convertToVector();
	void mustTikTakToe();
	~COMPredictor();

private :
	void initializeAll(IplImage *frame);
};

#endif /*SOMPREDICTOR_*/
