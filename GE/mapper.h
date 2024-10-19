# ifndef __MAPPER__H
# define __MAPPER__H
# include <GE/fparser.hh>
# include <MLMODELS/dataset.h>
# include <string>
using namespace std;
typedef vector<double> Data;
class Mapper
{
	private:
		int dimension;
		string vars;
		vector<FunctionParser*> parser;
		vector<int> foundX;
        double *xx;
	public:
		Mapper(int d);
        void	setExpr(vector<string> &s);
        int     map(Data &x,Data &x1);
        void    mapDataset(Dataset *original,Dataset *mappedDataset);
		~Mapper();
};
# endif
