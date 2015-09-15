class Pixel
{
public:
	int id;
	double L;
	double U;
	double V;
	int auxx;
	int auxy;
	int auxt;
	//if a region is a human region then this flag is true
	//the pixel will be specially handled during the clustering procedure
	bool human;
	//the region it belongs to
	int regionId;
	Pixel* next;

	Pixel(int id, int L, int U, int V, int auxx, int auxy, int auxt)
	{
		this->id = id;
		this->L = L;
		this->U = U;
		this->V = V;
		this->auxx = auxx;
		this->auxy = auxy;
		this->auxt = auxt;
		this->human = false;
		next=NULL;
	}

	Pixel(int id, int L, int U, int V, int auxx, int auxy,
		int auxt, int regionId)
	{
		this->id = id;
		this->L = L;
		this->U = U;
		this->V = V;
		this->auxx = auxx;
		this->auxy = auxy;
		this->auxt = auxt;
		this->human = true;
		this->regionId=regionId;
		next=NULL;
	}

	Pixel(Pixel* base,Pixel* n)
	{
		this->id=base->id;
		this->L=base->L;
		this->U=base->U;
		this->V=base->V;
		this->auxx=base->auxx;
		this->auxy=base->auxy;
		this->auxt=base->auxt;
		this->human=base->human;
		this->next=n;
	}
};
