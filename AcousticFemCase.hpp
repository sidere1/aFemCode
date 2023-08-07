
#ifndef DEF_ACOUSTICFEMCASE
#define DEF_ACOUSTICFEMCASE

//#include "Mesh.h" // already included via Setup.h
//#include "Node.h"//idem 
//#include "PointLoad.h"
// #include <fstream>
// #include <iostream>
// #include <cassert>
// #include <sys/stat.h>
// #include "Setup.hpp"
//#include "Eigen::SparseMatrix.h"
// #include <Eigen/Sparse>
// #include <vector> 
// #include <complex>
// #include "fLinSys.hpp"
#include "FemCase.hpp"

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

// #include <Eigen/Sparse>

/*! \brief 
* AcousticFemCase is the base class for an acoustic run, and inherits from FemCase 
 * in the setup file, it is good practice to put first the largest domain (containing the most nodes). But it should work either way. 
 * 
*/
template <typename T>
class AcousticFemCase: public FemCase<T> 
{
public:
    AcousticFemCase();
    AcousticFemCase(std::string setupFile);
    ~AcousticFemCase();

	bool buildF();
	virtual bool performResolution(); // solve 

protected:
};

template <typename T>
AcousticFemCase<T>::AcousticFemCase()
{
	this->m_loaded = false;
}

template <typename T>
AcousticFemCase<T>::AcousticFemCase(std::string setupFile): FemCase<T>(setupFile)
{
}


template <typename T>
AcousticFemCase<T>::~AcousticFemCase()
{

}

template <typename T>
bool AcousticFemCase<T>::performResolution()
{
	int iC(0); // normally, in this class there should be only one coupling 
	const complex<double> i(0.0,1.0); // pas complex<T> ? chuis surpris ! 
	const double pi(3.1415926);	

	assert(this->m_nCoupling == 1 && "Not allowed yet. Don't hesitate to develop it !");
	fLinSys<T> *linSys;
	linSys = new fLinSys<T>(0,0);
	double k(0);
	int nNodes(this->m_mesh[0]->getNodesNumber());
	vector<double> frequencies(this->m_setup[0]->getFrequencies());
	this->currentSys[iC] = new Eigen::SparseMatrix<T>(nNodes, nNodes);

	// preparing results 
	vector<int> mics = this->m_setup[0]->getMics();
	unsigned int nMics(mics.size());
	this->writeMicValuesHeader();	
	vector<T> values;

	bool writeVtkOnce(false);
	ostringstream vtkfilename;
	if(writeVtkOnce){
		vtkfilename << "pressure_allFreq.vtk";
		this->writeVtkMesh(vtkfilename.str(), this->m_mesh[0]);
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
			k = 2*pi*frequencies[iFreq]/this->m_setup[0]->getC();
			*this->currentSys[iC] = *this->m_Ksurf[iC]+(-1)*(k*k)*(*this->m_Msurf[iC])-i*k*(*this->m_Mseg[iC]);
			delete linSys;
			linSys = new fLinSys<T>(*this->currentSys[iC], *this->m_Fsurf[iC]);
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
				this->writeVtkMesh(vtkfilename.str(), this->m_mesh[0]);
				firstTime=true;
			}

			if(writeVtkOnce){dataName.str(""); dataName << "sol_abs_f_" << abs(frequencies[iFreq]);}
			else{dataName.str(""); dataName << "sol_abs";}
			this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime, this->m_mesh[0]);
			firstTime = false;
			if(writeVtkOnce){dataName.str(""); dataName << "sol_real_f_" << abs(frequencies[iFreq]);}
			else{dataName.str(""); dataName << "sol_real";}
			//dataName.str(""); dataName << "sol_real_f_" << real(frequencies[iFreq]);
			this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime, this->m_mesh[0]);
			if(writeVtkOnce){dataName.str(""); dataName << "sol_imag_f_" << abs(frequencies[iFreq]);}
			else{dataName.str(""); dataName << "sol_imag";}
			//dataName.str(""); dataName << "sol_imag_f_" << real(frequencies[iFreq]);
			this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime, this->m_mesh[0]);
		}
	}
	return true;
}


template <typename T>
bool AcousticFemCase<T>::buildF()
{
	size_t iC(0); // there should be only one coupling in this class 
	int nN(this->m_mesh[0]->getNodesNumber()); 
	this->m_Fvol[iC] = new Eigen::SparseMatrix<T>(nN,1);
	this->m_Fsurf[iC] = new Eigen::SparseMatrix<T>(nN,1);
	this->m_Fseg[iC] = new Eigen::SparseMatrix<T>(nN,1);


	std::vector<Eigen::Triplet<T>> coefficients;
	coefficients.push_back(Eigen::Triplet<T>(254,0,1));

	this->m_Fvol[iC]->setFromTriplets(coefficients.begin(), coefficients.end());
	this->m_Fsurf[iC]->setFromTriplets(coefficients.begin(), coefficients.end());
	this->m_Fseg[iC]->setFromTriplets(coefficients.begin(), coefficients.end());

	// (*m_Fvol)(254,0) = 1;
	// (*m_Fsurf)(254,0) = 1;
	// (*m_Fseg)(254,0) = 1;
	return true; 
}



#endif
