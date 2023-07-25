
#ifndef DEF_ACOUSTICROTATINGFEMCASE
#define DEF_ACOUSTICROTATINGFEMCASE

//#include "Mesh.h" // already included via Setup.h
//#include "Node.h"//idem 
//#include "PointLoad.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <sys/stat.h>
#include "Setup.hpp"
//#include "Eigen::SparseMatrix.h"
#include <Eigen/Sparse>
#include <vector> 
#include <complex>
#include "fLinSys.hpp"
#include "FemCase.hpp"
#include <cassert>
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
	bool writeVtk();

protected:
	int m_eta;
	int m_N;
	int m_L;
	int C;
	int m_Omega;
	Eigen::SparseMatrix<T> m_coupledSystem; 
	Eigen::SparseMatrix<T> m_PhiR; 
	Eigen::SparseMatrix<T> m_PhiF; 
	Mesh * m_globalMesh; 
};

template <typename T>
AcousticRotatingFemCase<T>::AcousticRotatingFemCase()
{
	this->m_loaded = false;
}

template <typename T>
AcousticRotatingFemCase<T>::AcousticRotatingFemCase(std::string setupFile): FemCase<T>(setupFile)
{
	if (this->m_nCoupling != 2)
	{
		std::cout << "You should not be using " << this->m_nCoupling << " coupling in a coupled rotating FSBC case" << std::endl; 
	}
	assert(this->m_nCoupling == 2); // ou plus si on veut mettre plusieurs zones tournantes 
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
	size_t totalNodesNumber(0);
	size_t nNInside(0);
	size_t nNOutside(0);
	vector<double> frequencies(this->m_setup[0]->getFrequencies());
	size_t nF = frequencies.size();
	for(size_t iC = 0; iC < this->m_nCoupling ; iC ++)
	{
		totalNodesNumber+=this->m_mesh[iC]->getNodesNumber();
		if (this->m_setup[0]->getFrequencies().size() != nF)
		{
			std::cout << "MAIS N IMPORTE QUOI TOI, D OU TU METS PAS LES MEMES FREQUENCES PARTOUT" << std::endl;
			return false; 
		}
	}
	
	// faut compter et repérer les noeuds sur l'interface inside, sur l'interface outside pour initialiser la matrice globale correctement, ainsi que les vecteurs d'indices et valeurs pour construire la matrice sparse  
	// faut ensuite construire les matrices de projection. 
	// on peut faire une matrice Magic qui touche tous les noeuds d'un coup, comme d'hab 
	// enfin, on passe dans la boucle sur les fréquences pour construire les systèmes globaux, projeter, et concaténer les irn jcn values. On ajoute ensuite tous les termes extra diagonaux 
	// construire le système global à partir des trois vecteurs 
	// pour le post-traitement, refaire une boucle en fréquence 
	//		faire la projection inverse, exporter tous les points micros confondus et stocker les résultats pour la suite
	// 		exporter le maillage et les données fréquentielles 
	// 		faire la transformée inverse et exporter une animation 
	std::cout << "Building a system of environ " << totalNodesNumber << " ddl by " << nF << " frequencies" << std::endl;

	ostringstream vtkfilename;
	// this->m_mesh[0]->displayInfo();
	// this->m_mesh[1]->displayInfo();
	m_globalMesh = new Mesh(*(this->m_mesh[0]), *(this->m_mesh[1]));
	// globalMesh.displayInfo();
	vtkfilename.str(""); vtkfilename << "meshesConcatenated" << ".vtk";
	this->writeVtkMesh(vtkfilename.str(), m_globalMesh);
	
	// m_coupledSystem = Eigen::SparseMatrix<T>(nF*totalNodesNumber, nF*totalNodesNumber);

	// for(size_t iC = 0; iC < this->m_nCoupling ; iC ++)
	// {
	// 	fLinSys<T> *linSys;
	// 	linSys = new fLinSys<T>(0,0);
	// 	double k(0);
	// 	int nNodes(this->m_mesh[0]->getNodesNumber());
		
	// 	this->currentSys[iC] = new Eigen::SparseMatrix<T>(nNodes, nNodes);

	// 	// preparing results 
	// 	vector<int> mics = this->m_setup[0]->getMics();
	// 	unsigned int nMics(mics.size());
	// 	this->writeMicValuesHeader();	
	// 	vector<T> values;

	// 	bool writeVtkOnce(false);
	// 	ostringstream vtkfilename;
	// 	if(writeVtkOnce){
	// 		vtkfilename << "pressure_allFreq.vtk";
	// 		this->writeVtkMesh(vtkfilename.str(), attention il manque un pointeur sur le maillage);
	// 	}
	// 	bool firstTime(true);
	// 	ostringstream dataName;

	// 	if (this->m_couplingType[0] == 1) // ou autre chose pour de la vibration harmonique par exemple ? 
	// 	{
	// 		// loop over the frequencies
	// 		for(unsigned int iFreq = 2; iFreq < nF; iFreq ++) 
	// 		{
	// 			// computing solution for the current frequency 
	// 			values.clear();
	// 			k = 2*pi*frequencies[iFreq]/this->m_setup[0]->getC();
	// 			*this->currentSys[iC] = *this->m_Ksurf[iC]+(-1)*(k*k)*(*this->m_Msurf[iC])-i*k*(*this->m_Mseg[iC]);
	// 			delete linSys;
	// 			linSys = new fLinSys<T>(*this->currentSys[iC], *this->m_Fsurf[iC]);
	// 			std::cout << "Solving at f = " << frequencies[iFreq] << ", k = " << k << std::endl;
	// 			linSys->solve();
	// 			// exporting results 
	// 			for(unsigned int iMic = 0; iMic < nMics ; iMic++)
	// 			{
	// 				values.push_back(linSys->getSolution().coeff(mics[iMic],0));
	// 			}
	// 			this->writeMicValues(frequencies[iFreq], values);
	// 			if(!writeVtkOnce){
	// 				vtkfilename.str(""); vtkfilename << "pressure_f_" << abs(frequencies[iFreq]) << ".vtk";
	// 				this->writeVtkMesh(vtkfilename.str());
	// 				firstTime=true;
	// 			}


	// 		}
	// 	}
	// }	
	
	return true;
}


template <typename T>
bool AcousticRotatingFemCase<T>::buildF()
{
	size_t iC(0); // la on ne met qu'une seule source dans le truc 
	// faire une fonction dans FemCase setDirac(icoupling, amplitude, nodeId) ? Comme ça, ça pourrait être réutilisé à la fois par AcousticFemCase et AcousticRotatingFemCase 
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

template <typename T>
bool AcousticRotatingFemCase<T>::writeVtk()
{
	// faire une boucle sur les fréquences, récupérer les données pour les deux couplings, les multiplier par m_PhiR ou m_PhiF, les concaténer 
	// faire un maillage à partir des deux maillages. Dans la classe mesh, un constructeur qui prend en paramètre plusieurs maillages ? Ce serait le plus cool... 
	// if(writeVtkOnce){dataName.str(""); dataName << "sol_abs_f_" << abs(frequencies[iFreq]);}
	// else{dataName.str(""); dataName << "sol_abs";}
	// this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
	// firstTime = false;
	// if(writeVtkOnce){dataName.str(""); dataName << "sol_real_f_" << abs(frequencies[iFreq]);}
	// else{dataName.str(""); dataName << "sol_real";}
	// //dataName.str(""); dataName << "sol_real_f_" << real(frequencies[iFreq]);
	// this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
	// if(writeVtkOnce){dataName.str(""); dataName << "sol_imag_f_" << abs(frequencies[iFreq]);}
	// else{dataName.str(""); dataName << "sol_imag";}
	// //dataName.str(""); dataName << "sol_imag_f_" << real(frequencies[iFreq]);
	// this->writeVtkData(vtkfilename.str(), dataName.str(), linSys->getSolution().block(0,0, nNodes, 1), firstTime);
	return true; 
}



#endif
