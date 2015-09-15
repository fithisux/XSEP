#ifndef SOM_CONDENSATION_CORE_H
#define SOM_CONDENSATION_CORE_H

//#define SOM_LOG
#include "cxcore.h"
#include "bgUtilities.h"
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool.hpp>

typedef struct _som_temporal_weight_t
{
	CvScalar value;
	int fg_class_timeout;	
	int rarity_timeout;
	struct _som_temporal_weight_t *next,*prev;
}som_temporal_weight_t;

typedef struct
{		
	double generating_threshold;
	double joining_threshold;
	double irrelevance_threshold;
	int rarity_threshold;
	int fg_class_threshold;
	double the_A;
	double the_B;
	boost::pool<> *weight_pool;
}som_temporal_conf_t;

typedef struct
{
	som_temporal_conf_t *conf;
	int time;	
}som_temporal_timer_t;

typedef struct
{
	tri_info *howto_norm;
	int length;
	int x;
	int y;
	som_temporal_timer_t *som_timer;
	som_temporal_weight_t *weight_list;
}som_temporal_series_t;

som_temporal_timer_t * som_create_timer(som_temporal_conf_t *aconf);
void som_release_timer(som_temporal_timer_t **timer_ptr);
void som_increase_timer(som_temporal_timer_t *timer);

som_temporal_series_t * som_create_series(som_temporal_timer_t *atimer,tri_info *anorm,int x,int y);
void som_release_series(som_temporal_series_t **aseries_ptr);

som_temporal_weight_t *   som_find_neighbor(som_temporal_series_t *aseries,CvScalar *value);
void   som_delete_weight(som_temporal_series_t *aseries,som_temporal_weight_t * node);

void  som_create_weight(som_temporal_series_t *aseries,CvScalar *value);
void som_remove_stale_weights(som_temporal_series_t *aseries);
void som_merge_weights(som_temporal_series_t *aseries);

void som_adjust_weight(som_temporal_series_t *aseries,som_temporal_weight_t *aweight,CvScalar *value);
bool som_add_value(som_temporal_series_t *aseries,CvScalar *value);



#endif

