#include <iostream>
#include <vector>
#include <cassert>

#include "fLinSys.h"

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

fLinSys::fLinSys() : m_eps(1e-12), m_size(0), m_nRhs(0), m_mat(0,0)
{

}

fLinSys::fLinSys(int size, int nRhs)
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

fLinSys::fLinSys(fMatrix mat, fMatrix rhs)
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

fLinSys::~fLinSys()
{

}

int fLinSys::getSize()
{
    return m_size;
}



bool fLinSys::printMatrix()
{
	return m_mat.print();
}


bool fLinSys::fillMatrix()
{
	double count(0);
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

bool fLinSys::buildLU()
{
	double sum(0);
	//int k(0);
	//int i(0);

	//m_l.resize(m_size);
	//m_u.resize(m_size);
	//for(int i = 0; i < m_size ; i++)
	//{
	//	m_l[i].resize(i+1);
	//	m_u[i].resize(m_size);
	//}

	//cout << endl << endl; 
	// i = 0;
	for(int i = 0; i < m_size-1; i++)	
	{
		for(int j = i ; j < m_size ; j++)
		{
			sum = 0;
			//k = 0;
			for(int k = 0; k < i; k++)
			{
				sum += m_l(i,k) * m_u(k,j);
			}
			// cout << "sum u: " << sum << " ; k = " << k << "; i =  " << i << ";" << endl; 
			m_u(i,j) = m_mat(i,j) - sum;
			// cout << m_u[i][j] << endl ;
		}
		for(int j = i+1 ; j < m_size ; j++)
		{
			sum = 0;
			for(int k = 0; k < i; k++)
			{
				sum += m_l(j,k) * m_u(k,i);
			}
			//cout << "sum l: " << sum << endl; 
			m_l(j,i) = 1/m_u(i,i)*(m_mat(j,i) - sum);
			// cout << m_l[i][j] << endl;
		}
	}
	sum = 0;
	for(int k = 0 ; k < m_size-1 ; k++)
	{
		sum+=m_l(m_size-1,k)*m_u(k,m_size-1);
	}
	m_u(m_size-1,m_size-1) = m_mat(m_size-1,m_size-1) - sum;
	for(int i = 0 ; i < m_size ; i++)
	{
		m_l(i,i) = 1;
	}
	//cout << endl << endl;

	// a ajouter : calcul du determinant avec la trace de U
	
	
	return true; 
}

bool fLinSys::printLU()
{
	bool printU(false);
	bool printL(false);

	// just making sure the factorisation has been calculated already
	if (!m_luDone)
	{
		m_luDone = fLinSys::buildLU();
	}
	cout << endl << "L : " << endl;
	printL = m_l.print();
	cout << endl << "U : " << endl;
	printU = m_u.print();
	cout << endl << "L*U" << endl;
	fMatrix lu(m_l*m_u);
	lu.print();
	cout << endl << "A+A" << endl;
	fMatrix apa(m_mat+m_mat);
	apa.print();
	cout << endl << "2A" << endl;
	fMatrix a2(m_mat+m_mat);
	a2.print();
	cout << endl << endl;

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

bool fLinSys::solve()
{
	fMatrix y(m_size, m_nRhs); 
	double sum(0);
	// making sure the factorisation A = LU is done 
	if (!m_luDone)
	{
		m_luDone = fLinSys::buildLU();
	}
	
	// loop on the rhs 
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
			assert(abs(m_l(i,i)) > m_eps);
			//assert(m_l[i][i] < m_eps,"pivot nul, c'est ballot !" );
			y(i,l) = (m_rhs(i,l)  - sum)/m_l(i,i);
		}

		// // printing y 
		// cout << endl << "y" << endl;
		// for (int i = 0; i < m_size ; i++)
		// {
		// 	cout << y[i][l] << endl;
		// }
		// cout << endl ;
		
		// solving Ux = y
		for(int i = m_size-1; i >= 0 ; i--)
		{
			sum = 0;
			for(int k = i+1; k < m_size ; k++)
			{
				sum += m_u(i,k)*m_solution(k,l);
			}
			//cout << sum << endl;
			//cout << abs(m_u[i][i]) << endl;
			assert(abs(m_u(i,i)) > m_eps);
			m_solution(i,l) = (y(i,l) - sum)/m_u(i,i);
		}
	
	}
	// printing results 
	// cout << endl << "Result for first RHS " << endl ;
	// for (int i = 0; i< m_size ; i++)
	// {
	// 	cout << m_solution[i][1] << endl; 
	// }
	
	//for (int l=0; l < m_nRhs ; l++)
	//{
	//	cout << endl << "A*x(" << l+1 << ") " << endl ;
	//	for (int i = 0; i< m_size ; i++)
	//	{
	//		sum = 0;
	//		for (int k = 0 ; k < m_size ; k++)
	//		{
	//			sum += m_mat[i][k]*m_solution[k][l];
	//		}
	//		cout << sum << endl; 
	//	}
	//}

	m_solved = true;
	return true; 
}




fMatrix fLinSys::getL()
{
	if(!m_luDone)
	{
		m_luDone = fLinSys::buildLU();
	}
	return m_l;
}
fMatrix fLinSys::getU()
{
	if(!m_luDone)
	{
		m_luDone = fLinSys::buildLU();
	}
	return m_u;
}

fMatrix fLinSys::getSolution()
{
	if(!m_solved)
	{
		m_solved = this->solve();
	}
	return m_solution;
}










