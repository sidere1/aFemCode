#ifndef DEF_FMATRIX
#define DEF_FMATRIX

#include <vector>

using namespace std;

// un peu d'aide trouvée ici : https://zestedesavoir.com/tutoriels/822/la-programmation-en-c-moderne/la-programmation-orientee-objet/t-p-entrons-dans-la-matrice/#3-correction-detaillee

class fMatrix
{
public: 	
	fMatrix();
	fMatrix(unsigned int m, unsigned int n);
	~fMatrix();
	bool fill();
	bool print();
	bool spyPrint();
	unsigned int getSizeM();
	unsigned int getSizeN();
	bool isSquare() const;

	// ---- Operator overloading ----

	// operateur indice, accès en lecture
	double const & operator()(std::size_t x, std::size_t y) const;
	// Version d'accès en écriture.
	double& operator()(std::size_t x, std::size_t y);

	// basic operations 
	friend fMatrix operator*(fMatrix const & leftM, fMatrix const & rightM);
	friend fMatrix operator*(double const & scal, fMatrix const & rightM); // left scalar multiplication
	friend fMatrix operator*(fMatrix const & leftM, double const & scal); //right scalar mult
	friend fMatrix operator+(fMatrix const & leftM, fMatrix const & rightM);
	fMatrix submat(unsigned int a, unsigned int b, unsigned int c, unsigned int d) const; 

	
	// basic matrix manipulations 
	fMatrix t() const; // friend ?
   	fMatrix swapLines(vector<int> perm) const;
	double trace() const;
	double det() const; // computes the det by taking the product of the trace of the factorized LU matrices. Shitty way of doing it (with permutations it would be better...) but for small matrices it's okay :)  
	double getFemDetJac() const; // for 3D element : det. For 2D, cross prod. For 1D, L2 norm.  
	double norm2(); // L2 norm, for m*1 vectors only 
	double sum() const;
	fMatrix inv() const; // only for matrices or size 1 and 2  for now ;) 

	// ---- Basic matrices -----
	bool setOne();// matrix full of ones
	bool setZero();// matrix full of zeros
	bool setId();// Identity, ones on the diagonal for square matrices

private: 
	unsigned int m_m; // il ne faut pas les mettre en const sous peine de voir l'operateur = se faire supprimer... 
	unsigned int m_n;
	vector<vector<double>> m_mat;
};

#endif

/*

*/
