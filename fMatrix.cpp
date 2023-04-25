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



bool fMatrix::print()
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


bool fMatrix::fill()
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



// ---- Operator overloading ----

// access by indices 
double const & fMatrix::operator()(std::size_t x, std::size_t y) const
{
    return m_mat[x][y];
}
double& fMatrix::operator()(std::size_t x, std::size_t y)
{
    return m_mat[x][y];
}

// multiplication 


fMatrix operator*(fMatrix const& leftM, fMatrix const& rightM)
// ca, ca marche pas : fMatrix fMatrix::operator*(fMatrix const & leftM, fMatrix const & rightM)
{
	double sum(0);
    fMatrix res(leftM.m_m, rightM.m_n);
	
	assert(leftM.m_n==rightM.m_m);
    
	for (int i = 0 ; i < res.m_m; i++)
	{
		for(int j = 0; j < res.m_n; j++)
		{
			sum = 0;
			for(int k = 0; k < leftM.m_n; k++)
			{
				sum+= leftM(i,k)*rightM(k,j);
			}
			res(i,j)=sum;
		}
	}
	//for (int i = 0 ; i < copie.m_n; ++i)
    //{
    //    auto const ligne_courante { leftM.ligne(i).m_matrice };
    //    for (std::size_t j { 0 }; j < copie.m_n; ++j)
    //    {
    //        auto const colonne_courante { rightM.colonne(j).m_matrice };
    //        const int valeur { std::inner_product(std::begin(ligne_courante), std::end(ligne_courante), std::begin(colonne_courante), 0) };
    //        copie(i, j) = valeur;
    //    }
    //}

    return res;
}

fMatrix operator*(double const& scal, fMatrix const& rightM)
{
    fMatrix res(rightM.m_m, rightM.m_n);
	
	for (int i = 0 ; i < res.m_m; i++)
	{
		for(int j = 0; j < res.m_n; j++)
		{
			res(i,j)=scal*rightM(i,j);
		}
	}

    return res;
}

fMatrix operator+(fMatrix const& leftM, fMatrix const& rightM)
{
    fMatrix res(leftM.m_m, rightM.m_n);
	
	assert(leftM.m_n==rightM.m_m);
    
	for (int i = 0 ; i < res.m_m; i++)
	{
		for(int j = 0; j < res.m_n; j++)
		{
			res(i,j)=leftM(i,j)+rightM(i,j);
		}
	}

    return res;
}
