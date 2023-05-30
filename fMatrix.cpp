#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

#include "fMatrix.h"
#include "fLinSys.h"

using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

//  FINALEMENT TOUT ÇA EST BALANCÉ VIOLEMMENT DANS LE .H !!

//template<typename T>
//fMatrix<T>::fMatrix() : m_m(0), m_n(0)
//{
//
//}
//
//template<typename T>
//fMatrix<T>::fMatrix(unsigned int m, unsigned int n)
//	: m_m(m),
//	m_n(n)
//{
//	m_mat.resize(m_m);
//	for(unsigned int i = 0; i < m_m ; i++)
//	{
//		m_mat[i].resize(m_n);
//	}
//}
//
//
////template<typename T>
////fMatrix<T>::fMatrix(int m, int n)
////{
////	unsigned int um(m);
////	unsigned int un(n);
////	m_m = um;
////	m_n = un;
////	m_mat.resize(m_m);
////	for(unsigned int i = 0; i < m_m ; i++)
////	{
////		m_mat[i].resize(m_n);
////	}
////}
////template<typename T>
////fMatrix<T>::fMatrix(unsigned long m, unsigned long n)
////{
////	unsigned int um(m);
////	unsigned int un(n);
////	m_m = um;
////	m_n = un;
////	m_mat.resize(m_m);
////	for(unsigned int i = 0; i < m_m ; i++)
////	{
////		m_mat[i].resize(m_n);
////	}
////}
//
//
//template<typename T>
//fMatrix<T>::~fMatrix()
//{
//
//}
//
//template<typename T>
//unsigned int fMatrix<T>::getSizeM()
//{
//    return m_m;
//}
//template<typename T>
//unsigned int fMatrix<T>::getSizeN()
//{
//    return m_n;
//}
//
//
//
//template<typename T>
//bool fMatrix<T>::print()
//{
//	for(unsigned int i = 0; i<m_m; i++)
//	{
//		for(unsigned int j = 0; j<m_n; j++)
//		{
//			cout << m_mat[i][j] << " ";
//		}
//		cout << endl; 
//	}
//	cout << endl;
//	return true;
//}
//
//template<typename T>
//bool fMatrix<T>::spyPrint()
//{
//	char disp(' ');
//	for(unsigned int i = 0; i<m_m; i++)
//	{
//		for(unsigned int j = 0; j<m_n; j++)
//		{
//			if (m_mat[i][j] == 0)
//			{
//				disp = ' ';
//			}
//			else 
//			{
//				disp = '.';
//			}
//			cout << disp;
//		}
//		cout << endl; 
//	}
//	cout << endl;
//	return true;
//}
//
//template<typename T>
//bool fMatrix<T>::fill()
//{
//	double count(0);
//	for(unsigned int i = 0; i<m_m; i++)
//	{
//		for(unsigned int j = 0; j<m_n; j++)
//		{
//			count++;
//			m_mat[i][j]=(double)(rand() % 100) / 100;
//			//m_mat[i][j] = count ; 
//		}
//	}
//	return true;
//}
//
//
//
//
//
//template<typename T>
//bool fMatrix<T>::isSquare() const
//{
//	return m_m == m_n;
//}
//
//
//
//// ---- Operator overloading ----
//
//// access by indices 
//template<typename T>
//T const & fMatrix<T>::operator()(std::size_t x, std::size_t y) const
//{
//	if ((x >= m_m) || (y >= m_n))
//	{
//		cout << endl << "WARNING incorrect access to matrix of size " << m_m << "*" << m_n << endl;
//		cout << "you asked position " << x << "*" << y << endl << endl;
//		assert(((x < m_m) && (y < m_n)));
//	}
//	return m_mat[x][y];
//}
//template<typename T>
//T& fMatrix<T>::operator()(std::size_t x, std::size_t y)
//{
//	if ((x >= m_m) || (y >= m_n))
//	{
//		cout << endl << "WARNING incorrect access to matrix of size " << m_m << "*" << m_n << endl;
//		cout << "you asked position " << x << "*" << y << endl << endl;
//		assert(((x < m_m) && (y < m_n)));
//	}
//    return m_mat[x][y];
//}
//
//// multiplication 
//
//
//template<typename T>
//fMatrix<T> operator*(fMatrix<T> const& leftM, fMatrix<T> const& rightM)
//// ca, ca marche pas : fMatrix fMatrix<T>::operator*(fMatrix const & leftM, fMatrix const & rightM)
//{
//	T sum(0);
//    fMatrix<T> res(leftM.m_m, rightM.m_n);
//	
//	if(leftM.m_n!=rightM.m_m)
//	{
//		cout << "impossible operation. leftM is of size (" << leftM.m_m << "," << leftM.m_n << "); rightM (" << rightM.m_m << "," << rightM.m_n << ")." << endl;
//		assert(leftM.m_n==rightM.m_m);
//	}
//    
//	for (unsigned int i = 0 ; i < res.m_m; i++)
//	{
//		for(unsigned int j = 0; j < res.m_n; j++)
//		{
//			sum = 0;
//			for(unsigned int k = 0; k < leftM.m_n; k++)
//			{
//				sum+= leftM(i,k)*rightM(k,j);
//			}
//			res(i,j)=sum;
//		}
//	}
//	//for (int i = 0 ; i < copie.m_n; ++i)
//    //{
//    //    auto const ligne_courante { leftM.ligne(i).m_matrice };
//    //    for (std::size_t j { 0 }; j < copie.m_n; ++j)
//    //    {
//    //        auto const colonne_courante { rightM.colonne(j).m_matrice };
//    //        const int valeur { std::inner_product(std::begin(ligne_courante), std::end(ligne_courante), std::begin(colonne_courante), 0) };
//    //        copie(i, j) = valeur;
//    //    }
//    //}
//
//    return res;
//}
//
//template<typename T>
//fMatrix<T> operator*(T const& scal, fMatrix<T> const& rightM)// left scalar multiplication
//{
//    fMatrix<T> res(rightM.m_m, rightM.m_n);
//	
//	for (unsigned int i = 0 ; i < res.m_m; i++)
//	{
//		for(unsigned int j = 0; j < res.m_n; j++)
//		{
//			res(i,j)=scal*rightM(i,j);
//		}
//	}
//
//    return res;
//}
//
//		
//template<typename T>
//fMatrix<T> operator*(fMatrix<T> const & leftM, T const& scal) //right scalar multiplication
//{
//	return scal*leftM;
//}
//
//template<typename T>
//fMatrix<T> operator+(fMatrix<T> const& leftM, fMatrix<T> const& rightM)
//{
//    fMatrix<T> res(leftM.m_m, rightM.m_n);
//	
//	assert(leftM.m_n==rightM.m_m);
//    
//	for (unsigned int i = 0 ; i < res.m_m; i++)
//	{
//		for(unsigned int j = 0; j < res.m_n; j++)
//		{
//			res(i,j)=leftM(i,j)+rightM(i,j);
//		}
//	}
//
//    return res;
//}
//
//template<typename T>
//fMatrix<T> fMatrix<T>::t() const
//{ // transposition
//    fMatrix<T> M(m_n, m_m);
//
//    for(unsigned int i=0; i < m_m; ++i)
//    {
//        for(unsigned int j=0; j < m_n; ++j)
//        {
//            M(j,i) = this->m_mat[i][j];
//        }
//    }
//
//    return M;
//}
//
//template<typename T>
//fMatrix<T> fMatrix<T>::submat(unsigned int a, unsigned int b, unsigned int c, unsigned int d) const 
//{
//	// return a subset of m_mat : lines a to b and columns c to d
//	assert(a <= b);
//	assert(c <= d);
//	assert(b < m_m);
//	assert(d < m_n);
//	
//	fMatrix m(b-a+1, d-c+1);
//	
//	unsigned int ci(0);
//	unsigned int cj(0);
//
//	for(unsigned int i = a; i<=b; i++)
//	{
//		cj = 0;
//		for(unsigned int j = c; j<=d ; j++)
//		{
//			m(ci, cj) = m_mat[i][j];
//			cj++;
//		}
//		ci++;
//	}
//	return m;
//}
//
//
//template<typename T>
//fMatrix<T> fMatrix<T>::swapLines(vector<int> perm) const
//{
//	// reorganizes the lines of the matrix
//	//
//	if (perm.size()!= m_m)
//	{
//		cout << "perm.size() = " << perm.size() << " and m_m = " << m_m << endl;
//		assert(perm.size() == m_m);
//	}
//	
//	fMatrix<T> v(m_m, m_n);
//	unsigned int newI;
//	
//	for(unsigned int i = 0 ; i < m_m ; i++)
//	{
//		newI = perm[i];
//		assert(newI<m_m);
//		for(unsigned int j = 0 ; j < m_n ; j++)
//		{
//			v(i, j) = m_mat[newI][j];
//		}
//	}
//	return v;
//}
//
//template<typename T>
//T fMatrix<T>::trace() const
//{
//	assert((m_m == m_n) && "trace() can be used only on a square matrix");
//	T prod(1);
//	for(unsigned int i = 0; i < m_m ; i++)
//	{
//		prod*=m_mat[i][i];
//	}
//}
//
//template<typename T>
//T fMatrix<T>::det() const
//{// calcul du déterminant par Tr(L)*Tr(U);
//	assert((m_m == m_n) && "det() can be used only on a square matrix");
//	T det(0);
//	fMatrix zero(m_m,1);
//	zero.setZero();
//	fLinSys<T> s(*this,zero);
//	s.buildLU();
//	fMatrix l(s.getL());
//	fMatrix u(s.getU());
//
//	return l.trace()*u.trace();	
//}
//
//
//template<typename T>
//T fMatrix<T>::getFemDetJac() const
//{
//	//jacobian matrices may be 3*3 for 3D elements, 2*3 for 2D elements, 1*3 for 1D elements
//	//
//	assert(m_m < 4);
//	assert(m_m > 0);
//	assert(m_n == 3);
//
//	if (m_m == 3)
//	{
//		return this->det();
//	}
//	else if (m_m == 2)
//	{
//		// norm(cross(JacG(1,:), JacG(2,:)))
//		fMatrix a(3,1);
//		a(0,0) = (m_mat[1][0]*m_mat[2][1])-(m_mat[2][0]*m_mat[1][1]);
//		a(1,0) = (m_mat[0][1]*m_mat[2][0])-(m_mat[2][1]*m_mat[0][0]);
//		a(2,0) = (m_mat[0][0]*m_mat[1][1])-(m_mat[1][0]*m_mat[0][1]);
//		return a.norm2();
//	}
//	else if (m_m == 1)
//	{
//		return sqrt(pow(m_mat[0][0],2) + pow(m_mat[0][1], 2) + pow(m_mat[0][2], 2));
//	}
//	else
//	{
//		cout << "strange ! I should never have gotten here !" << endl;
//		WHEREAMI
//	}
//}
//
//
//
//template<typename T>
//T fMatrix<T>::norm2()
//{
//	// il faudrait le decouper en sous fonctions... 
//	assert(m_n == 1);
//	double sum(0);
//	for(unsigned int i = 0 ; i < m_m ; i++)
//	{
//		sum += pow(m_mat[i][0],2);
//	}
//	return sqrt(sum);
//}
//
//
//template<typename T>
//T fMatrix<T>::sum() const // somme de tous les termes 
//{
//	T sum(0);
//	for(unsigned int i=0 ; i < m_m; i++)
//	{
//		for(unsigned int j = 0 ; j < m_n ; j++)
//		{
//			sum+=m_mat[i][j];
//		}
//	}
//	return sum;
//}
//
//template<typename T>
//fMatrix<T> fMatrix<T>::inv() const // only for matrices or size 2 and 3 for now ;) 
//{
//	assert(m_m < 4);
//	assert(m_m == m_n);
//
//	fMatrix inv(m_m, m_m);
//	if(m_m == 2)
//	{
//		inv(0,0) = m_mat[1][1];
//		inv(1,0) = -m_mat[1][0];
//		inv(0,1) = -m_mat[0][1];
//		inv(1,1) = m_mat[0][0];
//		inv = inv*(1/(this->det()));
//	}
//	else
//	{
//		inv(0,0) = (m_mat[1][1]*m_mat[2][2]-m_mat[1][2]*m_mat[2][1]);
//		inv(1,0) = (m_mat[1][2]*m_mat[2][0]-m_mat[1][0]*m_mat[2][2]);
//		inv(2,0) = (m_mat[1][0]*m_mat[2][1]-m_mat[1][1]*m_mat[2][0]);
//		inv(0,1) = (m_mat[0][2]*m_mat[2][1]-m_mat[0][1]*m_mat[2][2]);
//		inv(1,1) = (m_mat[0][0]*m_mat[2][2]-m_mat[0][2]*m_mat[2][0]);
//		inv(2,1) = (m_mat[0][1]*m_mat[2][0]-m_mat[0][0]*m_mat[2][1]);
//		inv(0,2) = (m_mat[0][1]*m_mat[1][2]-m_mat[0][2]*m_mat[1][1]);
//		inv(1,2) = (m_mat[0][2]*m_mat[1][0]-m_mat[0][0]*m_mat[1][2]);
//		inv(2,2) = (m_mat[0][0]*m_mat[1][1]-m_mat[0][1]*m_mat[1][0]);	
//		inv = inv*(1/(this->det()));
//	}
//	return inv;
//}
//
//
//
//
//
//
//
//
//
//// ---- Basic matrices ----
//template<typename T>
//bool fMatrix<T>::setOne()
//{
//	for(unsigned int i = 0 ; i < m_m ; i++)
//	{
//		for(unsigned int j = 0 ; j < m_n ; j++)
//		{
//			m_mat[i][j] = 1;
//		}
//	}
//	return true;
//}
//template<typename T>
//bool fMatrix<T>::setZero()
//{
//	for(unsigned int i = 0 ; i < m_m ; i++)
//	{
//		for(unsigned int j = 0 ; j < m_n ; j++)
//		{
//			m_mat[i][j] = 0;
//		}
//	}
//	return true;
//}
//template<typename T>
//bool fMatrix<T>::setId()
//{
//	if(m_m != m_n)
//	{
//		cout << "the matrix is not square, I cannot set it to identity" << endl;
//		return false; 
//	}
//	else 
//	{
//		for(unsigned int i = 0 ; i < m_m ; i++)
//		{
//			for(unsigned int j = 0 ; j < m_m ; j++)
//			{
//				m_mat[i][j] = 0;
//			}
//		}
//		for(unsigned int i = 0 ; i < m_m ; i++)
//		{
//			m_mat[i][i] = 1;
//		}
//		return true;
//	}
//}
