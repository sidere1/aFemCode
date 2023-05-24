#ifndef DEF_FLINSYS
#define DEF_FLINSYS

#include <vector>
#include "fMatrix.h"
using namespace std;


class fLinSys
{
public: 	
	fLinSys();
	fLinSys(int size, int nRhs);
	fLinSys(fMatrix mat, fMatrix rhs);
	~fLinSys();
	bool fillMatrix();
	bool printMatrix();
	bool buildLU();
	bool printLU();
	bool solve();
	int getSize();
	fMatrix getL();
	fMatrix getU();
	fMatrix getSolution();

private: 
	double const m_eps;
	int m_size;
	int m_nRhs;
	bool m_luDone;
	bool m_solved;
	fMatrix m_mat;
	fMatrix m_l;
	fMatrix m_u;
	fMatrix m_rhs;
	fMatrix m_solution;
	//vector<vector<double>> m_mat;
	//vector<vector<double>> m_l;
	//vector<vector<double>> m_u;
	//vector<vector<double>> m_rhs;
	//vector<vector<double>> m_solution;
};

#endif

/*

*/
