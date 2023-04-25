#ifndef DEF_FMATRIX
#define DEF_FMATRIX

#include <vector>

using namespace std;

// un peu d'aide trouvée ici : https://zestedesavoir.com/tutoriels/822/la-programmation-en-c-moderne/la-programmation-orientee-objet/t-p-entrons-dans-la-matrice/#3-correction-detaillee

class fMatrix
{
public: 	
	fMatrix();
	fMatrix(int m, int n);
	~fMatrix();
	bool fill();
	bool print();
	int getSizeM();
	int getSizeN();
	bool getIsSquare();

	// ---- Operator overloading ----

	// operateur indice, accès en lecture
	double const & operator()(std::size_t x, std::size_t y) const;
	// Version d'accès en écriture.
	double& operator()(std::size_t x, std::size_t y);

	// basic operations 
	friend fMatrix operator*(fMatrix const & leftM, fMatrix const & rightM);
	friend fMatrix operator*(double const & scal, fMatrix const & rightM); // scalar multiplication
	friend fMatrix operator+(fMatrix const & leftM, fMatrix const & rightM);

private: 
	int const m_m;
	int const m_n;
	vector<vector<double>> m_mat;
};

#endif

/*

*/
