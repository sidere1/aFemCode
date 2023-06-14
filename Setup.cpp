#include <iostream>


#include "Setup.h"
#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
// #include <cmath>
 

using namespace std;
// using Eigen::MatrixXf;
// using Eigen::VectorXf;


#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


Setup::Setup()
{

}

Setup::Setup(std::string setupFile, std::string path):m_path(path)
{
    //opening the setup file 
    string entry;
    string value;
    int skip;
    m_setupFile = setupFile;
    ifstream setup(setupFile.c_str());
    if (setup) 
    {
        for(int cursor = 1; cursor < 4 ; cursor++)
        {// Reading header 
            setup >> entry;
        }
        
        for(int cursor = 1; cursor < 8 ; cursor++)
        {// Reading parameters

            setup >> entry;
            setup >> value;

            skip = setup.tellg();

            //cout << "entry " << entry  << " ; value " << value << endl << endl; 
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
		m_loaded = true;
    }
    else
    {
        cout << "Your setup file hasn't been found or opened" << endl;    
    	m_loaded = false;
	}
}

Setup::~Setup()
{

}


//bool Setup::performAcoustic3DNodalResolution() const 
//{
    // Mesh, 3D, freq, micros, loadCases, algo, sources, paramètre supplémentaire, nodal/modal 
    // Objet flux pour les erreurs, logs en tout genre, etc, qu'on peut passer en paramètre aux différents objets dessous 
    // voire 2 flux, u nlogs, un pour les erreurs. Eventuellement les deux sont les mêmes.   
    // un mode verbose pour savoir si ce flux va vers cout pour débugger ou vers un fichier au cas où  

    // lecture dans un fichier général d'entrée 
        // bande de fréquence 
        // sources (aux mêmes fréquences)
        // algo de résolution LU/autre... 
        // zones d'export des résultats 
        // un paramètre pour savoir si on privilégie l'espace mémoire ou le temps de calcul (A = H-w2Q ou H = H-w2Q puis H = H+w2Q-w2Q etc )


    // boucle sur tous les éléments 
        // si 2D, quelles CL
        // si 3D, assemblage H,Q
    // boucle sur tous les cas de charge
        // assemblage F
    
    // boucle sur les fréquences
        // construction matrice H_omega2 Q = F selon paramètre mémoire/vitesse 
        // résolution selon l'algo choisi 
        // boucle sur tous les micros 
            // stockage des résultats dans une ligne d'une matrice  
            // f    mic1    mic2    mic3
            // 20   1       2       1.5
            // 30   1       2       1.5
    // écriture du fichier de résultats 
    
//    return true;    
//}

// bool Setup::performHAssembly(MatrixXf H)  
// {
//     return true;
// }

// bool Setup::performHAssembly(MatrixXf Q) 
// {
//     return true;
// }

// bool Setup::performFAssembly(MatrixXf F) 
// {
//     return true;
// }

int Setup::addAtribute(int cursor, string entry, string value) 
{// we return the number of line that should be skipped 
    //string message;
    stringstream msg;
	string checkEntry;
    // int count;
    unsigned int countAim;
    switch (cursor)
    {
        case 1 :
        {
            if (entry != "3D") 
            {
                cout << "Please check your setup file. Found " << entry << " instead of 3D." << endl;
                break;
            }
            else
            {
                if (value == "true") 
                {
                    set3D(true);
                    return 0;

                }
                else
                {
                    set3D(false);
                    return 0;
                }
            }
            break;
        }

        case 2 :
        {
            if (entry != "nodal") 
            {
                msg << "Please check your setup file. Found" << entry <<" instead of nodal.";
                writeError(msg.str());
				cout << msg.str() << endl;
                break;
            }
            else
            {
                if (value == "true") 
                {
                    m_nodal = true;
                    return 0;
                }
                else
                {
                    m_nodal = false ; 
                    writeError("Par contre là je suis pas hyper d'accord, le mode modal est pas encore hyper implémenté.");
                    return 0;
                }                
            }
            break;
        }

        case 3 :
        {
            checkEntry = "frequencies";
            // cout << "Entry vaut " << entry << " et checkEntry " << checkEntry << endl;             

            if (entry.compare(checkEntry) != 0) 
            {
                //cout << "Please check your setup file. Found " << entry << " instead of " << checkEntry << endl; ////////////////À PROPAGER DANS LES PRÉCÉDENTS ////////////////
                msg << "Please check your setup file. Found" << entry << " instead of " << checkEntry;
                writeError(msg.str());
                break;
            }
            else
            {
                if (value.size() > 7 && value.compare(value.size()-7,7,".femfrq") == 0)  
                {
                    readFrequencies(m_path + value,0);    
                    return 0;
                }
                else
                {
                    countAim = atoi(value.c_str());
                    ///////////// IL FAUDRAIT CHECKER QUE LA CONVERSION A BIEN MARCHÉ /////////////////////////
                    readFrequencies(m_setupFile,countAim);    
                    if (countAim != m_frequencies.size())
                    {
                        cout << "erreur cheloue Setup.cpp ligne " << __LINE__ << endl;
                    }    
                    // getFrequencies();                                
                    return countAim+1;
                }
            }
            break;
        } 

        case 4 :
        {
            checkEntry = "micros";
            // cout << "Entry vaut " << entry << " et checkEntry " << checkEntry << endl;             

            if (entry.compare(checkEntry) != 0) 
            {
                //cout << "Please check your setup file. Found " << entry << " instead of " << checkEntry << endl; 
                //message = string("Please check your setup file. Found") + entry + string( " instead of ") + checkEntry;
                //writeError(message);
                msg << "Please check your setup file. Found" << entry << " instead of " << checkEntry;
                writeError(msg.str());
                break;
            }
            else
            {
                if (value.size() > 7 && value.compare(value.size()-7,7,".femmic") == 0)  
                {
                    readMicros(value,0);    
                    return 0;
                }
                else
                {
                    countAim = atoi(value.c_str());
                    readMicros(m_setupFile,countAim);    

                    // cout << "countaim vaut " << countAim << " et m_microsIndexs.size "<< m_microsIndex.size() << endl;
                    ///////////// IL FAUDRAIT CHECKER QUE LA CONVERSION A BIEN MARCHÉ /////////////////////////
                    if (countAim != m_microsIndex.size())
                    {
                        cout << "erreur cheloue Setup.cpp ligne " << __LINE__ << endl;
                    }    
                    return countAim+1;
                }
            }
            break;
        }     

        case 5 :
        {
            checkEntry = "loads";

            if (entry.compare(checkEntry) != 0) 
            {
                //cout << "Please check your setup file. Found " << entry << " instead of " << checkEntry << endl; 
                //message = string("Please check your setup file. Found") + entry + string( " instead of ") + checkEntry;
                //writeError(message);
                msg << "Please check your setup file. Found" << entry << " instead of " << checkEntry;
                writeError(msg.str());
                break;
            }
            else
            {
                if (value.size() > 7 && value.compare(value.size()-8,8,".femload") == 0)  
                {
                    readLoads(m_path + value);    
                    return 0;
                }
                else
                {
                    msg << "Expected a filename with .femload extension";
                    writeError(msg.str());
                    return 0;
                }
            }
            break;
        }     
        case 6 :
        {
            checkEntry = "rho";

            if (entry.compare(checkEntry) != 0) 
            {
                //message = string("Please check your setup file. Found") + entry + string( " instead of ") + checkEntry;
                //cout << message << endl; 
				//writeError(message);
                msg << "Please check your setup file. Found" << entry << " instead of " << checkEntry;
                writeError(msg.str());
                break;
            }
            else
            {
				m_rho = stod(value.c_str());
            }
			break;
        }     
        case 7 :
        {
            checkEntry = "c";

            if (entry.compare(checkEntry) != 0) 
            {
                //message = string("Please check your setup file. Found") + entry + string( " instead of ") + checkEntry;
                //cout << message << endl; 
				//writeError(message);
                msg << "Please check your setup file. Found" << entry << " instead of " << checkEntry;
                writeError(msg.str());
                break;
            }
            else
            {
				m_c = stod(value.c_str());
            }
			break;
        }     
    
        default:
            msg << "Entry " << entry << " was not expected here.";
            writeError(msg.str());
			cout << msg.str() << endl;
            return  0;
            break;
    }
    return 0;

}

//bool Setup::writeResults() 
//{
//    return true;
//}

bool Setup::set3D(bool dim)
{
    if (dim) 
    {
        m_3D = true;
    }
    else 
    {
        m_3D = false;
    }
    // We should check if there are some 3D elements as well as 2D elements in the mesh
    return true;
}

//bool Setup::setMeshFile(string meshFile)
//{
//    m_meshFile = meshFile;
//    ifstream isMeshFileOk(meshFile.c_str());
//    if (isMeshFileOk) 
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//    return true ;
//}

//string Setup::getMeshFile()
//{
//    return "deprecated";
//    //return m_meshFile;
//}

bool Setup::setInfo(string value)
{
    m_info = value;
    return true;
}

string Setup::getInfo() const
{
    return m_info;
}

bool Setup::writeInfo(string info) const
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

bool Setup::setError(string value)
{
    m_error = value;
    return true;
}

string Setup::getError() const
{
    return m_error;
}

bool Setup::writeError(string error) const
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

bool Setup::readFrequencies(string fileToRead, int end)
{
    ifstream frqFile(fileToRead.c_str());
    int count;
    int countAim = end;
    int index(0);
    double freq;
    string devNull;

    writeInfo("Reading frequencies");

    if (frqFile) 
    {
        do
        {//reading header if it is in another file, the beginning of the file if it is in the setup
            frqFile >> devNull ;
            index++;
        }while(devNull.compare("frequencies") != 0 && index < 50);

        if (index == 49) 
        {// checking that the last loop went well
            writeError("I must have missed the keyword 'frequencies' in the setup file...");
        }
        frqFile >> devNull;// skipping the file name or the numver of frequencies 
        
        if (countAim == 0) 
        {//dans ce cas de figure, c'est un fichier à part, on le lit jusqu'au bout
            while(frqFile >> index)
            {
                frqFile >> freq;
				//cout << freq << endl;
                m_frequencies.push_back(freq);
                // cout << "fréquence : " << index << " | valeur : " << freq << endl;
            }
        }
        else
        {
            for(count = 0; count < countAim; count++)
            {
                frqFile >> index;
                frqFile >> freq;
                m_frequencies.push_back(freq);
                // cout << "fréquence : " << index << " | valeur : " << freq << endl;
            }
        }
        
        writeInfo("  frequencies -> Ok");
        return true;
    }
    else
    {
        cout << "Your frqFile " << fileToRead << " hasn't been found or opened" << endl;    
        writeError("I did'nt manage to read the resolution spectrum \n\n");
        return false;
    }
}


bool Setup::readMicros(string fileToRead, int end)
{
    ifstream micFile(fileToRead.c_str());
    int count;
    int countAim = end;
    unsigned int index(0);
    int mic;
    string devNull;

    writeInfo("Reading microphones");

    if (micFile) 
    {
        do
        {//reading header if it is in another file, the beginning of the file if it is in the setup
            micFile >> devNull ;
            index++;
            // cout << index << " ignoring " << devNull << endl;
        }while(devNull.compare("micros") != 0 && index < 50+2*m_frequencies.size());

        if (index == 50+2*m_frequencies.size()) 
        {// checking that the last loop went well
            writeError("I must have missed the keyword 'micros' in the setup file...");
        }
        micFile >> devNull;// skipping the file name or the number of micros 
        
        if (countAim == 0) 
        {//dans ce cas de figure, c'est un fichier à part, on le lit jusqu'au bout
            while(micFile >> index)
            {
                micFile >> mic;
                m_microsIndex.push_back(mic);
                // cout << "fréquence : " << index << " | valeur : " << mic << endl;
            }
        }
        else
        {//dans le fichier setup
            for(count = 0; count < countAim; count++)
            {
                micFile >> index;
                micFile >> mic;
                m_microsIndex.push_back(mic);
                // cout << "micro : " << index << " | valeur : " << mic << endl;
            }
        }
        
        writeInfo("  microphones -> Ok");
        return true;
    }
    else
    {
        cout << "Your micFile " << fileToRead << " hasn't been found or opened" << endl;    
        writeError("I did'nt manage to read the resolution spectrum \n\n");
        return false;
    }
}

bool Setup::readLoads(string fileToRead)//, int end)
{
    ifstream loadFile(fileToRead.c_str());
    // int count;
    // int countAim;
    unsigned int index(0);
    vector<complex<double> >  loadSpectrum;
    int numberOfLoads;
    int iLoad;
    unsigned int iFreq;
    double a, b;
    vector<int> nodes;
    string devNull;
	std::complex<double> complexI(0, 1.0);

    writeInfo("Reading loads");

    if (loadFile) 
    {
        do
        {//skipping header
            loadFile >> devNull ;
            index++;
            // cout << index << " Loads -> ignoring " << devNull << endl;
        }while(devNull.compare("loads") != 0 && index < 50+2*m_frequencies.size());

        if (index == 50+2*m_frequencies.size()) 
        {// checking that the last loop went well
            writeError("I must have missed the keyword 'loads' in the load file...");
        }
        loadFile >> numberOfLoads;

        index = 0;// just to check that we are not skipping too much lines 
        do
        {//skipping frequencies, they must be the same as the ones in the setup file 
            loadFile >> devNull ;
            index++;
            // cout << index << " Loads -> ignoring " << devNull << endl;
        }while(devNull.compare("sources") != 0 && index < 50+2*m_frequencies.size());

        if (index == 50+2*m_frequencies.size()) 
        {// checking that the last loop went well
            writeError("I must have missed the keyword 'sources' in the load file...");
        }

        for(iLoad = 0; iLoad < numberOfLoads; iLoad++)
        {// loop over the loads 
            loadFile >> devNull ; // skipping load name 
            loadFile >> a ; // reading the number of nodes on which the load is applied 
            nodes.clear();
            for(int i = 0; i < a; i++)
            {// reading the nodes sus nommés 
                loadFile >> b;
                nodes.push_back(b);
                // cout << "Load " << iLoad << " : node " << b << endl;
            }
            // cout << endl; 
            for(iFreq = 0; iFreq < m_frequencies.size(); iFreq++)
            {
                loadFile >> a;
                loadFile >> b; 
                loadSpectrum.push_back(a+complexI*b);
                // cout << "Adding " << a << " + i" << b << endl;
            }
            m_loads.push_back(PointLoad(iLoad+1, m_frequencies, loadSpectrum, nodes)) ;

            // m_loads[iLoad].printLoad();
            
        }

        writeInfo("  loads -> Ok");
        return true;
    }
    else
    {
        cout << "Your loadFile " << fileToRead << " hasn't been found or opened" << endl;    
        writeError("I did'nt manage to read the resolution spectrum \n\n");
        return false;
    }
}

bool Setup::displayFrequencies()
{
    stringstream msg;
    writeInfo("Resolution spectrum : \n");
    for(unsigned int i = 0; i < m_frequencies.size(); i++)
    {
        msg << i+1 << "      " << m_frequencies[i] << endl;
    }
    writeInfo(msg.str());
	cout << msg.str() << endl;
    return true;
}

vector<double> Setup::getFrequencies()
{
	return m_frequencies;
}

vector<int> Setup::getMics()
{
	//vector<int> mics;
	//cout << "micros size : " << m_micros.size();
	//for(unsigned int iMic = 0; iMic < m_micros.size() ; iMic ++)
	//{
	//	mics.push_back(m_micros[iMic].getIndex());
	//}
	return m_microsIndex;
}

double Setup::getC()
{
	return m_c;
}

bool Setup::displayInfo()
{
	cout << m_frequencies.size() << " frequencies" << endl; 
	cout << m_microsIndex.size() << " micros " << endl;  
	cout << m_loads.size() << " loads " << endl;  
	cout << "rho " << m_rho << endl;  
	cout << "c" << m_c << endl; 
   	cout << endl;
	//for (int i = 0; i < m_nCoupling; i++)
	//{
	//	cout << i+1 << " : " << m_meshFile[i] << " ; " << m_setupFile[i] << " ; type " << m_couplingType[i] << endl; 
	//}
	return true; 
}

bool Setup::isLoaded()
{
	return m_loaded;
}
