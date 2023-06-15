#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <cassert>
#include <complex>



#include "fMatrix.h"
//#include "fLinSys.h"
//#include "Node.h"
//#include "Element.h"
//#include "Mesh.h"
//#include "PointLoad.h"
#include "FemCase.h"
#include <Eigen/Dense>


using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;


int main(int argc, char** argv)
{

	unsigned int n(1000);
	MatrixXd A = MatrixXd::Random(n,n);
	VectorXd x = MatrixXd::Random(n,1);
	
	fMatrix<double> B(n,n);
	fMatrix<double> y(n,1);
	B.fill();
	y.fill();

	fLinSys<double> f(B,y);

	cout << "EEEEEEET C'EST LE TSEEEEET !!" << endl;

	// Eigen 
	cout << "Eigen fullPivLU    : " ;
	clock_t tStart = clock();
	VectorXd a = A.fullPivLu().solve(x);
	cout << clock()-tStart << endl;
	
	// Eigen 
	cout << "Eigen partialPivLU : "; 
	tStart = clock();
	a = A.partialPivLu().solve(x);
	cout << clock()-tStart << endl;
	
	// Eigen 
	cout << "Eigen : QR         : "; 
	tStart = clock();
	a = A.colPivHouseholderQr().solve(x);
	cout << clock()-tStart << endl;
	
	// Eigen 
	cout << "Eigen LDLt         : ";
	tStart = clock();
	a = A.ldlt().solve(x);
	cout << clock()-tStart << endl;
	
	// fMatrix 
	cout << "fMatrix            : " ;
	tStart = clock();
	f.solve();	
	cout << clock()-tStart << endl;
	
	// Fin du tset 
	cout << "Fin du tset." << endl;


	return 0;


	srand (time(NULL));
	cout << endl << endl; 	
	assert(argc == 2 && "please provide the name of the setup file as parameter in the command line");

	string setupFile (argv[1]);
	cout << "Reading setup file " << argv[1] << endl; 
	
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
	
	//FemCase<complex<double>> fc(setupFile.c_str());
	FemCase<double> fc(setupFile.c_str());
	if (!fc.isLoaded())
	{
		cout << "Femcase not loaded correctly" << endl;
		return 0;
	}
	fc.displayInfo();	
	fc.prepareComputation();	
	fc.buildKM(); // à remplacer par buildFLinSys 	
	fc.buildF();
	fc.performResolution();
	//unsigned int N(3);
	//unsigned int M(4);
	//fMatrix<complex<double>> a(N,N);
	//a.setId();
	//a.print();
	//fMatrix<complex<double>> b(N,M);
	//b.fill();
	//b.print();
	//(a*b).print();
	
	
	cout << endl << "Done " << endl << endl;
	return 0;
}


