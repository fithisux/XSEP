class AllocationMatrix
{
//private:
	int** matrix;

public:
	AllocationMatrix(const int rows,const int cols)
	{
		matrix = new int *[rows];
		for(int i=0;i<rows;i++)
			matrix[i] = new int [cols];
	}

	int get(int i,int j)
	{
		return matrix[i][j]; 
	}

	void set(int i,int j,int region)
	{
		matrix[i][j]=region;
	}
};


