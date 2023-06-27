#ifndef DEF_FLINSYS
#define DEF_FLINSYS

#include <vector>
#include <complex>
#include <iostream>
#include <cassert>
#include <Eigen/Sparse>
// #include <Eigen/SparseLU>


#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

using namespace std;

template<typename T>

class fLinSys
{
public: 	
	fLinSys();
	fLinSys(int size, int nRhs);
	fLinSys(Eigen::SparseMatrix<T> mat, Eigen::SparseMatrix<T> rhs);
	~fLinSys();
	bool fillMatrix();
	bool printMatrix();
	//bool buildLU();
	//bool printLU();
	bool solve();
	int getSize();
	//Eigen::SparseMatrix<T> getL();
	//Eigen::SparseMatrix<T> getU();
	Eigen::SparseMatrix<T> getSolution();
	bool isSymmetric();
	bool getSymmetric();

private: 
	T const m_eps;
	int m_size;
	int m_nRhs;
	bool m_luDone;
	bool m_solved;
	int m_sym;
	Eigen::SparseMatrix<T> m_mat;
	Eigen::SparseMatrix<T> m_l;
	Eigen::SparseMatrix<T> m_u;
	Eigen::SparseMatrix<T> m_rhs;
	Eigen::SparseMatrix<T> m_solution;
	//vector<vector<double>> m_mat;
	//vector<vector<double>> m_l;
	//vector<vector<double>> m_u;
	//vector<vector<double>> m_rhs;
	//vector<vector<double>> m_solution;
};



template<typename T>

fLinSys<T>::fLinSys() : m_eps(1e-12), m_size(0), m_nRhs(0), m_mat(0,0)
{

}

template<typename T>

fLinSys<T>::fLinSys(int size, int nRhs)
	: m_eps(1e-12),
	m_size(size),
	m_nRhs(nRhs),
	m_mat(size, size),
	m_l(size, size),
	m_u(size, size),
	m_rhs(size, nRhs),
	m_solution(size, nRhs)
{
	m_mat.setZero();
	m_rhs.setZero();
	m_luDone = false; 
}

template<typename T>
fLinSys<T>::fLinSys(Eigen::SparseMatrix<T> mat, Eigen::SparseMatrix<T> rhs)
	:
	m_eps(1e-12),
	m_size(mat.rows()),
	m_nRhs(rhs.cols()),
	m_mat(m_size, m_size),
	m_l(m_size, m_size),
	m_u(m_size, m_size),
	m_solution(m_size, m_nRhs)
{
	assert(mat.rows() == mat.cols());
	assert(mat.rows() == rhs.rows());

	m_mat = mat;
	m_rhs = rhs;
	m_luDone = false; 
}

template<typename T>
fLinSys<T>::~fLinSys()
{

}

template<typename T>
int fLinSys<T>::getSize()
{
    return m_size;
}



template<typename T>
bool fLinSys<T>::printMatrix()
{
	cout << m_mat;
	return true; //m_mat.print();
}


template<typename T>
bool fLinSys<T>::fillMatrix()
{
	std::vector<Eigen::Triplet<T>> tripA;
	std::vector<Eigen::Triplet<T>> tripB;

	tripA.reserve(m_size*m_size);
	tripB.reserve(m_size);
	
	T count(0);
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_size; j++)
		{
			count++;
			tripA.push_back(Eigen::Triplet<T>(i,j,count));
		}
		tripB.push_back(Eigen::Triplet<T>(i,0,count));
	}
	m_mat.setFromTriplets(tripA->begin(), tripA->end());
	m_rhs.setFromTriplets(tripB->begin(), tripB->end());
	return true;
}


template<typename T>
bool fLinSys<T>::solve()
{
	Eigen::SparseMatrix<T> y(m_size, m_nRhs); 
	// improve the choice of the solver... 	
	if(!isSymmetric())
	{
		cout << "Using LU" << endl;
		Eigen::SparseLU<Eigen::SparseMatrix<T>, Eigen::COLAMDOrdering<int>> solver;
		solver.analyzePattern(m_mat);
		solver.factorize(m_mat);
		// il manque la loop over the rhs...
		m_solution = solver.solve(m_rhs); 
		if(solver.info()!=Eigen::Success) 
		{
			return false;
		}
		return true;
	}
	else
	{
		cout << "Using LDLT" << endl;
		Eigen::SimplicialLDLT<Eigen::SparseMatrix<T>> solver;
		solver.compute(m_mat);
		if(solver.info()!=Eigen::Success) 
		{
			// decomposition failed
			cout << "Decomposition failed" << endl;
			return false;
		}
		m_solution = solver.solve(m_rhs);
		if(solver.info()!=Eigen::Success) 
		{
			// solving failed
			cout << "LDLT Solve failed" << endl;
			return false;
		}
		
		
		// Eigen::SimplicialLDLT<Eigen::SparseMatrix<T>, Eigen::COLAMDOrdering<int>> solver;
		// solver.analyzePattern(m_mat);
		// solver.factorize(m_mat);
		// // loop over the rhs...
		// m_solution = solver.solve(m_rhs);
		// si ça marche pas : http://www.eigen.tuxfamily.org/dox/group__TopicSparseSystems.html#TutorialSparseSolverConcept 	
	}
	m_solved = true;
	return true; 
}

template<typename T>
bool fLinSys<T>::isSymmetric()
{
	if (true)
	if (abs((m_mat-Eigen::SparseMatrix<T>(m_mat.transpose())).sum()) < m_eps)
	{
		m_sym=2;
		return true;
	}
	m_sym=1;
	return false;
}

template<typename T>
bool fLinSys<T>::getSymmetric()
{
	if(m_sym == 0)
	{
		return isSymmetric();
	}
	if(m_sym == 1)
	{
		return false;
	}
	if(m_sym == 2)
	{
		return true;
	}
	cout << "Aucune raison d'arriver la......" << endl;
	WHEREAMI
	return true;
}

//template<typename T>
//Eigen::SparseMatrix<T> fLinSys<T>::getL()
//{
//	if(!m_luDone)
//	{
//		m_luDone = fLinSys<T>::buildLU();
//	}
//	return m_l;
//}
//template<typename T>
//Eigen::SparseMatrix<T> fLinSys<T>::getU()
//{
//	if(!m_luDone)
//	{
//		m_luDone = fLinSys<T>::buildLU();
//	}
//	return m_u;
//}

template<typename T>
Eigen::SparseMatrix<T> fLinSys<T>::getSolution()
{
	if(!m_solved)
	{
		m_solved = this->solve();
	}
	return m_solution;
}



























































#endif

/*

*/
