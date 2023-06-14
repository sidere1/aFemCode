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

using namespace std;

int main(int argc, char** argv)
{
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


