class Index
{
public:
	double value;
	Pixel* start;
	Pixel* end;
	Index* subIndex;
	Index* next;

	Index()
	{
		subIndex=NULL;
		next=NULL;
	}
};
