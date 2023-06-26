#ifndef DEF_FLINSYS
#define DEF_FLINSYS

#include <vector>
#include <complex>
#include <iostream>
#include <cassert>
//#include "Eigen::SparseMatrix.h"
#include <Eigen/Sparse>


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
	bool buildLU();
	bool printLU();
	bool solve();
	int getSize();
	Eigen::SparseMatrix<T> getL();
	Eigen::SparseMatrix<T> getU();
	Eigen::SparseMatrix<T> getSolution();

private: 
	T const m_eps;
	int m_size;
	int m_nRhs;
	bool m_luDone;
	bool m_solved;
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
	m_size(mat.getSizeM()),
	m_nRhs(rhs.getSizeN()),
	m_mat(m_size, m_size),
	m_l(m_size, m_size),
	m_u(m_size, m_size),
	m_solution(m_size, m_nRhs)
{
	assert(mat.isSquare());
	assert(mat.getSizeM() == rhs.getSizeM());

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
	return m_mat.print();
}


template<typename T>
bool fLinSys<T>::fillMatrix()
{
	T count(0);
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_size; j++)
		{
			count++;
			m_mat(i,j)= count;
			//m_mat(i,j)=(double)(rand() % 100) / 100;
			//m_mat[i][j] = count ; 
		}
	}
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_nRhs; j++)
		{
			count ++;
			//m_rhs[i][j]=(double)(rand() % 100) / 100;
			m_rhs(i,j) = count;
		}
	}
	return true;
}

template<typename T>
bool fLinSys<T>::buildLU()
{
	if (!m_luDone)
	{
		m_luDone = m_mat.luFact(&m_l, &m_u);
	}
	return m_luDone;
}

template<typename T>
bool fLinSys<T>::printLU()
{
	bool printU(false);
	bool printL(false);

	// just making sure the factorisation has been calculated already
	if (!m_luDone)
	{
		m_luDone = fLinSys<T>::buildLU();
	}
	cout << endl << "L : " << endl;
	printL = m_l.print();
	cout << endl << "U : " << endl;
	printU = m_u.print();
	cout << endl << "L*U" << endl;
	Eigen::SparseMatrix<T> lu(m_l*m_u);
	lu.print();
	//cout << endl << "A+A" << endl;
	//Eigen::SparseMatrix<T> apa(m_mat+m_mat);
	//apa.print();
	//cout << endl << "2A" << endl;
	//Eigen::SparseMatrix<T> a2(m_mat+m_mat);
	//a2.print();
	//cout << endl << endl;

	return (printL && printU);
	//for(int i = 0; i < m_size ; i++)
	//{
	//	for(int j = 0; j <= i ; j++)
	//	{
	//		cout << m_l(i,j) << " " ;
	//	}
	//	cout << endl;
	//}
	//cout << "U : " << endl;
	//for(int i = 0; i < m_size ; i++)
	//{
	//	for(int j = 0; j < m_size ; j++)
	//	//for(int j = 0; j < m_size-i ; j++)
	//	{
	//		cout << m_u(i,j) << " " ;
	//	}
	//	cout << endl;
	//}
	//cout << "L*U : " << endl;
	//for(int i = 0; i < m_size ; i++)
	//{
	//	for(int j = 0; j < m_size ; j++)
	//	{
	//		sum = 0;
	//		for(int k = 0; k < m_size ; k++)
	//		{
	//			sum += m_l(i,k)*m_u(k,j);
	//		}
	//		cout << sum << " " ;
	//	}
	//	cout << endl;
	//}
	//return true;
}

template<typename T>
bool fLinSys<T>::solve()
{
	Eigen::SparseMatrix<T> y(m_size, m_nRhs); 
	T sum(0);
	if (!m_luDone)
	{
		//cout << "Performing LU factorization" << endl; 
		m_luDone = fLinSys<T>::buildLU();
	}
	// loop on the rhs 
	//cout << "LU descente - remontée" << endl;
	for(int l = 0; l < m_nRhs; l++)
	{
		// solving Ly = b 
		for(int i = 0 ; i < m_size ; i++)
		{
			sum = 0;
			for(int k = 0; k<i; k++)
			{
				sum+=m_l(i,k)*y(k,l);
			}
			// cette ligne n'est pas hyper utile parce que la diagonale de L est unitaire, mais ca fait trop mal au coeur de diviser sans verifier, deso !
			assert(abs(m_l(i,i)) > m_eps && "pivot nul dans une matrice L... how strange !");
			y(i,l) = (m_rhs(i,l)  - sum)/m_l(i,i);
		}

		// solving Ux = y
		for(int i = m_size-1; i >= 0 ; i--)
		{
			sum = 0;
			for(int k = i+1; k < m_size ; k++)
			{
				sum += m_u(i,k)*m_solution(k,l);
			}
			//cout << sum << endl;
			//cout << abs(m_u(i,i)) << endl;
			assert(abs(m_u(i,i)) > m_eps);
			m_solution(i,l) = (y(i,l) - sum)/m_u(i,i);
		}
	
	}
	m_solved = true;
	return true; 
}




template<typename T>
Eigen::SparseMatrix<T> fLinSys<T>::getL()
{
	if(!m_luDone)
	{
		m_luDone = fLinSys<T>::buildLU();
	}
	return m_l;
}
template<typename T>
Eigen::SparseMatrix<T> fLinSys<T>::getU()
{
	if(!m_luDone)
	{
		m_luDone = fLinSys<T>::buildLU();
	}
	return m_u;
}

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
