/*! 
Main file
Build an object femcase, performs stuff with it. 
*/

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <cassert>
#include <complex>
#include <Eigen/Dense>
#include <Eigen/Sparse>

// #include "fMatrix.h"
#include "FemCase.h"

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

	string setupFile (argv[1]);
	cout << "Reading setup file " << argv[1] << endl; 
	
	// FemCase<double> fc(setupFile.c_str());
	// FemCase<float> fc(setupFile.c_str());
	// FemCase<complex<float>> fc(setupFile.c_str()); 
	FemCase<complex<double>> fc(setupFile.c_str());
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


	Eigen::Matrix3d a;
	a << 1,2,3,
		4,5,6,
		7,8,9;
	cout << a;
	
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


