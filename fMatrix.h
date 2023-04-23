#ifndef DEF_FMATRIX
#define DEF_FMATRIX

#include <vector>

using namespace std;


class fMatrix
{
public: 	
	fMatrix();
	fMatrix(int m, int n);
	~fMatrix();
	bool fillMatrix();
	bool printMatrix();
	int getSizeM();
	int getSizeN();
	bool getIsSquare();
private: 
	int const m_m;
	int const m_n;
	vector<vector<double>> m_mat;
};

#endif

/*

*/
