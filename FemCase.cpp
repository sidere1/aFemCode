#include <iostream>
#include <vector>
#include <cassert>
#include <sys/stat.h>


#include "Setup.h"
#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include "FemCase.h"
 
#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;

using namespace std;


#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


FemCase::FemCase()
{

}

FemCase::FemCase(std::string setupFile)
{
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
			m_setup[i] = s;
		}
    }
    else
    {
        cout << "Your setup file hasn't been found or opened" << endl;    
    }
}

FemCase::~FemCase()
{

}



int FemCase::addAtribute(int cursor, string entry, string value) 
{// we return the number of line that should be skipped 
    string message;
    string checkEntry;
    // int count;
    unsigned int countAim;
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
            message = string("Entry " + SSTR(entry) + string(" was not expected here."));
            writeError(message);
            return  0;
            break;
    }
    return 0;

}


bool FemCase::setMeshFile(string meshFile, int iCoupling)
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

string FemCase::getMeshFile(int iCoupling)
{
    return m_meshFile[iCoupling];
}

bool FemCase::setInfo(string value)
{
    m_info = value;
    return true;
}

string FemCase::getInfo() const
{
    return m_info;
}

bool FemCase::writeInfo(string info) const
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

bool FemCase::setError(string value)
{
    m_error = value;
    return true;
}

string FemCase::getError() const
{
    return m_error;
}

bool FemCase::writeError(string error) const
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

bool FemCase::prepareComputation()
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
		m_mesh[i]->computeAspectRatio();
		m_mesh[i]->calculateVolume();
	}

	return true; 
}

bool FemCase::displayInfo()
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


bool FemCase::buildFLinSys()
{
	// loop over the couplings 
	// construction de K M B T F 
	switch(m_couplingType[0])
	{
		case 1:
			buildK();
			buildM();
			buildB();
			buildF();
			break;
		default: 
			cout << "coupling type " << m_couplingType[0] << " not implemented" << endl; 
			return false; 
	}
	return true;
}


bool FemCase::performResolution()
{
	// check for any couplings, perform coupling. 
	// 
	// if no coupling and harmonic resolution : 
		// loop over the frequencies 
		// construction de fLinSys avec la fréquence courante 
		// résolution du systeme 
		// storage of the result 

	assert(m_nCoupling == 1 && "Not allowed yet. Don't hesitate to develop it !");
	
	if (m_couplingType[0] == 1) // ou autre chose pour de la vibration harmonique par exemple ? 
	{
		
	}
	return true;
}


bool FemCase::buildK()
{
	return true; 
}
bool FemCase::buildM()
{
	return true; 
}
bool FemCase::buildB()
{
	return true; 
}
bool FemCase::buildF()
{
	return true; 
}
