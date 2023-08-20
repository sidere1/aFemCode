/*! 
Main file
Build an object femcase, performs stuff with it. 
*/

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

// #include <iostream>
// #include <string>
// #include <vector>
#include <time.h>
// #include <cassert>
// #include <complex>
// #include <Eigen/Dense>
// #include <Eigen/Sparse>

// #include "fMatrix.h"
// #include "FemCase.hpp"
#include "AcousticFemCase.hpp"
#include "AcousticRotatingFemCase.hpp"

using namespace std;
//using Eigen::MatrixXd;
//using Eigen::VectorXd;
// typedef Eigen::SparseMatrix<double> SpMatXd;
// typedef Eigen::SparseMatrix<complex<double>> SpMatXcd;  

int main(int argc, char** argv)
{
	srand (time(NULL));
	cout << endl << endl; 	
	assert(argc == 2 && "please provide the name of the setup file as parameter in the command line");

	// string setupFile (argv[1]);
	// cout << "Reading setup file " << argv[1] << endl; 
	// AcousticFemCase<complex<double>> fc(setupFile.c_str());
	// if (!fc.isLoaded())
	// {
	// 	cout << "Femcase not loaded correctly" << endl;
	// 	return 0;
	// }
	// fc.displayInfo();	
	// fc.prepareComputation();	
	// fc.buildKM(); // à remplacer par buildFLinSys 	
	// fc.buildF();
	// fc.performResolution();
	
	// cout << endl << "Done " << endl << endl;


	Mesh m;
	m.unvImport("./example/twoElements/meshes/twoElements.unv");
	m.printCoordinates();
	m.printConnectivities();
	cout << endl << endl;
	vector<size_t> rV1;
	rV1.push_back(2);
	rV1.push_back(7);
	rV1.push_back(0);
	rV1.push_back(6);
	rV1.push_back(1);
	m.renumberMesh(rV1);
	m.printCoordinates();
	m.printConnectivities();

	// 		2	-	7	-	0				
	// 		|	\			|	
	// 		4		8		6													
	// 		|			\	|	
	// 		3	-	5	-	1													

	// 2 4 3 5 1 8
	// 1 6 0 7 2 8

	//---->

	// 		0	-	1	-	2				
	// 		|	\			|	
	// 		6		8		3													
	// 		|			\	|	
	// 		5	-	7	-	4
	
	// 0 6 5 7 4 8
	// 4 2 3 1 0 8

	// ben ça a l'air parfait ça mon gros, pourquoi le vtk marche pas ?? 
	


	// AcousticRotatingFemCase<complex<double>> fcr("/home/silouane/Documents/C++/aFemCode/example/rotatingSource/setup/main");
	
	AcousticRotatingFemCase<complex<double>> fcr("./example/rotatingSource/setup/main");
	fcr.displayInfo();
	fcr.prepareComputation();	
	fcr.performResolution();
	
	// const H5std_string FILE_NAME("deleted/data.h5");
	// const H5std_string DATASET_NAME("res");
	// const int          DIM0 = 3; // dataset dimensions
	// const int          DIM1 = 3;

	// double *data = a.data();

	// try {
    //     // Turn off the auto-printing when failure occurs so that we can
    //     // handle the errors appropriately
    //     H5::Exception::dontPrint();
	// 	H5::H5File* out_file(0);
	// 	out_file = new H5::H5File( FILE_NAME, H5F_ACC_TRUNC );

    //     // Open an existing file and dataset.
    //     // H5::H5File  file(FILE_NAME, H5F_ACC_RDWR);
    //     // H5::DataSet dataset = file.openDataSet(DATASET_NAME);

    //     // // Write the data to the dataset using default memory space, file
    //     // // space, and transfer properties.
    //     // dataset.write(data, H5::PredType::NATIVE_DOUBLE);
    // } 

	// // catch failure caused by the H5File operations
    // catch (H5::FileIException error) {
    //     error.printErrorStack();
    //     return -1;
    // }

    // // catch failure caused by the DataSet operations
    // catch (H5::DataSetIException error) {
    //     error.printErrorStack();
    //     return -1;
    // }

	return 0;
}


