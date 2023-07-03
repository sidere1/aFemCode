
#ifndef DEF_FEMCASE
#define DEF_FEMCASE

//#include "Mesh.h"//already included via Setup.h
#include "Node.h"
#include "PointLoad.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <sys/stat.h>
#include "Setup.h"
//#include "Eigen::SparseMatrix.h"
#include <Eigen/Sparse>
#include <vector> 
#include <complex>
#include "fLinSys.h"


#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <Eigen/Sparse>

/*! \brief 
* AcousticFemCase is the class for a non coupled Helmholtz nodal resolution. Not tested at all, just for the test of documentation 
 * 
*/
template <typename T>
class AcousticFemCase: public FemCase
{
public:
    AcousticFemCase();
    ~AcousticFemCase();
	bool performResolution(); // solve 
	
private:
};



template <typename T>
bool FemCase<T>::performResolution()
{
	const complex<double> i(0.0,1.0);
	const double pi(3.1415926);	

	assert(m_nCoupling == 1 && "Not allowed yet. Don't hesitate to develop it !");
	fLinSys<T> *linSys;
	linSys = new fLinSys<T>(0,0);
	double k(0);
	int nNodes(m_mesh[0]->getNodesNumber());
	vector<double> frequencies(m_setup[0].getFrequencies());
	currentSys = new Eigen::SparseMatrix<T>(nNodes, nNodes);

	// preparing results 
	vector<int> mics = m_setup[0].getMics();
	unsigned int nMics(mics.size());
	writeMicValuesHeader();	
	vector<T> values;

	bool writeVtkOnce(true);
	ostringstream vtkfilename;
	if(writeVtkOnce){
		vtkfilename << "pressure_allFreq.vtk";
		writeVtkMesh(vtkfilename.str());
	}
	bool firstTime(true);
	ostringstream dataName;

	if (m_couplingType[0] == 1) // ou autre chose pour de la vibration harmonique par exemple ? 
	{
		// loop over the frequencies
		for(unsigned int iFreq = 2; iFreq < frequencies.size(); iFreq ++) 
		{
			// computing solution for the current frequency 
			values.clear();
			k = 2*pi*frequencies[iFreq]/m_setup[0].getC();
			*currentSys = *m_Ksurf+(-1)*(k*k)*(*m_Msurf)-i*k*(*m_Mseg);
			delete linSys;
			linSys = new fLinSys<T>(*currentSys, *m_Fsurf);
			cout << "Solving at f = " << frequencies[iFreq] << ", k = " << k << endl;
			linSys->solve();
			// exporting results 
			for(unsigned int iMic = 0; iMic < nMics ; iMic++)
			{
				values.push_back(linSys->getSolution().coeff(mics[iMic],0));
			}
			writeMicValues(frequencies[iFreq], values);
			if(!writeVtkOnce){
				vtkfilename.str(""); vtkfilename << "pressure_f_" << abs(frequencies[iFreq]) << ".vtk";
				writeVtkMesh(vtkfilename.str());
				firstTime=true;
			}
			dataName.str(""); dataName << "sol_abs_f_" << abs(frequencies[iFreq]);
			writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
			firstTime = false;
			dataName.str(""); dataName << "sol_real_f_" << real(frequencies[iFreq]);
			writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
			dataName.str(""); dataName << "sol_imag_f_" << real(frequencies[iFreq]);
			writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
		}
	}
	return true;
}
