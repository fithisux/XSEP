class STGraph
{
//private:
	int** adj;

public:
	STGraph(int regionsNumber)
	{
		adj = new int *[regionsNumber];
		for(int i=0;i<regionsNumber;i++)
		{
			adj[i]= new int[regionsNumber];
			for(int j=0;j<regionsNumber;j++) adj[i][j]=NULL;
		}
	}

	int get(int n1,int n2)
	{
		return adj[n1][n2];
	}

	void set(int n1,int n2)
	{
		adj[n1][n2]=1;
	}
};
