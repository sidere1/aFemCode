#include <iostream>
#include <vector>
#include <cassert>

#include "fLinSys.h"

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

fLinSys::fLinSys() : m_size(0), m_nRhs(0), m_eps(1e-12)
{

}

fLinSys::fLinSys(int size, int nRhs)
	: m_size(size),
	m_nRhs(nRhs),
	m_eps(1e-12)
{
	cout << "Building a system of size " << m_size << " and " << m_nRhs << " right hand sides" << endl;
	m_mat.resize(m_size);
	for(int i = 0; i < m_size ; i++)
	{
		m_mat[i].resize(m_size);
	}

	m_rhs.resize(m_size);
	for(int i = 0; i < m_size ; i++)
	{
		m_rhs[i].resize(m_nRhs);
	}
	
	m_solution.resize(m_size);
	for(int i = 0; i < m_size ; i++)
	{
		m_solution[i].resize(m_nRhs);
	}
	
	
	m_luDone = false; 

	fLinSys::fillMatrix();
	//fLinSys::printMatrix();
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
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_size; j++)
		{
			cout << m_mat[i][j] << " ";
		}
		cout << endl; 
	}
	cout << endl;
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_nRhs; j++)
		{
			cout << m_rhs[i][j] << " ";
		}
		cout << endl; 
	}
	return true;
}


bool fLinSys::fillMatrix()
{
	double count(0);
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_size; j++)
		{
			count++;
			m_mat[i][j]=(double)(rand() % 100) / 100;
			//m_mat[i][j] = count ; 
		}
	}
	for(int i = 0; i<m_size; i++)
	{
		for(int j = 0; j<m_nRhs; j++)
		{
			count ++;
			//m_rhs[i][j]=(double)(rand() % 100) / 100;
			m_rhs[i][j] = count;
		}
	}
	return true;
}

bool fLinSys::buildLU()
{
	double sum(0);
	//int k(0);
	//int i(0);

	m_l.resize(m_size);
	m_u.resize(m_size);
	for(int i = 0; i < m_size ; i++)
	{
		m_l[i].resize(i+1);
		m_u[i].resize(m_size);
	}

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
				sum += m_l[i][k] * m_u[k][j];
			}
			// cout << "sum u: " << sum << " ; k = " << k << "; i =  " << i << ";" << endl; 
			m_u[i][j] = m_mat[i][j] - sum;
			// cout << m_u[i][j] << endl ;
		}
		for(int j = i+1 ; j < m_size ; j++)
		{
			sum = 0;
			for(int k = 0; k < i; k++)
			{
				sum += m_l[j][k] * m_u[k][i];
			}
			//cout << "sum l: " << sum << endl; 
			m_l[j][i] = 1/m_u[i][i]*(m_mat[j][i] - sum);
			// cout << m_l[i][j] << endl;
		}
	}
	sum = 0;
	for(int k = 0 ; k < m_size-1 ; k++)
	{
		sum+=m_l[m_size-1][k]*m_u[k][m_size-1];
	}
	m_u[m_size-1][m_size-1] = m_mat[m_size-1][m_size-1] - sum;
	for(int i = 0 ; i < m_size ; i++)
	{
		m_l[i][i] = 1;
	}
	//cout << endl << endl;

	// a ajouter : calcul du determinant avec la trace de U
	
	
	return true; 
}

bool fLinSys::printLU()
{
	double sum(0);
	// just making sure the factorisation has been calculated already
	if (!m_luDone)
	{
		m_luDone = fLinSys::buildLU();
	}

	cout << "L : " << endl;
	for(int i = 0; i < m_size ; i++)
	{
		for(int j = 0; j <= i ; j++)
		{
			cout << m_l[i][j] << " " ;
		}
		cout << endl;
	}
	cout << "U : " << endl;
	for(int i = 0; i < m_size ; i++)
	{
		for(int j = 0; j < m_size ; j++)
		//for(int j = 0; j < m_size-i ; j++)
		{
			cout << m_u[i][j] << " " ;
		}
		cout << endl;
	}
	cout << "L*U : " << endl;
	for(int i = 0; i < m_size ; i++)
	{
		for(int j = 0; j < m_size ; j++)
		{
			sum = 0;
			for(int k = 0; k < m_size ; k++)
			{
				sum += m_l[i][k]*m_u[k][j];
			}
			cout << sum << " " ;
		}
		cout << endl;
	}
	return true;
}

bool fLinSys::solve()
{
	vector<vector<double>> y(m_solution); 
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
				sum+=m_l[i][k]*y[k][l];
			}
			// cette ligne n'est pas hyper utile parce que la diagonale de L est unitaire, mais ca fait trop mal au coeur de diviser sans verifier, deso !
			assert(abs(m_l[i][i]) > m_eps);
			//assert(m_l[i][i] < m_eps,"pivot nul, c'est ballot !" );
			y[i][l] = (m_rhs[i][l]  - sum)/m_l[i][i];
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
				sum += m_u[i][k]*m_solution[k][l];
			}
			//cout << sum << endl;
			//cout << abs(m_u[i][i]) << endl;
			assert(abs(m_u[i][i]) > m_eps);
			m_solution[i][l] = (y[i][l] - sum)/m_u[i][i];
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


	return true; 
}


















