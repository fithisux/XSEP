#ifndef MOGPREDICTOR_H_
#define MOGPREDICTOR_H_

#include "PixelLevel.h"
//#define MOG_LOG

class MOGPredictor : public PixelLevel
{
public :

	CvBGStatModel* bg_model;
	CvGaussBGStatModelParams* mog_model;
	int ready;

public :
/*! \fn FrameWienerPredictor(confParameters* params)
 *  \brief A constructor.
 *  \param params a class that contains configuration parameters (confParameters).
 */
	MOGPredictor(separ_pixellevel_conf_t *params);

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
	~MOGPredictor();

private :
	void computePrediction();	
};

#endif /*MOGPREDICTOR_H_*/
