#ifndef CENTEROFMASSCLUSTER_H_
#define CENTEROFMASSCLUSTER_H_
#include "cxcore.h"
#include <boost/pool/poolfwd.hpp>
#include <boost/pool/pool.hpp>

typedef struct _center_of_mass_cluster_conf_t
{
	int rarity_threshold;
	double force_threshold;	
	boost::pool<> *com_pool;
} center_of_mass_cluster_conf_t;

typedef struct _center_of_mass_cluster_t
{
	center_of_mass_cluster_conf_t *conf;
	int timer;
	CvScalar center_of_mass;
	int mass;
	struct _center_of_mass_cluster_t* next;
} center_of_mass_cluster_t;

typedef struct _center_of_mass_array_t
{
	center_of_mass_cluster_conf_t *conf;
	center_of_mass_cluster_t * array;
	center_of_mass_cluster_t*last_cluster;
} center_of_mass_array_t;

center_of_mass_cluster_t * create_center_of_mass_cluster(center_of_mass_cluster_conf_t* aconf,CvScalar* some_value);
void modify_center_of_mass_cluster(center_of_mass_cluster_t *cluster,CvScalar *some_value);
bool has_expired_center_of_mass_cluster(center_of_mass_cluster_t *cluster);
double find_force_center_of_mass_cluster(center_of_mass_cluster_t *cluster,CvScalar *some_value);


center_of_mass_array_t * create_center_of_mass_array(center_of_mass_cluster_conf_t* aconf);
void destroy_center_of_mass_array(center_of_mass_array_t **com_arr);
void find_winner_center_of_mass_array(center_of_mass_array_t *com_arr,CvScalar *some_value);
bool adapt_center_of_mass_array(center_of_mass_array_t *com_arr,CvScalar *some_value);
void tiktaktoe_center_of_mass_array(center_of_mass_array_t *com_arr);
bool add_value_center_of_mass_array(center_of_mass_array_t *com_arr,CvScalar *some_value);


#endif /*CENTEROFMASSCLUSTER_H_*/
