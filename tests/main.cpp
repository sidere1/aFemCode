#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <cassert>
#include <complex>



#include "../fMatrix.h"
#include "../fLinSys.h"
#include <Eigen/Dense>


using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;


int main(int argc, char** argv)
{
	string fileName("tests/compEigenVsfMatrix");
	cout << fileName << endl;
	ofstream resFile(fileName.c_str(), ios::app);

	if(!resFile)
	{
		return 0;
	}

	assert(argc == 2 && "please provide n as parameter in the command line");

	unsigned int n(stoi(argv[1]));
	MatrixXd A = MatrixXd::Random(n,n);
	VectorXd x = MatrixXd::Random(n,1);
	
	fMatrix<double> B(n,n);
	fMatrix<double> y(n,1);
	B.fill();
	y.fill();

	fLinSys<double> f(B,y);

	cout << "Using n = " << n << endl;
	resFile << n << " " ;

	// Eigen 
	cout << "Eigen fullPivLU    : " ;
	clock_t tStart = clock();
	VectorXd a = A.fullPivLu().solve(x);
	cout << clock()-tStart << endl;
	resFile << clock()-tStart << " " ;
	
	// Eigen 
	cout << "Eigen partialPivLU : "; 
	tStart = clock();
	a = A.partialPivLu().solve(x);
	cout << clock()-tStart << endl;
	resFile << clock()-tStart << " " ;
	
	// Eigen 
	cout << "Eigen : QR         : "; 
	tStart = clock();
	a = A.colPivHouseholderQr().solve(x);
	cout << clock()-tStart << endl;
	resFile << clock()-tStart << " " ;
	
	// Eigen 
	cout << "Eigen LDLt         : ";
	tStart = clock();
	a = A.ldlt().solve(x);
	cout << clock()-tStart << endl;
	resFile << clock()-tStart << " " ;
	
	// fMatrix 
	cout << "fMatrix            : " ;
	tStart = clock();
	f.solve();	
	cout << clock()-tStart << endl;
	resFile << clock()-tStart << endl ;
	
	// Fin du tset 
	cout << "Fin du tset." << endl;

	// à ajouter : 
	// * prendre en paramètre une taille de matrice 
	// * sortir le résultat dans un fichier texte 
	// * faire un script zsh pour lancer plein de runs et un script python pour visualiser le résultat 
	return 0;


	//unsigned int N(3);
	////unsigned int M(4);
	//fMatrix<double> a(N,N);
	//a.fill();
	//a.print();
	//cout << "det(a) = " << a.det() << endl;
	//fMatrix<double> b(N,1);
	//b.fill();
	//b.print();
	//cout << "building a linsys " << endl;
	//fLinSys<double> f(a,b);
	//f.solve();
	//cout << "Determinant " << a.det() << endl;	
	
}


