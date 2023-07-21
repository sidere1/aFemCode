#include <iostream>

#include "AcousticSetup.h"
#include "Node.h"
#include "PointLoad.h"
 

using namespace std;



#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


AcousticSetup::AcousticSetup()
{}


AcousticSetup::AcousticSetup(std::string setupFile, std::string path)
{
    this->m_path = path;
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
        // il faudrait faire un check : en fonction du couplingType, que tous les champs ont bien été chargés correctement. 
        m_loaded = true; 
    }
    else
    {
        cout << "Your setup file hasn't been found or opened" << endl;    
    	m_loaded = false;
	}
}

AcousticSetup::~AcousticSetup()
{}

int AcousticSetup::addAtribute(int cursor, string entry, string value) 
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
                        cout << "erreur cheloue AcousticSetup.cpp ligne " << __LINE__ << endl;
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
                        cout << "erreur cheloue AcousticSetup.cpp ligne " << __LINE__ << endl;
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

bool AcousticSetup::displayInfo()
{
	cout << "Définition de l'AcousticSetup " << endl;
    cout << m_frequencies.size() << " frequencies" << endl; 
	cout << m_microsIndex.size() << " micros " << endl;  
	cout << m_loads.size() << " loads " << endl;  
	cout << "rho " << m_rho << endl;  
	cout << "c" << m_c << endl; 
   	cout << endl;
	return true; 
}
