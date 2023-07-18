
#ifndef DEF_ACOUSTICROTATINGFEMCASE
#define DEF_ACOUSTICROTATINGFEMCASE

//#include "Mesh.h" // already included via Setup.h
//#include "Node.h"//idem 
//#include "PointLoad.h"
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
#include "FemCase.h"

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

#include <Eigen/Sparse>

/*! \brief 
* AcousticRotatingFemCase is the base class for an acoustic run, and inherits from FemCase 
 * 
 * 
*/
template <typename T>
class AcousticRotatingFemCase: public FemCase<T> 
{
public:
    AcousticRotatingFemCase();
    AcousticRotatingFemCase(std::string setupFile);
    ~AcousticRotatingFemCase();

	bool buildF();
	bool performResolution(); // solve 

protected:
	int m_eta;
	int m_N;
	int m_L;
	int C;


};

template <typename T>
AcousticRotatingFemCase<T>::AcousticRotatingFemCase()
{
	this->m_loaded = false;
}

template <typename T>
AcousticRotatingFemCase<T>::AcousticRotatingFemCase(std::string setupFile): FemCase<T>(setupFile)
{
	assert m_nCoupling == 2; // ou plus si on veut mettre plusieurs zones tournantes 
}


template <typename T>
AcousticRotatingFemCase<T>::~AcousticRotatingFemCase()
{

}

template <typename T>
bool AcousticRotatingFemCase<T>::performResolution()
{
	const complex<double> i(0.0,1.0); // pas complex<T> ? chuis surpris ! 
	const double pi(3.1415926);	

	assert(this->m_nCoupling == 1 && "Not allowed yet. Don't hesitate to develop it !");
	fLinSys<T> *linSys;
	linSys = new fLinSys<T>(0,0);
	double k(0);
	int nNodes(this->m_mesh[0]->getNodesNumber());
	vector<double> frequencies(this->m_setup[0].getFrequencies());
	this->currentSys = new Eigen::SparseMatrix<T>(nNodes, nNodes);

	// preparing results 
	vector<int> mics = this->m_setup[0].getMics();
	unsigned int nMics(mics.size());
	this->writeMicValuesHeader();	
	vector<T> values;

	bool writeVtkOnce(false);
	ostringstream vtkfilename;
	if(writeVtkOnce){
		vtkfilename << "pressure_allFreq.vtk";
		this->writeVtkMesh(vtkfilename.str());
	}
	bool firstTime(true);
	ostringstream dataName;

	if (this->m_couplingType[0] == 1) // ou autre chose pour de la vibration harmonique par exemple ? 
	{
		// loop over the frequencies
		for(unsigned int iFreq = 2; iFreq < frequencies.size(); iFreq ++) 
		{
			// computing solution for the current frequency 
			values.clear();
			k = 2*pi*frequencies[iFreq]/this->m_setup[0].getC();
			*this->currentSys = *this->m_Ksurf+(-1)*(k*k)*(*this->m_Msurf)-i*k*(*this->m_Mseg);
			delete linSys;
			linSys = new fLinSys<T>(*this->currentSys, *this->m_Fsurf);
			cout << "Solving at f = " << frequencies[iFreq] << ", k = " << k << endl;
			linSys->solve();
			// exporting results 
			for(unsigned int iMic = 0; iMic < nMics ; iMic++)
			{
				values.push_back(linSys->getSolution().coeff(mics[iMic],0));
			}
			this->writeMicValues(frequencies[iFreq], values);
			if(!writeVtkOnce){
				vtkfilename.str(""); vtkfilename << "pressure_f_" << abs(frequencies[iFreq]) << ".vtk";
				this->writeVtkMesh(vtkfilename.str());
				firstTime=true;
			}

			if(writeVtkOnce){dataName.str(""); dataName << "sol_abs_f_" << abs(frequencies[iFreq]);}
			else{dataName.str(""); dataName << "sol_abs";}
			this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
			firstTime = false;
			if(writeVtkOnce){dataName.str(""); dataName << "sol_real_f_" << abs(frequencies[iFreq]);}
			else{dataName.str(""); dataName << "sol_real";}
			//dataName.str(""); dataName << "sol_real_f_" << real(frequencies[iFreq]);
			this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
			if(writeVtkOnce){dataName.str(""); dataName << "sol_imag_f_" << abs(frequencies[iFreq]);}
			else{dataName.str(""); dataName << "sol_imag";}
			//dataName.str(""); dataName << "sol_imag_f_" << real(frequencies[iFreq]);
			this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
		}
	}
	return true;
}


template <typename T>
bool AcousticRotatingFemCase<T>::buildF()
{
	int nN(this->m_mesh[0]->getNodesNumber()); 
	this->m_Fvol = new Eigen::SparseMatrix<T>(nN,1);
	this->m_Fsurf = new Eigen::SparseMatrix<T>(nN,1);
	this->m_Fseg = new Eigen::SparseMatrix<T>(nN,1);


	std::vector<Eigen::Triplet<T>> coefficients;
	coefficients.push_back(Eigen::Triplet<T>(254,0,1));

	this->m_Fvol->setFromTriplets(coefficients.begin(), coefficients.end());
	this->m_Fsurf->setFromTriplets(coefficients.begin(), coefficients.end());
	this->m_Fseg->setFromTriplets(coefficients.begin(), coefficients.end());

	// (*m_Fvol)(254,0) = 1;
	// (*m_Fsurf)(254,0) = 1;
	// (*m_Fseg)(254,0) = 1;
	return true; 
}



#endif
