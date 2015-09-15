using namespace std;

class Region
{
public:
	//id
	int id;
	//all the vectors
	double L;
	double U;
	double V;
	bool human;
	//vector type 1
	//double ax;
	//double ay;
	//vector type 2
	double x;
	double y;
	double t;
	//next region in list
	Region* nextRegion;
	//the pixels of the region are also inserted in a stack (linked list)
	Pixel* top;
	int counter;

	//learning constants
	double gama;
	double gamaerror;

	Region(int id, Pixel* p, bool human)
	{
		//initialize the region parameters
		this->id = id;
		this->L = p->L;
		this->U = p->U;
		this->V = p->V;
		this->human = human;
		x=p->auxx;
		y=p->auxy;
		t=p->auxt;
		counter=1;
		gama=0.006;
		gamaerror=0.0001;
		nextRegion=0;
	    //initialize the list
		Pixel* newPixel = new Pixel(p,NULL);
		this->top = newPixel;
		p->regionId=id;
	}
	
	Region(int id)
	{
		//initialize the region parameters
		this->id = id;
		this->L=this->U=this->V = 0;
		this->human = false;
		x=y=t=0;
		counter=0;
		gama=0.006;
		gamaerror=0.0001;
		nextRegion=0;
	    this->top = 0;		
	}

	~Region()
	{
		Pixel* p_swap;

		p_swap=this->top;
		while(p_swap)
		{
			p_swap=this->top->next;
			delete this->top;
			this->top=p_swap;
		}
		this->top=NULL;
		this->nextRegion=0;
	}

	void add(Pixel* p)
	{ 
		Pixel* newPixel = new Pixel(p,this->top);
		this->top = newPixel;
		counter++;
		p->regionId=this->id;
	}

	void penalize(Pixel* p)
	{
		this->L=this->L -gamaerror*(p->L-this->L);
		this->U=this->U -gamaerror*(p->U-this->U);
		this->V=this->V -gamaerror*(p->V-this->V);
		this->x=this->x -gamaerror*(p->auxx-this->x);
		this->y=this->y -gamaerror*(p->auxy-this->y);
		this->t=this->t -gamaerror*(p->auxt-this->t);
	}

	void praiseError(Pixel* p)
	{
		this->L=this->L +gamaerror*(p->L-this->L);
		this->U=this->U +gamaerror*(p->U-this->U);
		this->V=this->V +gamaerror*(p->V-this->V);
		this->x=this->x +gamaerror*(p->auxx-this->x);
		this->y=this->y +gamaerror*(p->auxy-this->y);
		this->t=this->t +gamaerror*(p->auxt-this->t);
	}

	void praiseLearn(Pixel* p)
	{
		this->L=this->L +gama*(p->L-this->L);
		this->U=this->U +gama*(p->U-this->U);
		this->V=this->V +gama*(p->V-this->V);
		this->x=this->x +gama*(p->auxx-this->x);
		this->y=this->y +gama*(p->auxy-this->y);
		this->t=this->t +gama*(p->auxt-this->t);
	}

	void updateCentroid(Pixel* p)
	{
		L=(L*(counter-1)+p->L)/counter;
		U=(U*(counter-1)+p->U)/counter;
		V=(V*(counter-1)+p->V)/counter;
		x=(x*(counter-1)+p->auxx)/((double)counter);
		y=(y*(counter-1)+p->auxy)/((double)counter);
		t=(t*(counter-1)+p->auxt)/((double)counter);
	}
};
