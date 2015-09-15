#include "centerOfMassCluster.h"
#include <math.h>

center_of_mass_cluster_t * create_center_of_mass_cluster(center_of_mass_cluster_conf_t* aconf,CvScalar* some_value)
{	
	center_of_mass_cluster_t *x= (center_of_mass_cluster_t *) aconf->com_pool->malloc();
	x->next=NULL;
	x->conf=aconf;
	x->timer=x->conf->rarity_threshold;
	x->mass=1;
	for(int l=0;l<4;l++) x->center_of_mass.val[l]=some_value->val[l];
	return x; 
}

void modify_center_of_mass_cluster(center_of_mass_cluster_t *cluster,CvScalar *some_value)
{
	int temp=cluster->mass + 1;
	for(int l=0;l<4;l++)
	{
		cluster->center_of_mass.val[l]=some_value->val[l]+ cluster->mass * cluster->center_of_mass.val[l];
		cluster->center_of_mass.val[l] /= (temp);		
	}
	cluster->mass++;
	cluster->timer=cluster->conf->rarity_threshold;
}

bool has_expired_center_of_mass_cluster(center_of_mass_cluster_t *cluster)
{
	if(cluster->timer == 0) return true;
	else
	{
		cluster->timer--;
		return false;
	}
}

double find_force_center_of_mass_cluster(center_of_mass_cluster_t *cluster,CvScalar *some_value)
{
	double sq_dist;
	double x;
	
	sq_dist=0.001;
	for(int l=0;l<4;l++)
	{
		x = some_value->val[l]-cluster->center_of_mass.val[l];
		sq_dist += (x*x);
	}
	
	return ( cluster->mass / sq_dist);
}


center_of_mass_array_t * create_center_of_mass_array(center_of_mass_cluster_conf_t* aconf)
{	
	center_of_mass_array_t * x = new center_of_mass_array_t;
	x->conf=aconf;
	x->array=NULL;
	return x;
}

void destroy_center_of_mass_array(center_of_mass_array_t **com_arr)
{
	center_of_mass_cluster_t *cluster;
	center_of_mass_array_t * x=*com_arr;
	cluster=x->array;
	while(cluster)
	{
		x->array=x->array->next;
		delete cluster;
		cluster=x->array;
	}
	delete x;
	x=NULL;
}

void find_winner_center_of_mass_array(center_of_mass_array_t *com_arr,CvScalar *some_value)
{
	center_of_mass_cluster_t *cluster;
	double temp_force,max_force;
	com_arr->last_cluster=NULL;	
	cluster=com_arr->array;
	while(cluster)
	{
		temp_force=find_force_center_of_mass_cluster(cluster,some_value);
		if( (!com_arr->last_cluster) || (temp_force > max_force) )
		{
			com_arr->last_cluster=cluster;
			max_force=temp_force;
		} 
		cluster=cluster->next;		 
	}
	
	if((com_arr->last_cluster) && (max_force < com_arr->conf->force_threshold))
	{
		com_arr->last_cluster=NULL;
	} 
	
}

bool adapt_center_of_mass_array(center_of_mass_array_t *com_arr,CvScalar *some_value)
{
	center_of_mass_cluster_t *cluster;

	if(!com_arr->last_cluster)
	{
		cluster=create_center_of_mass_cluster(com_arr->conf,some_value);
		cluster->next=com_arr->array;
		com_arr->array=cluster;
		return true;
	}
	else
	{
		modify_center_of_mass_cluster(com_arr->last_cluster,some_value);	
		return false;		
	}
}

void tiktaktoe_center_of_mass_array(center_of_mass_array_t *com_arr)
{
	center_of_mass_cluster_t *prev,*temp,*cur;

	prev=NULL;
	cur=com_arr->array;
	
	while(cur)
	{

		if(has_expired_center_of_mass_cluster(cur))
		{
			if(!prev) com_arr->array=cur->next;
			else prev->next=cur->next;
			temp=cur;
			cur=cur->next;
			com_arr->conf->com_pool->free(temp);
		}
		else
		{
			prev=cur;
			cur=cur->next;
		}
	}
}

bool add_value_center_of_mass_array(center_of_mass_array_t *com_arr,CvScalar *some_value)
{
	find_winner_center_of_mass_array(com_arr,some_value);
	return adapt_center_of_mass_array(com_arr,some_value);
}
