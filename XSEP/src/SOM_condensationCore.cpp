#include "SOM_condensationCore.h"

som_temporal_series_t * som_create_series(som_temporal_timer_t *atimer,tri_info *anorm,int x,int y)
{
	som_temporal_series_t *temp_series= new som_temporal_series_t;
	temp_series->som_timer=atimer;	
	temp_series->length=0;
	temp_series->weight_list = NULL;
	temp_series->howto_norm=anorm;
	temp_series->x=x;
	temp_series->y=y;
	return temp_series;
}

void som_release_series(som_temporal_series_t **aseries_ptr)
{
	som_temporal_series_t *temp_series=*aseries_ptr;
	som_temporal_weight_t *runner=temp_series->weight_list;
	som_temporal_weight_t *temp;
	while(runner)
	{
		temp=runner;
		runner=runner->next;
		temp_series->som_timer->conf->weight_pool->free(temp);
	}
	delete temp_series;
	temp_series=NULL;
}

som_temporal_timer_t* som_create_timer(som_temporal_conf_t *aconf)
{
	som_temporal_timer_t* temp_timer = new som_temporal_timer_t;
	temp_timer->conf=aconf;
	temp_timer->time=0;
	return temp_timer;
}

void som_release_timer(som_temporal_timer_t **timer_ptr)
{
	som_temporal_timer_t* temp_timer = *timer_ptr;
	temp_timer->conf=NULL;
	delete temp_timer;
	temp_timer=NULL;
}

void som_increase_timer(som_temporal_timer_t *timer)
{
	timer->time++;	
}

som_temporal_weight_t *  som_find_neighbor(som_temporal_series_t *aseries,CvScalar *value)
{
	double y,y_max,y_min;
	som_temporal_weight_t *min_weight;
	som_temporal_weight_t *runner;

	y_min=3000;
	y_max=0;
	min_weight=NULL;

	if(!aseries->weight_list) return NULL;
	
	runner=aseries->weight_list;
	//find the closest weight
	//and simultaneously compute mean distance
	while(runner)
	{
		//y=norm_infty(&runner->value,value);
		y=tri_diff(&runner->value,value,aseries->howto_norm);
		if(y_max < y) y_max=y;
		if(y_min > y)
		{
			y_min=y;
			min_weight=runner;
		}
		runner=runner->next;
	}

	//if((aseries->x==125) && (aseries->y==240))printf(" ratio test ");
	if( y_max > y_min *  aseries->som_timer->conf->generating_threshold)
	{
		min_weight=NULL;
	}

	//if((aseries->x==125) && (aseries->y==240))printf(" irrelevance test ");
	if( y_min > aseries->som_timer->conf->irrelevance_threshold)
	{
		min_weight=NULL;
	}

	//if((aseries->x==125) && (aseries->y==240))printf(" sanity test ");
	//if(aseries->length > 1 ) printf("Warning : Algorithmic instability \n");
	return min_weight;
}

//creates a new weight
void  som_create_weight(som_temporal_series_t *aseries,CvScalar *value)
{
#ifdef SOM_LOG
	printf("Create \n");
#endif
	som_temporal_weight_t * pp = (som_temporal_weight_t *) aseries->som_timer->conf->weight_pool->malloc();	
	pp->next=aseries->weight_list;
	pp->prev=NULL;
	if(aseries->weight_list) aseries->weight_list->prev=pp;
	aseries->weight_list=pp;
	pp->fg_class_timeout=aseries->som_timer->conf->fg_class_threshold;
	pp->rarity_timeout=aseries->som_timer->conf->rarity_threshold;
	for(int l=0;l<4;l++) pp->value.val[l]=value->val[l];	
	aseries->length++;
}

//we refresh weights here per  time TIC-TAC-TOE
void som_remove_stale_weights(som_temporal_series_t *aseries)
{
	som_temporal_weight_t *runner=aseries->weight_list;
	som_temporal_weight_t *temp;
	
	//delete rare nodes
	while(runner)
	{		
		temp=runner;
		runner=runner->next;
		if(temp->fg_class_timeout > 0) temp->fg_class_timeout--;
		if(temp->rarity_timeout > 0) temp->rarity_timeout--;
		
		if(!temp->rarity_timeout)
		{
#ifdef SOM_LOG
	printf("Purge \n");
#endif
			som_delete_weight(aseries,temp);
		}
	}
}

//we refresh weights here per  time TIC-TAC-TOE
void som_merge_weights(som_temporal_series_t *aseries)
{
	som_temporal_weight_t *runner=aseries->weight_list;
	som_temporal_weight_t *temp;
	double y;		

	//merge close nodes	
	while(runner)
	{
		temp=runner;
		runner=runner->next;
		if(runner)
		{
			//y=norm_infty(&temp->value,&runner->value);
			y=tri_diff(&runner->value,&temp->value,aseries->howto_norm);
			if( y < aseries->som_timer->conf->joining_threshold)
			{
#ifdef SOM_LOG
	printf("Merge \n");
#endif
				temp->fg_class_timeout = min(runner->fg_class_timeout,temp->fg_class_timeout);
				temp->rarity_timeout = max(runner->rarity_timeout,temp->rarity_timeout);				
				temp->next=runner->next;
				som_delete_weight(aseries,runner);
				runner=temp;
			}
		}
	}	
}

//here we adjust a weight for a node after insertion and detrmine if still fg
void som_adjust_weight(som_temporal_series_t *aseries,som_temporal_weight_t *aweight,CvScalar *value)
{		
	som_temporal_weight_t *temp;	
	double param;
	
	param= aseries->som_timer->conf->the_A;
	temp=aweight;
	//adjust the weight in pos
	for(int l=0;l<4;l++) temp->value.val[l] += param * (value->val[l] - temp->value.val[l]);
	
	param *= aseries->som_timer->conf->the_B;
	//transmit to the previous
	if(temp=aweight->prev)
	{
		for(int l=0;l<4;l++) temp->value.val[l] += param * (value->val[l] - temp->value.val[l]);
	}
	
	//transmit to the next
	if(temp=aweight->next)
	{
		for(int l=0;l<4;l++) temp->value.val[l] += param * (value->val[l] - temp->value.val[l]);
	}

	//refresh rarity
	aweight->rarity_timeout=aseries->som_timer->conf->rarity_threshold;	
}

//what to do with a value
bool som_add_value(som_temporal_series_t *aseries,CvScalar *value)
{
	som_temporal_weight_t *some_weight;

	
	som_temporal_weight_t *runner;
/*
	if((aseries->x==125) && (aseries->y==240))
	{
		printf("before merge %d %f\n",aseries->length,aseries->som_timer->conf->irrelevance_threshold);
		runner=aseries->weight_list;
		while(runner)
		{
			for(int l=0;l<4;l++) printf(" %f ",runner->value.val[l]);
			printf("\n");
			runner=runner->next;
		}
	}
*/

	som_merge_weights(aseries); //first merge close weights
	
/*
	if((aseries->x==125) && (aseries->y==240))
	{
		printf("after merge %d \n",aseries->length);
		runner=aseries->weight_list;
		while(runner)
		{
			for(int l=0;l<4;l++) printf(" %f ",runner->value.val[l]);
			printf("\n");
			runner=runner->next;
		}
	}
*/

	some_weight = som_find_neighbor(aseries,value); //second find neighbor
	
	bool resus;
	if(!some_weight)
	{
		 //if not found close core
		//if((aseries->x==125) && (aseries->y==240)) printf("In 1\n");
		som_create_weight(aseries,value);//create one
		resus=true;
	}
	else
	{
		//if((aseries->x==125) && (aseries->y==240)) printf("In 2\n");
		som_adjust_weight(aseries,some_weight,value);	//else update the close core
		resus=(some_weight->fg_class_timeout!=0); //determine if still foreground		
	}

	//if(resus) printf(" true \n");else printf(" false \n");
	//if(!resus) exit(0);
	som_remove_stale_weights(aseries);
	return resus;
}

//we delete a weight here
void som_delete_weight(som_temporal_series_t *aseries,som_temporal_weight_t * node)
{
	som_temporal_weight_t *temp_neighbor_back,*temp_neighbor_forth;
					
	temp_neighbor_back=node->prev;
	temp_neighbor_forth=node->next;	

	if(temp_neighbor_back) temp_neighbor_back->next=temp_neighbor_forth;
	else aseries->weight_list=temp_neighbor_forth;
	if(temp_neighbor_forth) temp_neighbor_forth->prev=temp_neighbor_back;

	aseries->length--;

	int x=0;
	for(som_temporal_weight_t * runner=aseries->weight_list;runner;runner=runner->next)
	{
		x++;
	}
	if(x!=aseries->length)
	{
		printf("Misalignment %d %d \n",x,aseries->length);
		exit(0);
	}

	aseries->som_timer->conf->weight_pool->free(node);	
}
