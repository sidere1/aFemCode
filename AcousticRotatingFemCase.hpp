
#ifndef DEF_ACOUSTICROTATINGFEMCASE
#define DEF_ACOUSTICROTATINGFEMCASE

#include "FemCase.hpp"

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
	bool performResolution();
	bool writeVtk();
	bool checkAndExport(); // temp data for basic checks, groups, nodes ids, interface locations, etc. 
	bool buildPhiF();
	bool buildPhiR();
	bool checkInterfaceNameConsistency(size_t iC);
	vector<double> createRotatingFrequencies(vector<double> frequencies);


protected:
	// size_t m_eta;
	// size_t m_N;
	// size_t m_L;
	// size_t C;
	// double m_Omega;
	Eigen::SparseMatrix<T> m_coupledSystem; 
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> m_PhiR; 
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> m_PhiF; 
	vector<size_t> m_gammaR;
	vector<size_t> m_gammaF;
	vector<double> m_rR;
	vector<double> m_rF;
	vector<double> m_thetaR;
	vector<double> m_thetaF;
	size_t m_nNGammaR;
	size_t m_nNGammaF;

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
		std::cout << "You should not be using " << this->m_nCoupling << " coupling in a coupled rotating FSBC case. Anything else than 2 is prohibited" << std::endl; 
	}
	assert(this->m_nCoupling == 2); // ou plus si on veut mettre plusieurs zones tournantes 


	//////// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	// A AJOUTER : GENERATION DES FREQUENCES 
	// verification à faire : 
	// * que les deux setups soient bien les mêmes 
	// * taille de système acceptable ? nombre de fréquences cohérent par rapport à la bande demandée, etc. 
	// * 

}


template <typename T>
AcousticRotatingFemCase<T>::~AcousticRotatingFemCase()
{

}

template <typename T>
bool AcousticRotatingFemCase<T>::performResolution()
{
	if (this->m_nCoupling != 2)
	{
		cout << "You should not be using " << this->m_nCoupling << " coupling in a coupled rotating FSBC case. Anything else than 2 is prohibited" << std::endl; 
		return false; 
	}
	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	// a ajouter : un check que les deux setup matchent parfaitement sur tous les paramètres. Notamment les fréquences 

	const double pi(3.1415926);	
	size_t totalNodesNumber(0);
	vector<double> frequencies(this->m_setup[0]->getFrequencies());
	size_t nF(0);
	vector<size_t> nodeGroupTemp;
	vector<size_t> elemGroupTemp;
	vector<int> nodesTemp;
	vector<double> radiusTemp; 
	vector<double> thetaTemp; 
	double x;
	double y;
	double r;
	double theta;
	ostringstream vtkfilename;

	m_nNGammaR = 0;
	m_nNGammaF = 0;

	frequencies = createRotatingFrequencies(frequencies);
	nF = frequencies.size();

	for(size_t iC = 0; iC < this->m_nCoupling ; iC ++)
	{
		radiusTemp.clear();
		nodesTemp.clear();
		nodeGroupTemp.clear();
		elemGroupTemp.clear();

		// calcul du nombre total de noeuds
		totalNodesNumber+=this->m_mesh[iC]->getNodesNumber();

		// vérification que les noms de groupe matchent 
		if (!checkInterfaceNameConsistency(iC))
		{
			cout << "Interface name " << this->m_setup[iC]->getInterfaceName() << " does not match any group name in the mesh " << endl;
			WHEREAMI
			return false; 
		}
		
		// construction de la liste des noeuds et de leurs coordonnées r 
		elemGroupTemp = this->m_mesh[iC]->getGroup(this->m_setup[iC]->getInterfaceGroup());
		for (size_t iElem = 0; iElem < elemGroupTemp.size(); ++ iElem)
		{
			nodesTemp = this->m_mesh[iC]->getElement(elemGroupTemp[iElem]).getNodesIds();
			for (size_t iN = 0; iN < nodesTemp.size(); ++iN)
				nodeGroupTemp.push_back(abs(nodesTemp[iN]));
		}
		removeDoublons(nodeGroupTemp);
		radiusTemp.resize(nodeGroupTemp.size());
		thetaTemp.resize(nodeGroupTemp.size());
		for (size_t iNode = 0 ; iNode < nodeGroupTemp.size() ; ++iNode)
		{
			x = this->m_mesh[iC]->getNode(nodeGroupTemp[iNode]).getX();
			y = this->m_mesh[iC]->getNode(nodeGroupTemp[iNode]).getY();
			r = sqrt(pow(x,2)+pow(y,2));
			if (y < 0)
				theta = acos(x/r);
			else
				theta = -acos(x/r);
			radiusTemp[iNode] = r;
			thetaTemp[iNode] = theta;
		}

		// renumbering mesh in order to 
		if (!this->m_mesh[iC]->renumberMesh(nodeGroupTemp))
		{
			cout << "error while renumbering the mesh";
			WHEREAMI
			return false; 
		}
		// cout << "Mesh renumbered successfully !!!" << endl;

		// since the mesh is renumbered, the is no need to store nodeGroupTemp anymore...
		for (size_t iNode = 0; iNode < nodeGroupTemp.size(); ++iNode)
		{
			nodeGroupTemp[iNode] = iNode;
		}

		// après ça, normalement gammaR et gammaF c'est 1:nNGammaAlpha
		if (this->m_setup[iC]->getRotating())
		{
			cout << "group " << iC << " " << this->m_mesh[iC]->getGroupNames()[this->m_setup[iC]->getInterfaceGroup()] << " is rotating" << endl;
			m_gammaR = nodeGroupTemp;
			m_nNGammaR = nodeGroupTemp.size();
			m_rR = radiusTemp;
			m_thetaR = thetaTemp;
		}
		else 
		{
			cout << "group " << iC << " " << this->m_mesh[iC]->getGroupNames()[this->m_setup[iC]->getInterfaceGroup()] << " is fixed" << endl;
			m_gammaF = nodeGroupTemp;
			m_nNGammaF = nodeGroupTemp.size();
			m_rF = radiusTemp;
			m_thetaF = thetaTemp;
		}		
	}
	// exporting various vtk stuff, and checking that the setup radius matches the mesh group radius  
	checkAndExport();
	assert(m_nNGammaR != 0 && "the number of nodes on the rotating interface is null");
	assert(m_nNGammaF != 0 && "the number of nodes on the fixed interface is null");

	// building projection matrices 
	buildPhiF();
	buildPhiR();

	
	// on peut faire une matrice Magic qui touche tous les noeuds d'un coup, comme d'hab 
	// Eigen::SparseMatrix<T> Magic; les trois vecteurs, avec PhiF, ones, PhiR, ones;   
	// enfin, on passe dans la boucle sur les fréquences pour construire les systèmes globaux, projeter, et concaténer les irn jcn values. On ajoute ensuite tous les termes extra diagonaux 
	// construire le système global à partir des trois vecteurs 
	// pour le post-traitement, refaire une boucle en fréquence 
	//		faire la projection inverse, exporter tous les points micros confondus et stocker les résultats pour la suite
	// 		exporter le maillage et les données fréquentielles 
	// 		faire la transformée inverse et exporter une animation 
	std::cout << "Building a system of " << totalNodesNumber-m_nNGammaF-m_nNGammaF+2*this->m_setup[0]->getN() << " ddl by " << nF << " frequencies" << std::endl;

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
bool AcousticRotatingFemCase<T>::checkInterfaceNameConsistency(size_t iC)
{
	for (size_t iGroup = 0; iGroup < this->m_mesh[iC]->getGroupNames().size(); ++iGroup)
	{
		if (this->m_setup[iC]->getInterfaceName() == this->m_mesh[iC]->getGroupNames()[iGroup])
		{
			this->m_setup[iC]->setInterfaceGroup(iGroup);
			return true;
		}
	}
	return false;
}

template <typename T>
bool AcousticRotatingFemCase<T>::checkAndExport()
{
	// check radius/theta : vérification par rapport à la tolérance
	// export vtk : t, theta, interface group and node index
	ostringstream vtkfilename;
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> dataExport; 
	double tolerance(1e-5);
	vector<int> nodesTemp;
	bool rotating(false);
	size_t interfaceGroup(0);


	for (size_t iC = 0; iC < this->m_nCoupling; ++iC)
	{
		rotating = this->m_setup[iC]->getRotating();
		// if (rotating)
		// 	cout << "group " << iC << " is rotating " << endl;
		// else
		// 	cout << "group " << iC << " is not rotating " << endl;
		vtkfilename.str("");
		vtkfilename << "interfaceChecks" << iC << ".vtk";
		dataExport.resize(this->m_mesh[iC]->getNodesNumber(), 1);
		dataExport.setZero();
		for (size_t iNode = 0 ; iNode < m_nNGammaR ; ++iNode)
		{
			if (rotating)
			{
				dataExport(m_gammaR[iNode], 0) = (T) m_rR[iNode];
				if (abs(m_rR[iNode]-this->m_setup[iC]->getRadius()) > tolerance)
				{
					cout << "in rotating group, coupling " << iC << ", node" << m_gammaR[iNode] << " is at r = " << m_rR[iNode] << " instead of " << this->m_setup[iC]->getRadius() << endl;
					return false; 
				}
			}
			else 
			{
				dataExport(m_gammaF[iNode], 0) = (T) m_rF[iNode];
				if (abs(m_rF[iNode]-this->m_setup[iC]->getRadius()) > tolerance)
				{
					cout << "in fixed group, coupling " << iC << ", node" << m_gammaF[iNode] << " is at r = " << m_rF[iNode] << " instead of " << this->m_setup[iC]->getRadius() << endl;
					return false; 
				}
			}
		}
		this->writeVtkMesh(vtkfilename.str(), this->m_mesh[iC]);
		this->writeVtkData(vtkfilename.str(), "radius", dataExport.sparseView(), true, this->m_mesh[iC]);
		// exporting theta 
		dataExport.setZero();
		for (size_t iNode = 0 ; iNode < m_nNGammaR ; ++iNode)
		{
			if (rotating)
				dataExport(m_gammaR[iNode], 0) = (T) m_thetaR[iNode];
			else 
				dataExport(m_gammaF[iNode], 0) = (T) m_thetaF[iNode];
		}
		this->writeVtkData(vtkfilename.str(), "theta", dataExport.sparseView(), false, this->m_mesh[iC]);
		// export group 
		dataExport.setZero();
		interfaceGroup = this->m_setup[iC]->getInterfaceGroup();
		for (size_t iElem = 0; iElem < this->m_mesh[iC]->getGroup(interfaceGroup).size(); ++iElem)
		{
			nodesTemp = this->m_mesh[iC]->getElement(this->m_mesh[iC]->getGroup(interfaceGroup)[iElem]).getNodesIds();
			for (size_t iN = 0; iN < nodesTemp.size(); ++iN)
				dataExport(nodesTemp[iN], 0) = (T) 1;
		}
		this->writeVtkData(vtkfilename.str(), "interfaceGroup", dataExport.sparseView(), false, this->m_mesh[iC]);
		// export indices 
		dataExport.setZero();
		for (int iNode = 0; iNode < this->m_mesh[iC]->getNodesNumber(); ++ iNode)
		{
			dataExport(iNode, 0) = (T) iNode;
		}
		this->writeVtkData(vtkfilename.str(), "index", dataExport.sparseView(), false, this->m_mesh[iC]);
	}
	return true;
}

template <typename T>
bool AcousticRotatingFemCase<T>::buildPhiF()
{
	const complex<double> i(0.0,1.0);
	m_PhiF.resize(m_nNGammaF, this->m_setup[0]->getN());
	for (size_t iNode = 0; iNode < m_nNGammaF ; ++iNode)
		for(size_t n = 0; n < this->m_setup[0]->getN(); ++n)
		{
			m_PhiF(iNode, n) = std::exp(i*(T)n*(T)m_thetaF[iNode]);
		}
	return true;
}

template <typename T>
bool AcousticRotatingFemCase<T>::buildPhiR()
{
	const complex<double> i(0.0,1.0); 
	cout << "resizing of size " << m_nNGammaR << " * " << this->m_setup[0]->getN() << endl; 
	m_PhiR.resize(m_nNGammaR, this->m_setup[0]->getN());
	for (size_t iNode = 0; iNode < m_nNGammaR ; ++iNode)
		for(size_t n = 0; n < this->m_setup[0]->getN(); ++n)
			m_PhiR(iNode, n) = std::exp(i*(T)n*(T)m_thetaR[iNode]);
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

template <typename T>
vector<double> AcousticRotatingFemCase<T>::createRotatingFrequencies(vector<double> frequencies)
{
	double Omega (this->m_setup[0]->getOmega()*2*3.1415926/60); 
	double C (this->m_setup[0]->getFsbcC()); 
	double eta (this->m_setup[0]->getEta());
	size_t nF(0);
	double fmin(*min_element(frequencies.begin(), frequencies.end())), fmax(*max_element(frequencies.begin(), frequencies.end()));
	vector<double> rotFreq;
	vector<int> indices;

	int i(0);
	if (fmin-Omega/C > 0)
	{
		while (fmin-eta*Omega/C > i*Omega/C)
		{
			i++;
		}
	}
	else 
	{
		while (fmin-eta*Omega/C < i*Omega/C)
		{
			i--;
		}
		i--;
	}
	
	while(i*Omega/C < fmax+Omega/C*eta)
	{
		indices.push_back(i++);
	}
	
	nF = indices.size();
	for (size_t iFreq = 0; iFreq < nF ; ++iFreq)
	{
		rotFreq.push_back(indices[iFreq]*Omega/C);
	}
	std::cout << "New frequencies, from " << fmin << " to " << fmax << " with C = " << C << " and Omega = " << Omega << std::endl;
		for (size_t iFreq = 0; iFreq < indices.size() ; ++iFreq)
		std::cout << rotFreq[iFreq] << " ";
	std::cout << endl;

	return rotFreq;
}


#endif
