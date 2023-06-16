#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <cassert>
#include <complex>
#include <Eigen/Dense>

#include "fMatrix.h"
#include "FemCase.h"


using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;


int main(int argc, char** argv)
{
	srand (time(NULL));
	cout << endl << endl; 	
	assert(argc == 2 && "please provide the name of the setup file as parameter in the command line");

	string setupFile (argv[1]);
	cout << "Reading setup file " << argv[1] << endl; 
	
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
	
	cout << endl << "Done " << endl << endl;
	return 0;
}


