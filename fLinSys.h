#ifndef DEF_FLINSYS
#define DEF_FLINSYS

#include <vector>

using namespace std;


class fLinSys
{
public: 	
	fLinSys();
	fLinSys(int size, int nRhs);
	~fLinSys();
	bool fillMatrix();
	bool printMatrix();
	bool buildLU();
	bool printLU();
	bool solve();
	int getSize();
private: 
	int const m_size;
	int const m_nRhs;
	double const m_eps;
	bool m_luDone;
	vector<vector<double>> m_mat;
	vector<vector<double>> m_l;
	vector<vector<double>> m_u;
	vector<vector<double>> m_rhs;
	vector<vector<double>> m_solution;
};

#endif

/*

*/
