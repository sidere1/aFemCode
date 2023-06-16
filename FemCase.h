#ifndef DEF_FEMCASE
#define DEF_FEMCASE

#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <sys/stat.h>
#include "Setup.h"
#include "fMatrix.h"
#include <vector> 
#include <complex>
#include "fLinSys.h"


#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

template <typename T>
class FemCase
{
public:
    FemCase();
    FemCase(std::string setupFile);
    ~FemCase();

    int addAtribute(int cursor, std::string entry, std::string value);

    bool setInfo(std::string value);
    std::string getInfo() const;
    bool writeInfo(std::string info) const;

    bool setError(std::string value);
    std::string getError() const;
    bool writeError(std::string error) const;

    bool setMeshFile(std::string meshFile, int iCoupling);
    std::string getMeshFile(int iCoupling);


	bool displayInfo(); // for debugging purposes 
	bool isLoaded() const;
	bool prepareComputation(); // read meshn, renumber, compute volumes, aspect ratios, jacobs, etc. 
	bool buildFLinSys(); // assemblage
	bool buildKM(); 
	bool buildF();
	bool performCoupling(); // pour la projection - construction du systeme couple 
	bool performResolution(); // solve 

	// Assembly functions 
	fMatrix<T> *getGauss(int element, int order);
	fMatrix<T> *getN(int element, fMatrix<T> gp);
	fMatrix<T> *getB(int element, fMatrix<T> gp);
	fMatrix<T> computeFemB(fMatrix<T> Jac, fMatrix<T> Bref) const; 


	// postProcessing 
	bool exportResults();
	
	bool writeMicValuesHeader();
	bool writeMicValues(double f, vector<T> values);
	bool writeMicFrequencies();

	bool writeVtkMesh(string filename) const;
	bool writeVtkData(string filename, string dataName, fMatrix<T> data) const;


	// crade sa mere la race de sa grand mere 
	vector<int> swapLines(vector<int> v, vector<int> perm) const;

private:
	bool m_loaded;
	std::string m_info;
    std::string m_error;
	int m_nCoupling;
    std::string m_mainSetup;
	std::vector<std::string> m_meshFile; 
	std::vector<Mesh*> m_mesh; 
	std::vector<std::string> m_setupFile; 
	std::vector<Setup> m_setup; 
	std::vector<int> m_couplingType; // 1 : harmonic acoustics, 2 : time acoustics, ... 
	std::string m_path;
	// tout ça dans une Class storedResults ?
	// pour plusieurs couplings : un vecteur de storedResults ?  
	fMatrix<T> *m_Kvol;
	fMatrix<T> *m_Ksurf;
	fMatrix<T> *m_Kseg;
	fMatrix<T> *m_Mvol;
	fMatrix<T> *m_Msurf;
	fMatrix<T> *m_Mseg;
	fMatrix<T> *m_Fvol;
	fMatrix<T> *m_Fsurf;
	fMatrix<T> *m_Fseg;
	fMatrix<T> *currentSys;
};





template <typename T>

FemCase<T>::FemCase()
{
	m_loaded = false;
}

template <typename T>
FemCase<T>::FemCase(std::string setupFile)
{
	m_loaded = true;
	m_nCoupling = 0;
    //opening the setup file 
    string entry;
    string value;
    int skip;
    m_mainSetup = setupFile;
    ifstream setup(setupFile.c_str());
    if (setup) 
    {
        for(int cursor = 1; cursor < 4 ; cursor++)
        {// Reading header 
            setup >> entry;
			//cout << entry; 
        }
        
        for(int cursor = 1; cursor < 4 ; cursor++)
        {// Reading parameters

            setup >> entry;
            setup >> value;

			//while (entry[1] == '/')
			//{
			//	setup >> entry; 
			//	setup >> value; 
			//}

            skip = setup.tellg();

            //cout << "cursor : " << cursor << " ; entry " << entry  << " ; value " << value << endl; 
            skip = addAtribute(cursor, entry,value);
            
            for(int skipCursor = 0; skipCursor < skip; skipCursor++)
            {
                getline(setup,entry);    
                // cout << "Ignoring line " << entry << endl;
            }
            // setup.ignore();
            if (cursor == 2) 
            {
                writeInfo("Reading setup file");
            }
            
        }

        //writeInfo("Reading Mesh");
        //m_mesh = Mesh(m_info, m_error);
        //m_mesh.unvImport(getMeshFile());
        //writeInfo("Ok\n\n");

		// verifying that we are at a correct location and that all required folders exist
		auto pos1 = m_mainSetup.find_last_of('/');
		char myChar = m_mainSetup[pos1];
		assert(myChar == '/' && "expected a full path to the main setup file");
		m_path = m_mainSetup.substr(0,pos1);
		auto pos2 = m_path.find_last_of('/');
		myChar = m_mainSetup[pos2];
		assert(myChar == '/' && "expected a full path to the main setup file");
		assert (m_path.substr(pos2, pos1) == "/setup" && "expected to find the setup file in a setup folder");
		m_path = m_mainSetup.substr(0,pos2) + '/';

    	string newpath = m_path + "results";
		int status = mkdir(newpath.c_str(), 0777);
    	newpath = m_path + "temp";
		status = mkdir(newpath.c_str(), 0777);
    	newpath = m_path + "meshes";
		status = mkdir(newpath.c_str(), 0777);

		if (status == 0)
		{}// just to get rid of a warning

		// creating the setups 
		for(int i = 0; i < m_nCoupling ; i++)
		{
			cout << "Reading coupling " << i << m_path + "setup/" + m_setupFile[i] << endl;
			cout << m_path;
			Setup s(m_path + "setup/" + m_setupFile[i], m_path);
			if (s.isLoaded())
			{
				m_setup[i] = s;
			}
			else
			{
				cout << "In FemCase.h, your setup file has not been loaded correctly";
				m_loaded = false;
			} 
		}
    }
    else
    {
        cout << "Your setup file hasn't been found or opened" << endl;    
    	m_loaded = false;
	}
}

template <typename T>
FemCase<T>::~FemCase()
{

}



template <typename T>
int FemCase<T>::addAtribute(int cursor, string entry, string value) 
{// we return the number of line that should be skipped 
    string message;
    string checkEntry;
    // int count;
    unsigned int countAim(0);
    switch (cursor)
    {
        case 1 :
        {
            if (entry != "info") 
            {
                cout << "Please check your setup file. Found " << entry << " instead of info." << endl;
                message = string("Please check your setup file. Found") + entry + string( " instead of info.");
                writeError(message);
                break;
                ///////////////////////////////////////////////////////////////////////////////////////////////
                ///////////// À PROPAGER DANS LES SUIVANTS ////////////////////////////////////////////////////
                ///////////////////////////////////////////////////////////////////////////////////////////////
            }
            else
            {
                setInfo(value);
                return 0;
            }
            break;
        }
        case 2 :
        {
            if (entry != "errors") 
            {
                cout << "Please check your setup file. Found " << entry << " instead of errors." << endl;
                break;
            }
            else
            {
                setError(value);
                return 0;
            }
            break;
        }

        case 3 :
        {
            if (entry != "coupling") 
            {
                message = string("Please check your setup file. Found") + entry + string( " instead of coupling.");
                writeError(message);
				cout << message << endl; 
                break;
            }
            else
            {
                if ((atoi(value.c_str()) < 1000) && (atoi(value.c_str()) > 0)) // 1000 : arbitrary maximum value for m_nCoupling
                {
                    m_nCoupling = atoi(value.c_str());
					m_meshFile.resize(m_nCoupling);
					m_meshFile.resize(m_nCoupling);
					m_mesh.resize(m_nCoupling);
					m_setup.resize(m_nCoupling);
					m_setupFile.resize(m_nCoupling);
					m_couplingType.resize(m_nCoupling);
					
					ifstream setupFile(m_mainSetup.c_str());
					if (setupFile)
					{
						for (int i = 0; i < 10; i++)
						{
							setupFile >> entry;
							//cout << "discarded " << entry << endl;
						}
						countAim = m_nCoupling;
						for(int i = 0; i < m_nCoupling ; i++)
						{
							setupFile >> entry;
							//cout << "type " << entry << endl;
						   	//if(entry.compare("harmonic"))	
						   	if(entry == "harmonic")	
							{
								m_couplingType[i] = 1;
							}
							else if(entry == "time")
							{
								m_couplingType[i] = 2;
							}
							else
							{
								m_couplingType[i] = 0;
							}
							setupFile >> entry;
							//cout << "setup " <<  entry << endl; 
							m_setupFile[i] = entry;
							setupFile >> entry;
							//cout << "mesh " << entry << endl; 
							m_meshFile[i] = entry;
						}
					}
					else 
					{
						message = "Error while reading the couplings";
						writeError(message);
						cout << message << endl; 
					}

                    return countAim;
                }
                else
                {
                    m_nCoupling = 0;
                	message = string("Please check your setup file. Found something weird at coupling");
                	writeError(message);
                    return 0;
                }                
            }
            break;
        }

    
        default:
            message = string("Entry ")  + entry + string(" was not expected here.");
            writeError(message);
            return  0;
            break;
    }
    return 0;

}


template <typename T>
bool FemCase<T>::setMeshFile(string meshFile, int iCoupling)
{
    m_meshFile[iCoupling] = meshFile;
    ifstream isMeshFileOk(meshFile.c_str());
    if (isMeshFileOk) 
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <typename T>
string FemCase<T>::getMeshFile(int iCoupling)
{
    return m_meshFile[iCoupling];
}

template <typename T>
bool FemCase<T>::setInfo(string value)
{
    m_info = value;
    return true;
}

template <typename T>
string FemCase<T>::getInfo() const
{
    return m_info;
}

template <typename T>
bool FemCase<T>::writeInfo(string info) const
{
    string fileName = getInfo();
    ofstream infoFile(fileName.c_str(), ios::app);
    if (infoFile) 
    {
        infoFile << info;
        infoFile << endl;
        return true;     
    }
    else
    {
        return false;
    }
}

template <typename T>
bool FemCase<T>::setError(string value)
{
    m_error = value;
    return true;
}

template <typename T>
string FemCase<T>::getError() const
{
    return m_error;
}

template <typename T>
bool FemCase<T>::writeError(string error) const
{
    writeInfo(error);
    string fileName = getError();
    ofstream errorFile(fileName.c_str(), ios::app);
    if (errorFile) 
    {
        errorFile << error;
        errorFile << endl;
        return true;     
    }
    else
    {
        return false;
    }
}

template <typename T>
bool FemCase<T>::prepareComputation()
{
	//au programme : loop over the couplings 
		// creation des Meshes et read
		// renumber 
		// compute volumes, aspect ratios, jacobians 
	
	Mesh *m;
   	for(int i = 0; i < m_nCoupling; i++)
	{
		// creating mesh 
		m = new Mesh(m_info, m_error);
		m->unvImport(m_path + "meshes/"+ m_meshFile[i]);
		m_mesh[i] = m;
		// computing stuff once and for all
		// IL FAUDRAIT FAIRE UNE METHODE mesh.read() QUI DETERMINERAIT SON TYPE (unv...), L'IMPORTERAIT ET LE PREPARERAIT
		m_mesh[i]->computeAspectRatio();
		m_mesh[i]->calculateVolume();
	}

	return true; 
}

template <typename T>
bool FemCase<T>::displayInfo()
{
	cout << endl << "case composed of " << m_nCoupling << " couplings" << endl; 
	cout << "path : " << m_path << endl;  
	for (int i = 0; i < m_nCoupling; i++)
	{
		cout << i+1 << " : " << m_meshFile[i] << " ; " << m_setupFile[i] << " ; type " << m_couplingType[i] << endl;
		m_setup[i].displayInfo();	
	}
	return true; 
}

template <typename T>
bool FemCase<T>::isLoaded() const
{	
	return m_loaded;
}

template <typename T>
bool FemCase<T>::buildFLinSys()
{
	// loop over the couplings 
	// construction de K M B T F 
	switch(m_couplingType[0])
	{
		case 1:
			buildKM();
			buildF();
			break;
		default: 
			cout << "coupling type " << m_couplingType[0] << " not implemented" << endl; 
			return false; 
	}
	return true;
}


template <typename T>
bool FemCase<T>::performResolution()
{
	// check for any couplings, perform coupling. 
	// 
	// if no coupling and harmonic resolution : 
		// loop over the frequencies 
		// construction de fLinSys avec la fréquence courante 
		// résolution du systeme 
		// storage of the result 

	assert(m_nCoupling == 1 && "Not allowed yet. Don't hesitate to develop it !");
	fLinSys<T> *linSys;
	linSys = new fLinSys<T>(0,0);
	double k(0);
	int nNodes(m_mesh[0]->getNodesNumber());
	vector<double> frequencies(m_setup[0].getFrequencies());
	currentSys = new fMatrix<T>(nNodes, nNodes);

	// preparing results 
	vector<int> mics = m_setup[0].getMics();
	unsigned int nMics(mics.size());
	writeMicValuesHeader();	
	vector<T> values;

	string vtkfilename("res.vtk");
	ostringstream dataName;
	writeVtkMesh(vtkfilename);

	if (m_couplingType[0] == 1) // ou autre chose pour de la vibration harmonique par exemple ? 
	{
		// loop over the frequencies
		for(unsigned int iFreq = 2; iFreq < frequencies.size(); iFreq ++) 
		{
			values.clear();
			k = 2*3.1415926*frequencies[iFreq]/m_setup[0].getC();
			*currentSys = *m_Ksurf+(-1)*(k*k)*(*m_Msurf);
			delete linSys;
			linSys = new fLinSys<T>(*currentSys, *m_Fsurf);
			cout << "Solving at f = " << frequencies[iFreq] << ", k = " << k << endl;
			
			//cout << endl << endl << endl ;
			linSys->solve();
			cout << endl << endl << endl ;
			//cout << linSys->getSolution()(1,0) << endl;
			for(unsigned int iMic = 0; iMic < nMics ; iMic++)
			{
				//values.push_back(iMic);
				values.push_back(linSys->getSolution()(mics[iMic],0));
			}
			// writing mic pressure and vtk field for the first segment 
			writeMicValues(frequencies[iFreq], values);
			dataName.str("");
			dataName << "sol_f_" << abs(frequencies[iFreq]);
			cout << dataName.str() << endl; 
			writeVtkData(vtkfilename, dataName.str(), linSys->getSolution().submat(0,nNodes-1, 0,0));
			//writeVtkMesh(vtkfilename, "sol_f_"+(string)abs(frequencies[iFreq]), linSys->getSolution().submat(0,nNodes-1, 0,0));
		}
	}
	return true;
}


template <typename T>
bool FemCase<T>::buildKM()
{
	// au programme : 
	// loop over the couplings A CODER 
	//	 	loop over the elements to know which matrices must be initialized (et quand on bossera en sparse, pour faire un assemblage fictif et determiner la taille des matrices)
	//	 	loop over the elements
	//	 			check element type 
	//	 			compute elementary matrices K and M  
	//	 			assemble in main matrices 
	//	 	check if everything is alright (volume compared to theory)



	// variable initialisation
	int iC(0);// index of coupling => should be replaced by a loop, fixed at 0 for now 
	int nN(m_mesh[iC]->getNodesNumber());
	int nE(m_mesh[iC]->getElementNumber());
	int countSeg(0);// counts the number of segment elements
	int countSurf(0);// .. surface elements 
	int countVol(0);// .. volume elements 
	int globalI(0);
	int globalJ(0);
	Element elem;// current element
	string message("");
	vector<int> perm;
	fMatrix<T> *currentK; 
	fMatrix<T> *currentM;
   	fMatrix<T> *Ke; Ke = new fMatrix<T>(1,1); 
	fMatrix<T> *Me; Me = new fMatrix<T>(1,1);
	int np(0); // number of nodes in the current element
	int gw(0); // index of the column containing the weights 
	int ngp(0); // number of gauss points
	int nb(0); // number of lines in the gradient matrix
	double detJac(0);
	fMatrix<T> one(nN, 1);
	one.setOne();
	fMatrix<T> check(1,1);

	// pointers on gauss points tables, for all possible elements. In the element loop, depending on the element type, the correct pointer will be set 
	fMatrix<T> *gp_se3 = getGauss(22, 5);
	fMatrix<T> *gp_t6 = getGauss(42, 4);
	// idem for shape functions 
	fMatrix<T> *N_se3 = getN(22, *gp_se3);
	fMatrix<T> *N_t6 = getN(42, *gp_t6);
	fMatrix<T> *B_se3 = getB(22, *gp_se3);	
	fMatrix<T> *B_t6 = getB(42, *gp_t6);	
	
	//gp_se3->print();
	//gp_t6->print();
	//N_se3->print();
	//N_t6->print();
	//B_se3->print();
	//B_t6->print();

	fMatrix<T> *gp;// gauss points and weights for current element 
	fMatrix<T> *N;// shape functions for current element 
	fMatrix<T> *Ng; Ng = new fMatrix<T>(1,1);// shape functions for current gauss point. Submat of *N
	fMatrix<T> *B; // idem for gradient matrix
	fMatrix<T> *Bg; Bg = new fMatrix<T>(1,1);
	fMatrix<T> *Bref; Bref = new fMatrix<T>(1,1);
	fMatrix<T> *JacG; JacG = new fMatrix<T>(1,1);
	//fMatrix<float> *coordF; coordF = new fMatrix<float>(1,1);
	fMatrix<T> *coord; coord = new fMatrix<T>(1,1);
	vector<int> *nodes; nodes = new vector<int>;
	//fLinSys<T> *temp; temp = new fLinSys<T>();
	message = "Beginning assembly"; 
	cout << message << endl; 
	writeInfo(message);
	
	
	// matrix initialisation, as well as gauss points and shape functions 
	if (m_mesh[iC]->contains1D())
	{
		//cout << "yes 1D" << endl;	
		m_Mseg = new fMatrix<T>(nN, nN);
		m_Kseg = new fMatrix<T>(nN, nN);
	}
	if (m_mesh[iC]->contains2D())
	{
		//cout << "yes 2D" << endl;
		m_Msurf = new fMatrix<T>(nN, nN);
		m_Ksurf = new fMatrix<T>(nN, nN);
	}
	if (m_mesh[iC]->contains3D())
	{
		//cout << "yes 3D" << endl;
		m_Mvol = new fMatrix<T>(nN, nN);
		m_Kvol = new fMatrix<T>(nN, nN);
	}
	
	if ( !(m_mesh[iC]->contains1D() || m_mesh[iC]->contains2D() || m_mesh[iC]->contains3D())) 
	{
		cout << "your mesh hasn't been prepared correctly" << endl;
	}
	
	for(int iE = 0; iE < nE; iE++)
	{
		// initialisation  
		elem = m_mesh[iC]->getElement(iE);
		np = elem.getnN(); // nombre de noeuds 
		// construction de la liste de noeuds et réorganisation 
		delete coord;
		delete nodes;
		coord = new fMatrix<T>(np,3);
		nodes = new vector<int>; // A REMPLACER PAR UNE MATRICE AVEC TEMPLATE
		*coord = elem.getCoordinates();
		//*coord = *coordF;
		//coord->print();
		*nodes = elem.getNodesIds();	
		//cout << nodes->size() << endl;
		if (elem.is1D())
		{
			countSeg++;
			currentK = m_Kseg;
			currentM = m_Mseg;

			//FAUT LES REORGANISER. METHODE PERMUTATE ? qui prend un ou deux vector<int> en input 
			switch (elem.getFeDescriptor()){
				case 22:
					gp = gp_se3;
					N = N_se3;
					B = B_se3;
					perm = {0,1,2}; // unv to aster format. Avec des SE3 l'interet est limite, j'avoue... 
					*coord = coord->swapLines(perm);
					*nodes = swapLines(*nodes, perm); // AAAAAH !! A REMPLACER PAR UNE PUTAIN DE FMATRIX
					break;
				default:
					cout << "Unsupported element in FemCase<T>::buildKM, type" << elem.getFeDescriptor() << endl;
			}
		}
		else if (elem.is2D())
		{
			countSurf++;
			currentK = m_Ksurf;
			currentM = m_Msurf;
			switch (elem.getFeDescriptor()){
				case 42:
					gp = gp_t6;
					N = N_t6;
					B = B_t6;
					perm = {0,2,4,1,3,5}; // 42 unv to T6 aster format 
					*coord = coord->swapLines(perm);
					*nodes = swapLines(*nodes, perm); // AAAAAH !! A REMPLACER PAR UNE PUTAIN DE FMATRIX
					break;
				default:
					cout << "Unsupported element in FemCase<T>::buildKM, type" << elem.getFeDescriptor() << endl;
			}
		}
		else if(elem.is3D())
		{
			countVol++;
			currentK = m_Kvol;
			currentM = m_Mvol;
			switch (elem.getFeDescriptor()){
				case 125462: // T10
					perm = {0,2,4,9,1,3,5,6,7,8}; // unv ?? to T10 aster format 
					coord->swapLines(perm);
					*nodes = swapLines(*nodes, perm); // AAAAAH !! A REMPLACER PAR UNE PUTAIN DE FMATRIX
					break;
				default:
					cout << "Unsupported element in FemCase<T>::buildKM, type" << elem.getFeDescriptor() << endl;
			}
		}
		else
		{
			message = "Incoherent mesh, make sure it has been prepared correctly. The current element is neither 1D, 2D or not 3D";
			cout << message << endl; 
			writeError(message);
		}
		gw = gp->getSizeN()-1;// index of the column containing the weights  
		ngp = gp->getSizeM();// number of gauss points
		nb = B->getSizeM()/ngp;
		delete(Ke);
		delete(Me);
		delete(Bg);
		delete(Bref);
		delete(Ng);
		delete(JacG);
		Ke = new fMatrix<T>(np, np);
		Me = new fMatrix<T>(np, np);
		Bg = new fMatrix<T>(nb, np);
		Bref = new fMatrix<T>(nb, np);
		Ng = new fMatrix<T>(1, np);
		JacG = new fMatrix<T>(np,np);
		Ke->setZero();
		Me->setZero();
		Bg->setZero();
		Bref->setZero();
		Ng->setZero();
		JacG->setZero();
		for (int iG = 0; iG < ngp; iG++)
		{
			*Bref = B->submat(nb*iG, (nb*(iG+1))-1, 0, np-1);
			*Ng = N->submat(iG, iG ,0 ,np-1);
			*JacG = (*Bref)**coord;
			detJac = JacG->getFemDetJac();
			*Bg = computeFemB(*JacG, *Bref);
			*Ke = *Ke + detJac*(*gp)(iG, gw)*(Bg->t())**Bg; 
			*Me = *Me + detJac*(*gp)(iG, gw)*Ng->t()**Ng;
			//if(iE == 148)
			//{
			//	cout << "Element " << iE << ", gauss point " << iG << endl;
			//	cout << "detJac = " << detJac << endl;
			//	cout << "gp = " << (*gp)(iG, gw) << endl;
			//	cout << "JacG= " << *JacG << endl;
			//	cout << "Bref = " << *Bref << endl;
			//	cout << "Bg = " << *Bg << endl;
			//	cout << "Bg^T*Bg = " << (Bg->t()**Bg) << endl;
			//}
		}
		assert(Ke->getSizeM() == Me->getSizeM()); 
		assert(Ke->getSizeN() == Me->getSizeN());
		for (unsigned int i = 0 ; i < Ke->getSizeM() ; i++)
		{
			globalI = (*nodes)[i]-1;
			for (unsigned int j = 0; j < Ke->getSizeN() ; j++ )
			{
				globalJ = (*nodes)[j]-1;
				(*currentK)(globalI, globalJ) += (*Ke)(i,j);
				(*currentM)(globalI, globalJ) += (*Me)(i,j);
				if(isnan((*Me)(i,j)))
				{
					cout << "Found a nan in Me, element " << iE << endl << *Me << endl;
					return false; 	
				}
				if(isnan((*Ke)(i,j)))
				{
					cout << "Found a nan in Ke, element " << iE << endl << *Ke << endl;
					return false;	
				}
				//cout << "Ke : ";
				//cout << *Ke << endl;
				//return false;
			}
		}
		// stop to check current matrices 
		//if(elem.getFeDescriptor() == 42)
		//{
		//	cout << "Me : " << endl;
		//	Me->print();
		//	cout << "Ke : " << endl;
		//	Ke->print();
		//	cout << "coord : " << endl;
		//	coord->print();
		//	cout << "Nodes : " ;
		//	for(int i = 0; i<np ; i++)
		//	{
		//		cout << (*nodes)[i] << " ";
		//	}	
		//	return true;
		//}
	}	
	
	// check que le volume est correct 
	if (m_mesh[iC]->contains1D())
	{
		check = one.t()**m_Mseg*one;
		cout << "Total distance computed from M " << check << endl;
	}
	if (m_mesh[iC]->contains2D())
	{
		check = one.t()**m_Msurf*one;
		cout << "Total surface computed from M " << check << endl;
	}
	if (m_mesh[iC]->contains3D())
	{
		check = one.t()**m_Mvol*one;
		cout << "Total volume computed from M " << check << endl;
	}
	cout << "Assembly finished sucessfully, with " << countSeg << " segment elements, " << countSurf << " surface elements, and " << countVol << " volume elements." << endl;

	//cout << "Msurf : " << endl << m_Msurf->submat(0,10,0,10);
	//cout << "Ksurf : " << endl << m_Ksurf->submat(0,10,0,10);
	return true; 
}

template <typename T>
bool FemCase<T>::buildF()
{
	m_Fvol = new fMatrix<T>(m_mesh[0]->getNodesNumber(),1);
	m_Fsurf = new fMatrix<T>(m_mesh[0]->getNodesNumber(),1);
	m_Fseg = new fMatrix<T>(m_mesh[0]->getNodesNumber(),1);
	(*m_Fvol)(254,0) = 1;
	(*m_Fsurf)(254,0) = 1;
	(*m_Fseg)(254,0) = 1;
	return true; 
}




template <typename T>
bool FemCase<T>::writeMicValuesHeader()
{
	ofstream micValues(m_path+"results/micValues");
	if(!micValues)
	{
		return false;
	}
	micValues << "frequencies		";
	for(unsigned int iMic = 0; iMic < m_setup[0].getMics().size() ; iMic ++)
	{
		micValues << m_setup[0].getMics()[iMic] << "		";
	}
	micValues << endl;
	return true;
}

template <typename T>
bool FemCase<T>::writeMicValues(double f, vector<T> values)
{
	ofstream micValues(m_path+"results/micValues", ios::app);
	if(!micValues)
	{
		cout << "Could not open result/micValues file" << endl;
		return false;
	}
	if(values.size() != m_setup[0].getMics().size())
	{
		cout << "incoherence between values and mics number" << endl;
		return false;
	}
	micValues << f << "		";
	for(unsigned int iMic = 0; iMic < m_setup[0].getMics().size() ; iMic ++)
	{
		micValues << values[iMic] << " ";
		// si plusieurs segments, il faut rajouter une boucle là...
		micValues << "		";
	}
	micValues << endl;
	return true;
}

template <typename T>
bool FemCase<T>::writeVtkMesh(string filename) const
{
	cout << "Writing vtk" << endl;
	ofstream vtkfile(m_path+"results/"+filename);
	if(!vtkfile)
	{
		cout << "Could not open results/" << filename << " file" << endl;
		return false; 	
	}


	int nNodes(m_mesh[0]->getNodesNumber()); 
	int nElem(m_mesh[0]->getElementNumber());
	fMatrix<float> coord(m_mesh[0]->getCoordinates()+(float)1E-7);
	fMatrix<int> conec(m_mesh[0]->getConecAndNN());
	fMatrix<int> elemType(m_mesh[0]->getElemTypesVtk());
	int nTot(conec.submat(0,nElem-1, 0,0).sum() + nElem);

	
	vtkfile << "# vtk DataFile Version 2.0" << endl << "VTK from aFemCode" << endl;
	vtkfile << "ASCII" << endl << "DATASET UNSTRUCTURED_GRID" << endl;

	vtkfile << "POINTS " << nNodes << " float" << endl;
	vtkfile << coord << endl ;
	vtkfile << "CELLS " << nElem << " " << nTot << endl;
	vtkfile << conec << endl ;
	vtkfile << "CELL_TYPES " << nElem << endl;
	vtkfile << elemType << endl ;


	cout << "Done" << endl;

	return true;
}

template <typename T>
bool FemCase<T>::writeVtkData(string filename, string dataName, fMatrix<T> data) const
{
	ofstream vtkfile(m_path+"results/"+filename, ios::app);
	if(!vtkfile)
	{
		cout << "Could not open results/" << filename << " file" << endl;
		return false; 	
	}
	
	int nNodes(m_mesh[0]->getNodesNumber()); 

	vtkfile << "POINT_DATA " << nNodes << endl << "SCALARS " << dataName << " double 1" << endl;
	// attention je fais peut-être des bêtises avec les types...
	vtkfile << "LOOKUP_TABLE default" << endl; 
	vtkfile << data << endl;
	return true;
}





















// Assembly functions 

template <typename T>
fMatrix<T>* FemCase<T>::getGauss(int element, int order)
{
	// in case of an error, err is returned and message is printed
	fMatrix<T> *err;
	err = new fMatrix<T>(0,0);
	stringstream message;
	message << "Fatal error : element " << element << " with order " << order << " unsupported in FemCase<T>::getGauss";
	// if everything goes alright, we return gp
	fMatrix<T> *gp;
	// initialization of various shit 
	double a = 0.445948490915965;
    double b = 0.091576213509771;
	double c = 0.11169079483905;
	double d = 0.0549758718227661;	
	switch (element)
	{
		case 22: // segment SE3
		case 21: // segment SE2
			switch (order)
			{
				case 1:
					gp = new fMatrix<T>(1,2);
					(*gp) (0,0) = 0;
					(*gp)(0,1) = 2;
					break;
				case 3:
					gp = new fMatrix<T>(2,2);
					(*gp)(0,0) = -0.57735;
					(*gp)(0,1) = 1;
					(*gp)(1,0) = 0.57735;
					(*gp)(1,1) = 1;
					break;
				case 5:
					gp = new fMatrix<T>(3,2);
					(*gp)(0,0) = -0.77460;
					(*gp)(0,1) = 0.555555556;
					(*gp)(1,0) = 0; 
					(*gp)(1,1) = 0.888888889;
					(*gp)(2,0) = 0.77460;
					(*gp)(2,1) = 0.555555556;
					break;
				default:
					cout << message.str() << endl;
				   	writeError(message.str());
					return err;	
			}
			break;
		case 42: // T6 
			switch (order)
			{
				case 1:
					gp = new fMatrix<T>(1,3);
					(*gp)(0,0) = 0.33333333334;
					(*gp)(0,1) = 0.33333333334;
					(*gp)(0,2) = 0.5;
					break;
				case 2:
					gp = new fMatrix<T>(3,3);
					gp->setOne();
					*gp = *gp*(0.16666666667);
					(*gp)(1,0) = 0.66666666667;
					(*gp)(2,1) = 0.66666666667;
					break;
				case 4:
					gp = new fMatrix<T>(6,3);
					gp->setOne();
					*gp = *gp*a;
					(*gp)(1,0) = 1-2*a;
					(*gp)(2,1) = 1-2*a;
					(*gp)(3,0) = b;
					(*gp)(3,1) = b;
					(*gp)(4,0) = 1-2*b;
					(*gp)(4,1) = b;
					(*gp)(5,0) = b;
					(*gp)(5,1) = 1-2*b;
					(*gp)(0,2) = c;
					(*gp)(1,2) = c;
					(*gp)(2,2) = c;
					(*gp)(3,2) = d;
					(*gp)(4,2) = d;
					(*gp)(5,2) = d;
					break;
				default:
					cout << message.str() << endl;
				   	writeError(message.str());
					return err;	
			}
			break;
		default:
			cout << message.str() << endl;
			writeError(message.str());
			return err;
	}
	return gp;
}


template <typename T>
fMatrix<T>* FemCase<T>::getN(int element, fMatrix<T> gp)
{
	// in case of an error, err is returned and message is printed
	fMatrix<T> *err;
	err = new fMatrix<T>(0,0);
	stringstream message;
	message << "Fatal error : element " << element << " unsupported in FemCase<T>::getN";
	// if everything goes alright, we return gp
	fMatrix<T> *N;
	
	// initialization of various shit 
	int ng(gp.getSizeM());
	double a(0);
	double xi(0);
	double eta(0);
	double ksi(0);


	switch (element)
	{
		case 22: // segment SE3
			N = new fMatrix<T>(ng,3);
			for (int ig = 0; ig < ng ; ig++)
			{
				ksi = gp(ig,0);
				//cout << "ksi :" << ksi<< endl;
				(*N)(ig,0) = -0.5*ksi*(1-ksi);
				(*N)(ig,1) = (1-ksi*ksi); 
				(*N)(ig,2) = 0.5*ksi*(1+ksi);
			}	
			//ksiG = gaussPts(1);
        	//Ni = [-ksiG*(1-ksiG); ...
        	//    2*(1-ksiG^2) ; ...
        	//    ksiG*(1+ksiG)];
        	//Ni = 1/2*Ni';
			break;
		case 42: // T6 
			N = new fMatrix<T>(ng,6);
			for (int ig = 0; ig < ng ; ig++)
			{
				xi = gp(ig,0);
				eta = gp(ig,1);
				a = 1-xi-eta;
				(*N)(ig,0) = -a*(1-2*a); 
				(*N)(ig,1) = -xi*(1-2*xi); 
				(*N)(ig,2) = -eta*(1-2*eta); 
				(*N)(ig,3) = 4*xi*a; 
				(*N)(ig,4) = 4*xi*eta; 
				(*N)(ig,5) = 4*eta*a; 
			}
			//xi = gaussPts(1);
        	//eta = gaussPts(2);
        	//a = 1-xi-eta ;
        	//Ni = [-a*(1-2*a), ...       %1
        	//    -xi*(1-2*xi)...         %2
        	//    -eta*(1-2*eta)...       %3
        	//    4*xi*a...               %4
        	//    4*xi*eta...             %5
        	//    4*eta*a ] ;             %6
			break;
		default:
			cout << message.str() << endl;
			writeError(message.str());
			return err;
	}
	return N;
}

template <typename T>
fMatrix<T>* FemCase<T>::getB(int element, fMatrix<T> gp)
{
	// in case of an error, err is returned and message is printed
	fMatrix<T> *err;
	err = new fMatrix<T>(0,0);
	stringstream message;
	message << "Fatal error : element " << element << " unsupported in FemCase<T>::getN";
	// if everything goes alright, we return gp
	fMatrix<T> *B;
	
	// initialization of various shit 
	int ng(gp.getSizeM());
	double a(0);
	double xi(0);
	double eta(0);
	double ksi(0);
	
	switch (element)
	{
		case 22: // segment SE3
			B = new fMatrix<T>(ng,3);
			for (int ig = 0; ig < ng ; ig++)
			{
				ksi = gp(ig,0);
				(*B)(ig,0) = 0.5*(-1+2*ksi); 
				(*B)(ig,1) = -2*ksi; 
				(*B)(ig,2) = 0.5*(1+2*ksi);
			}
			//ksi = gaussPts(1);
        	//Bref = 1/2*[-1+2*ksi -4*ksi 1+2*ksi];	
			break;
		case 42: // T6 
			B = new fMatrix<T>(ng*2,6);
			for (int ig = 0; ig < ng ; ig++)
			{
				//cout <<"assembling T6 gp, ig = " << ig << endl; 
				xi = gp(ig,0);
				eta = gp(ig,1);
				a = 1-4*(1-xi-eta);
				(*B)(2*ig+0,0) = a; 
				(*B)(2*ig+0,1) = -1+4*xi; 
				(*B)(2*ig+0,2) = 0; 
				(*B)(2*ig+0,3) = 4*(1-2*xi-eta); 
				(*B)(2*ig+0,4) = 4*eta; 
				(*B)(2*ig+0,5) = -4*eta; 
				(*B)(2*ig+1,0) = a; 
				(*B)(2*ig+1,1) = 0; 
				(*B)(2*ig+1,2) = -1+4*eta; 
				(*B)(2*ig+1,3) = -4*xi; 
				(*B)(2*ig+1,4) = 4*xi; 
				(*B)(2*ig+1,5) = 4*(1-xi-2*eta); 
			}
			//xi = gaussPts(1);
        	//eta = gaussPts(2);
        	//a = 1-4*(1-xi-eta);
        	//Bref(:,1) = [a ; a] ;
        	//Bref(:,2) = [-1+4*xi ; 0] ;
        	//Bref(:,3) = [0 ; -1+4*eta] ;
        	//Bref(:,4) = [4*(1-2*xi-eta) ; -4*xi] ;
        	//Bref(:,5) = [4*eta ; 4*xi] ;
        	//Bref(:,6) = [-4*eta ; 4*(1-xi-2*eta)] ;
			break;
		default:
			cout << message.str() << endl;
			writeError(message.str());
			return err;
	}
	return B;
}


template <typename T>
fMatrix<T> FemCase<T>::computeFemB(fMatrix<T> Jac, fMatrix<T> Bref) const
{
	// if Bref is 3*3, return inv(Jac)*Bref
	// if Bref is 2*3, return 2 uper lines of inv([Jac ; 1 1 1])*[Bref ; 1 1 1]
	// if Bref is 1*3, should not be used for now, return null vector
	
	fMatrix<T> B(3,3);
	fMatrix<T> fullJac(3,3);	
	fMatrix<T> fullBref(3,Bref.getSizeN());

	if(Jac.getSizeM() == 3)
	{
		assert(Jac.getSizeM()==3);
		B = Jac.inv()*Bref;
	}
	else if(Jac.getSizeM() == 2)
	{
		assert(Jac.getSizeN() == 3);
		assert(Bref.getSizeN()==6);
		fullJac(0,0) = Jac(0,0);
		fullJac(0,1) = Jac(0,1);
		fullJac(0,2) = Jac(0,2);
		fullJac(1,0) = Jac(1,0);
		fullJac(1,1) = Jac(1,1);
		fullJac(1,2) = Jac(1,2);
		fullJac(2,0) = 1;
		fullJac(2,1) = 1;
		fullJac(2,2) = 1;
		fullBref(0,0) = Bref(0,0);
		fullBref(0,1) = Bref(0,1);
		fullBref(0,2) = Bref(0,2);
		fullBref(0,3) = Bref(0,3);
		fullBref(0,4) = Bref(0,4);
		fullBref(0,5) = Bref(0,5);
		fullBref(1,0) = Bref(1,0);
		fullBref(1,1) = Bref(1,1);
		fullBref(1,2) = Bref(1,2);
		fullBref(1,3) = Bref(1,3);
		fullBref(1,4) = Bref(1,4);
		fullBref(1,5) = Bref(1,5);
		fullBref(2,0) = 1;
		fullBref(2,1) = 1;
		fullBref(2,2) = 1;
		fullBref(2,3) = 1;
		fullBref(2,4) = 1;
		fullBref(2,5) = 1;
		//cout << "fullJac.det() : " << endl << fullJac.det(); 
		//cout << "fullJac.inv() : " << endl;
		//fullJac.inv().print();
		B = (fullJac.inv()*fullBref); //.submat(0,1,0,2);
		//cout << "inside computeFemB" << endl << "fullJac, fullB, B" << endl;
		//fullJac.print();
		//fullBref.print();
		//B.print();

		B(2,0) = 0;
		B(2,1) = 0;
		B(2,2) = 0;
		B(2,3) = 0;
		B(2,4) = 0;
		B(2,5) = 0;
	}
	return B;
}


// CRAAAAAAADE !! FAIS UN TEMPLATE VITE !
template <typename T>
vector<int> FemCase<T>::swapLines(vector<int> v, vector<int> perm) const
{
	// reorganizes the lines of the vector. Useful only until fMatrix<T> is made via template. 
	// Pleaaaase delete me as fast as possible !
	//
	
	if (perm.size()!= v.size())
	{
		cout << "perm.size() = " << perm.size() << " and v.size() = " << v.size() << endl;
		assert(perm.size() == v.size());
	}
	
	vector<int> v2(v.size());
	unsigned int newI;
	
	for(unsigned int i = 0 ; i < v.size() ; i++)
	{
		newI = perm[i];
		v2[i] = v[newI];
		//assert(newI<v.size());
		//for(unsigned int j = 0 ; j < m_n ; j++)
		//{
		//}
	}
	return v2;
}































































#endif

/*

*/