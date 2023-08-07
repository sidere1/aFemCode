#ifndef DEF_FMATRIX
#define DEF_FMATRIX

#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>



using namespace std;

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

// un peu d'aide trouvée ici : https://zestedesavoir.com/tutoriels/822/la-programmation-en-c-moderne/la-programmation-orientee-objet/t-p-entrons-dans-la-matrice/#3-correction-detaillee

template <typename T>

class fMatrix
{
public: 	
	fMatrix();
	fMatrix(unsigned int m, unsigned int n);
	//fMatrix(int m, int n);
	//fMatrix(unsigned long m, unsigned long n);
	~fMatrix();
	bool fill();
	bool print();
	bool spyPrint();
	unsigned int getSizeM() const;
	unsigned int getSizeN() const;
	bool isSquare() const;

	// ---- Operator overloading ----

	// operateur indice, accès en lecture
	T const & operator()(std::size_t x, std::size_t y) const;
	// Version d'accès en écriture.
	T& operator()(std::size_t x, std::size_t y);
	fMatrix<double> operator=(fMatrix<float> f)
	{
		unsigned int M(f.getSizeM());
		unsigned int N (f.getSizeN());
		m_m = M; 
		m_n = N;
		m_mat[0].resize(M);

		for(unsigned int i = 0; i < M; i++)
		{
			m_mat[i].resize(N);
		}
		for (unsigned int i = 0;  i < M ; i++)
		{
			for(unsigned int j = 0; j < N ; j++)
			{
				m_mat[i][j] = f(i,j);
			}
		}
		return *this;
	}

	// basic operations 
	//fMatrix<T> operator*(fMatrix<T> const& rightM);
	fMatrix<T> rightMult(fMatrix<T> const& rightM) const;
	fMatrix<T> scalMult(T const& scal) const;
	fMatrix<T> add(fMatrix<T> const& rightM) const;
	fMatrix<T> add(T const& alpha) const;
	fMatrix submat(unsigned int a, unsigned int b, unsigned int c, unsigned int d) const; 
	fMatrix diag() const; 

	// factorisation 
	bool luFact(fMatrix<T> *l, fMatrix<T> *u) const;
	
	// basic matrix manipulations 
	fMatrix<T> t() const; // friend ?
   	fMatrix<T> swapLines(vector<int> perm) const;
	T trace() const;
	T det() const; // computes the det by taking the product of the trace of the factorized LU matrices. Shitty way of doing it (with permutations it would be better...) but for small matrices it's okay :)  
	T getFemDetJac() const; // for 3D element : det. For 2D, cross prod. For 1D, L2 norm.  
	T norm2(); // L2 norm, for m*1 vectors only 
	T sum() const;
	fMatrix<T> inv() const; // only for matrices or size 1 and 2  for now ;) 

	// ---- Basic matrices -----
	bool setOne();// matrix full of ones
	bool setZero();// matrix full of zeros
	bool setId();// Identity, ones on the diagonal for square matrices

private: 
	unsigned int m_m; // il ne faut pas les mettre en const sous peine de voir l'operateur = se faire supprimer... 
	unsigned int m_n;
	vector<vector<T> > m_mat;
};


// les operateurs surcharges ne sont pas definis comme methodes, mais bien en dehors de la classe
template<typename T>
fMatrix<T> operator*(fMatrix<T> const& leftM, fMatrix<T> const& rightM);
template<typename T>
fMatrix<T> operator*(T const & scal, fMatrix<T> const & rightM); // left multiplic
template<typename T>
fMatrix<T> operator*(fMatrix<T> const & leftM, T const & scal); //right mult
template<typename T>
fMatrix<T> operator+(fMatrix<T> const & leftM, fMatrix<T> const & rightM);
template<typename T>
fMatrix<T> operator+(fMatrix<T> const & leftM, T const & alpha);
template<typename T>
ostream& operator<<( ostream &out, fMatrix<T> const& mat )
{
	for(unsigned int i = 0 ; i < mat.getSizeM() ; i++)
	{
		for(unsigned int j = 0 ; j < mat.getSizeN() ; j++)
		{
			if (abs(mat(i,j)) > 0)
			{
				out << mat(i,j) << " ";
			}
			else 
			{
				if (mat.submat(i,i,j,mat.getSizeN()-1).sum() == 0)
				{
					break;
				}
				else
				{
					out << mat(i,j) << " ";
				}
				//out << "  ";
				// this way, we don't print zeros if the end of the line is empty
			}
		}
		out << endl;
	}
    return out;
}







template<typename T>
fMatrix<T>::fMatrix() : m_m(0), m_n(0)
{

}

template<typename T>
fMatrix<T>::fMatrix(unsigned int m, unsigned int n)
	: m_m(m),
	m_n(n)
{
	m_mat.resize(m_m);
	for(unsigned int i = 0; i < m_m ; i++)
	{
		m_mat[i].resize(m_n);
	}
}

template<typename T>
fMatrix<T>::~fMatrix()
{

}

template<typename T>
unsigned int fMatrix<T>::getSizeM() const
{
    return m_m;
}
template<typename T>
unsigned int fMatrix<T>::getSizeN() const
{
    return m_n;
}



template<typename T>
bool fMatrix<T>::print()
{
	for(unsigned int i = 0; i<m_m; i++)
	{
		for(unsigned int j = 0; j<m_n; j++)
		{
			cout << m_mat[i][j] << " ";
		}
		cout << endl; 
	}
	cout << endl;
	return true;
}

template<typename T>
bool fMatrix<T>::spyPrint()
{
	char disp(' ');
	for(unsigned int i = 0; i<m_m; i++)
	{
		for(unsigned int j = 0; j<m_n; j++)
		{
			if (m_mat[i][j] == 0)
			{
				disp = ' ';
			}
			else 
			{
				disp = '.';
			}
			cout << disp;
		}
		cout << endl; 
	}
	cout << endl;
	return true;
}

template<typename T>
bool fMatrix<T>::fill()
{
	//T count(0);
	for(unsigned int i = 0; i<m_m; i++)
	{
		for(unsigned int j = 0; j<m_n; j++)
		{
			//count++;
			m_mat[i][j]=(T)(rand() % 100) ;
			//m_mat[i][j] = count ; 
		}
	}
	return true;
}





template<typename T>
bool fMatrix<T>::isSquare() const
{
	return m_m == m_n;
}



// ---- Operator overloading ----

// access by indices 
template<typename T>
T const & fMatrix<T>::operator()(std::size_t x, std::size_t y) const
{
	if ((x >= m_m) || (y >= m_n))
	{
		cout << endl << "WARNING incorrect access to matrix of size " << m_m << "*" << m_n << endl;
		cout << "you asked position " << x << "*" << y << endl << endl;
		assert(((x < m_m) && (y < m_n)));
	}
	return m_mat[x][y];
}

template<typename T>
T& fMatrix<T>::operator()(std::size_t x, std::size_t y)
{
	if ((x >= m_m) || (y >= m_n))
	{
		cout << endl << "WARNING incorrect access to matrix of size " << m_m << "*" << m_n << endl;
		cout << "you asked position " << x << "*" << y << endl << endl;
		assert(((x < m_m) && (y < m_n)));
	}
    return m_mat[x][y];
}


template<typename T>
fMatrix<T> operator*(fMatrix<T> const & leftM, fMatrix<T> const & rightM)
{
	return leftM.rightMult(rightM);
}

template<typename T>
fMatrix<T> fMatrix<T>::rightMult(fMatrix<T> const& rightM) const
{
	T sum(0);
	fMatrix<T> leftM;
	leftM = *this;
    unsigned int m1(leftM.getSizeM());
    unsigned int m2(rightM.getSizeM());
    unsigned int n1(leftM.getSizeN());
    unsigned int n2(rightM.getSizeN());
	fMatrix<T> res(m1, n2);
	
	if(n1!=m2)
	{
		cout << "impossible operation. leftM is of size (" << m1 << "," << n1 << "); rightM (" << m2 << "," << n2 << ")." << endl;
		assert(n1==m2);
	}
    
	for (unsigned int i = 0 ; i < m1 ; i++)
	{
		for(unsigned int j = 0; j < n2 ; j++)
		{
			sum = 0;
			for(unsigned int k = 0; k < n1 ; k++)
			{
				sum+= leftM(i,k)*rightM(k,j);
			}
			res(i,j)=sum;
		}
	}

    return res;
	
}


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

template<typename T>
fMatrix<T> operator*(T const& scal, fMatrix<T> const& rightM)// left scalar multiplication
{
    //fMatrix<T> res(rightM.m_m, rightM.m_n);
	//
	//for (unsigned int i = 0 ; i < res.m_m; i++)
	//{
	//	for(unsigned int j = 0; j < res.m_n; j++)
	//	{
	//		res(i,j)=scal*rightM(i,j);
	//	}
	//}

    return rightM.scalMult(scal);
}

template<typename T>
fMatrix<T> fMatrix<T>::scalMult( T const& scal) const
{
    fMatrix<T> res(m_m, m_n);
	
	for (unsigned int i = 0 ; i < m_m; i++)
	{
		for(unsigned int j = 0; j < m_n; j++)
		{
			res(i,j)=scal*m_mat[i][j]; //(i,j);
		}
	}
	return res;
}
		
template<typename T>
fMatrix<T> operator*(fMatrix<T> const & leftM, T const& scal) //right scalar multiplication
{
	return scal*leftM;
}

template<typename T>
fMatrix<T> operator+(fMatrix<T> const& leftM, fMatrix<T> const& rightM)
{
	return leftM.add(rightM);
}
template<typename T>
fMatrix<T> operator+(fMatrix<T> const & leftM, T const & alpha)
{
	return leftM.add(alpha);
}

template<typename T>
fMatrix<T> fMatrix<T>::add(fMatrix<T> const& rightM) const
{
    fMatrix<T> res(m_m, m_n);
	
	assert(m_n==rightM.getSizeN());
	assert(m_m==rightM.getSizeM());
    
	for (unsigned int i = 0 ; i < m_m; i++)
	{
		for(unsigned int j = 0; j < m_n; j++)
		{
			res(i,j)=m_mat[i][j]+rightM(i,j);
		}
	}

    return res;
}
template<typename T>
fMatrix<T> fMatrix<T>::add(T const& alpha) const
{
    fMatrix<T> res(m_m, m_n);
	
	for (unsigned int i = 0 ; i < m_m; i++)
	{
		for(unsigned int j = 0; j < m_n; j++)
		{
			res(i,j)=m_mat[i][j]+alpha;
		}
	}

    return res;
}

template<typename T>
fMatrix<T> fMatrix<T>::t() const
{ // transposition
    fMatrix<T> M(m_n, m_m);

    for(unsigned int i=0; i < m_m; ++i)
    {
        for(unsigned int j=0; j < m_n; ++j)
        {
            M(j,i) = this->m_mat[i][j];
        }
    }

    return M;
}

template<typename T>
fMatrix<T> fMatrix<T>::submat(unsigned int a, unsigned int b, unsigned int c, unsigned int d) const 
{
	// return a subset of m_mat : lines a to b and columns c to d
	assert(a <= b);
	assert(c <= d);
	assert(b < m_m);
	assert(d < m_n);
	
	fMatrix m(b-a+1, d-c+1);
	
	unsigned int ci(0);
	unsigned int cj(0);

	for(unsigned int i = a; i<=b; i++)
	{
		cj = 0;
		for(unsigned int j = c; j<=d ; j++)
		{
			m(ci, cj) = m_mat[i][j];
			cj++;
		}
		ci++;
	}
	return m;
}

template<typename T>
fMatrix<T> fMatrix<T>::diag() const
{
	assert(m_n==m_m);
	fMatrix diag(m_n, 1);
	for(unsigned int i = 0 ; i < m_m ; i++)
	{
		diag(i,0) = m_mat[i][i];
	}
	return diag;
}


template<typename T>
fMatrix<T> fMatrix<T>::swapLines(vector<int> perm) const
{
	// reorganizes the lines of the matrix
	if (perm.size()!= m_m)
	{
		cout << "perm.size() = " << perm.size() << " and m_m = " << m_m << endl;
		assert(perm.size() == m_m);
	}
	fMatrix<T> v(m_m, m_n);
	unsigned int newI;
	for(unsigned int i = 0 ; i < m_m ; i++)
	{
		newI = perm[i];
		assert(newI<m_m);
		for(unsigned int j = 0 ; j < m_n ; j++)
		{
			v(i, j) = m_mat[newI][j];
		}
	}
	return v;
}

//template<typename T>
//fMatrix<T> fMatrix<T>::swapCols(vector<int> perm) const
//{
//	// reorganizes the columns of the matrix
//	// IT HAS NOT BEEN TESTED !! USE CAREFULLY
//	assert(1==0);
//	if (perm.size()!= m_n)
//	{
//		cout << "perm.size() = " << perm.size() << " and m_n = " << m_n << endl;
//		assert(perm.size() == m_n);
//	}
//	fMatrix<T> v(m_m, m_n);
//	unsigned int newI;
//	for(unsigned int i = 0 ; i < m_n ; i++)
//	{
//		newI = perm[i];
//		assert(newI<m_n);
//		for(unsigned int j = 0 ; j < m_m ; j++)
//		{
//			v(j, i) = m_mat[j][newI];
//		}
//	}
//	return v;
//}

template<typename T>
T fMatrix<T>::trace() const
{
	assert((m_m == m_n) && "trace() can be used only on a square matrix");
	T prod(1);
	for(unsigned int i = 0; i < m_m ; i++)
	{
		prod*=m_mat[i][i];
	}
	return prod;
}




template<typename T>
T fMatrix<T>::det() const
{
	// computation from Tr(L)*Tr(U)
	assert((m_m == m_n) && "det() can be used only on a square matrix");
	fMatrix<T> l(m_m, m_m);
	fMatrix<T> u(m_m, m_m);
	luFact(&l,&u);

	T deter(0);
	if(m_m > 3)
	{
		return l.trace()*u.trace();	
	}
	if(m_m == 1)
	{
		return m_mat[0][0];
	}
	if(m_m == 2)
	{
		return (m_mat[0][0]*m_mat[1][1])-(m_mat[0][1]*m_mat[1][0]);
	}
	if(m_m == 3)
	{
		deter = (m_mat[0][0]*((m_mat[1][1]*m_mat[2][2])-(m_mat[2][1]*m_mat[1][2])))-(m_mat[1][0]*((m_mat[0][1]*m_mat[2][2])-(m_mat[2][1]*m_mat[0][2])))+(m_mat[2][0]*((m_mat[0][1]*m_mat[1][2])-(m_mat[1][1]*m_mat[0][2])));
		return deter;
	}
	if(m_m == 0)
	{// who should require the determinant of an empty matrix ?!? 
		return 0;
	}
	return 0;// otherwise le control il would reach la end of une non-void function....
}


template<typename T>
T fMatrix<T>::getFemDetJac() const
{
	//jacobian matrices may be 3*3 for 3D elements, 2*3 for 2D elements, 1*3 for 1D elements
	//
	assert(m_m < 4);
	assert(m_m > 0);
	assert(m_n == 3);

	if (m_m == 3)
	{
		return this->det();
	}
	else if (m_m == 2)
	{
		// norm(cross(JacG(1,:), JacG(2,:)))
		fMatrix a(3,1);
		a(0,0) = (m_mat[1][0]*m_mat[2][1])-(m_mat[2][0]*m_mat[1][1]);
		a(1,0) = (m_mat[0][1]*m_mat[2][0])-(m_mat[2][1]*m_mat[0][0]);
		a(2,0) = (m_mat[0][0]*m_mat[1][1])-(m_mat[1][0]*m_mat[0][1]);
		return a.norm2();
	}
	else if (m_m == 1)
	{
		return sqrt(pow(m_mat[0][0],2) + pow(m_mat[0][1], 2) + pow(m_mat[0][2], 2));
	}
	else
	{
		cout << "strange ! I should never have gotten here !" << endl;
		WHEREAMI
		return 0;
	}
}



template<typename T>
T fMatrix<T>::norm2()
{
	// il faudrait le decouper en sous fonctions... 
	assert(m_n == 1);
	double sum(0);
	for(unsigned int i = 0 ; i < m_m ; i++)
	{
		sum += pow(m_mat[i][0],2);
	}
	return sqrt(sum);
}


template<typename T>
T fMatrix<T>::sum() const // somme de tous les termes 
{
	T sum(0);
	for(unsigned int i=0 ; i < m_m; i++)
	{
		for(unsigned int j = 0 ; j < m_n ; j++)
		{
			sum+=m_mat[i][j];
		}
	}
	return sum;
}

template<typename T>
fMatrix<T> fMatrix<T>::inv() const // only for matrices or size 2 and 3 for now ;) 
{
	assert(m_m < 4);
	assert(m_m == m_n);

	fMatrix inv(m_m, m_m);
	if(m_m == 2)
	{
		inv(0,0) = m_mat[1][1];
		inv(1,0) = -m_mat[1][0];
		inv(0,1) = -m_mat[0][1];
		inv(1,1) = m_mat[0][0];
		inv = inv*(1/(this->det()));
	}
	else
	{
		inv(0,0) = (m_mat[1][1]*m_mat[2][2]-m_mat[1][2]*m_mat[2][1]);
		inv(1,0) = (m_mat[1][2]*m_mat[2][0]-m_mat[1][0]*m_mat[2][2]);
		inv(2,0) = (m_mat[1][0]*m_mat[2][1]-m_mat[1][1]*m_mat[2][0]);
		inv(0,1) = (m_mat[0][2]*m_mat[2][1]-m_mat[0][1]*m_mat[2][2]);
		inv(1,1) = (m_mat[0][0]*m_mat[2][2]-m_mat[0][2]*m_mat[2][0]);
		inv(2,1) = (m_mat[0][1]*m_mat[2][0]-m_mat[0][0]*m_mat[2][1]);
		inv(0,2) = (m_mat[0][1]*m_mat[1][2]-m_mat[0][2]*m_mat[1][1]);
		inv(1,2) = (m_mat[0][2]*m_mat[1][0]-m_mat[0][0]*m_mat[1][2]);
		inv(2,2) = (m_mat[0][0]*m_mat[1][1]-m_mat[0][1]*m_mat[1][0]);	
		inv = inv*(1/(this->det()));
	}
	return inv;
}

template<typename T>
bool fMatrix<T>::luFact(fMatrix<T> *l, fMatrix<T> *u) const
{
	// l and u contents are replaced by the factorisation of the current matrix. Useful to have it in this class, especially for determinant calculation	
	T sum(0);
	assert(m_m==m_n);
	int m_size(m_m);
	assert(isSquare() && "LU factorisation can only be performed for square matrices");
	
	for(int i = 0; i < m_size-1; i++)	
	{
		for(int j = i ; j < m_size ; j++)
		{
			sum = 0;
			for(int k = 0; k < i; k++)
			{
				sum += (*l)(i,k) * (*u)(k,j);
			}
			(*u)(i,j) = m_mat[i][j] - sum;
		}
		for(int j = i+1 ; j < m_size ; j++)
		{
			sum = 0;
			for(int k = 0; k < i; k++)
			{
				sum += (*l)(j,k) * (*u)(k,i);
			}
			(*l)(j,i) = 1/(*u)(i,i)*(m_mat[j][i] - sum);
		}
	}
	sum = 0;
	for(int k = 0 ; k < m_size-1 ; k++)
	{
		sum+=(*l)(m_size-1,k)*(*u)(k,m_size-1);
	}
	(*u)(m_size-1,m_size-1) = m_mat[m_size-1][m_size-1] - sum;
	for(int i = 0 ; i < m_size ; i++)
	{
		(*l)(i,i) = 1;
	}
	return true;
}






// ---- Basic matrices ----
template<typename T>
bool fMatrix<T>::setOne()
{
	for(unsigned int i = 0 ; i < m_m ; i++)
	{
		for(unsigned int j = 0 ; j < m_n ; j++)
		{
			m_mat[i][j] = 1;
		}
	}
	return true;
}
template<typename T>
bool fMatrix<T>::setZero()
{
	for(unsigned int i = 0 ; i < m_m ; i++)
	{
		for(unsigned int j = 0 ; j < m_n ; j++)
		{
			m_mat[i][j] = 0;
		}
	}
	return true;
}
template<typename T>
bool fMatrix<T>::setId()
{
	if(m_m != m_n)
	{
		cout << "the matrix is not square, I cannot set it to identity" << endl;
		return false; 
	}
	else 
	{
		for(unsigned int i = 0 ; i < m_m ; i++)
		{
			for(unsigned int j = 0 ; j < m_m ; j++)
			{
				m_mat[i][j] = 0;
			}
		}
		for(unsigned int i = 0 ; i < m_m ; i++)
		{
			m_mat[i][i] = 1;
		}
		return true;
	}
}























//template<>
//fMatrix<double> operator=(fMatrix<float> f)
//{
//	M = f.getSizeM();
//	N = f.getSizeN();
//	delete(this);
//	this = new fMatrix<double>(M, N);
//	for (unsigned int = 0 : i < M ; i++)
//	{
//		for(unsigned int j = 0; j < N ; j++)
//		{
//			m_mat[i][j] = f(i,j);
//		}
//	}
//	return *this;
//}










#endif

