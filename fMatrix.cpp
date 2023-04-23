#include <iostream>
#include <vector>
#include <cassert>

#include "fMatrix.h"

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

fMatrix::fMatrix() : m_m(0), m_n(0)
{

}

fMatrix::fMatrix(int m, int n)
	: m_m(m),
	m_n(n)
{
	m_mat.resize(m_m);
	for(int i = 0; i < m_m ; i++)
	{
		m_mat[i].resize(m_n);
	}

	//fLinSys::fillMatrix();
}

fMatrix::~fMatrix()
{

}

int fMatrix::getSizeM()
{
    return m_m;
}
int fMatrix::getSizeN()
{
    return m_n;
}



bool fMatrix::printMatrix()
{
	for(int i = 0; i<m_m; i++)
	{
		for(int j = 0; j<m_n; j++)
		{
			cout << m_mat[i][j] << " ";
		}
		cout << endl; 
	}
	cout << endl;
	return true;
}


bool fMatrix::fillMatrix()
{
	double count(0);
	for(int i = 0; i<m_m; i++)
	{
		for(int j = 0; j<m_n; j++)
		{
			count++;
			m_mat[i][j]=(double)(rand() % 100) / 100;
			//m_mat[i][j] = count ; 
		}
	}
	return true;
}





bool fMatrix::getIsSquare()
{
	return m_m == m_n;
}













