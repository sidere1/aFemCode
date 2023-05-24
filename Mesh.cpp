#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "Element.h"
#include "Node.h"
#include "Mesh.h"

#define WHEREAMI cout << endl << "ERROR line " << __LINE__ << " in the file " __FILE__ << endl << endl;
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

using namespace std;


Mesh::Mesh()
{

}

Mesh::Mesh(string info, string error) :m_info(info), m_error(error), m_1D(false), m_2D(false), m_3D(false)
{

}

Mesh::~Mesh()
{

}

bool Mesh::unvImport(string unvFileToRead )
{
    // int number;
    int position;
    int unvBlock;
    string line;
    string message;
    // string beginEnd("    -1");
    // string beginEnd("    -1");

    
    // ouverture du fichier 
    ifstream importFile(unvFileToRead.c_str());

    // on continue uniquement si le fichier est ouvert
    if (importFile) 
    {
        while(getline(importFile, line))
        {

            // cout << line << endl;
            if (isBeginEnd(line)) 
            {
                        // WHEREAMI

                // cout << "I found a new block" << endl << endl ; 
                getline(importFile, line);
                // cout << line ;
                // unvBlock = std::stoi(line.c_str());
                if(sscanf(line.c_str(), "%i", &unvBlock) != 1)
                {
                    message = string("The unv file does not look like it is supposed to. I expected a block number, I found ") + SSTR(unvBlock); 
                    writeError(message);
                    cout << message;
                }
                
                position = importFile.tellg();
                switch (unvBlock)
                {
                    case (2411):
                        // cout << "Reading nodes (block unv 2411)" << endl; 
                        writeInfo("Reading nodes (block unv 2411)") ;
                        importFile.seekg(import2411(unvFileToRead, position));
                        message = string("-> ") + SSTR(m_nodes.size()) + string(" nodes read");
                        writeInfo(message);
                        cout << message << endl; 
                        break;

                    case (2412):
                        // cout << "Reading connectivities (block unv 2412)" << endl; 
                        writeInfo("Reading connectivities (block unv 2412)");
                        importFile.seekg(import2412(unvFileToRead, position));
                        message = string("-> ") + SSTR(m_elements.size()) + string(" elements read");
                        writeInfo(message);
                        cout << message << endl; 
						break;
                
                    default:
                        // cout << "Unknown block : " << line << endl ; 
                        message = "Unknown block : " + line ; 
                        writeInfo(message);
                        while(!isBeginEnd(line))
                        {
                            getline(importFile, line);
                        }
                        break;
                }
            }
        }
	   	m_nN = m_nodes.size();
		m_nE = m_elements.size();	
        return true;
    }
    // Si le fichier ne s'est pas ouvert correctement on s'arrête là 
    else 
    {
        cout << "Mesh file " << unvFileToRead << " could not be openend, file  " << __FILE__ << " line "  << __LINE__ << endl;
        message = string("Mesh file ") + unvFileToRead + string(" could not be openend, file  ") + __FILE__  + string (" line ") + SSTR(__LINE__);
        // message = "Mesh file " + unvFileToRead  ; 
        // message = message + " could not be openend, file "  ; 
        // message = message + __FILE__ ; 
        // message = message + SSTR(__LINE__) ; 
        //+ " could not be openend, file  " + __FILE__ + " line "  + __LINE__ ;
        writeError(message);
        return false;
    }
            
}

int Mesh::import2411(string unvFileToRead, int position)
{
    string line; 
    // string beginEnd("    -1");
    int indexTemp, devNull;
    float xTemp, yTemp, zTemp;

    // cout << "Here I am " << endl << endl;

    ifstream importFile(unvFileToRead.c_str());

    // on continue uniquement si le fichier est ouvert
    if (importFile) 
    {
        importFile.seekg(position);
        getline(importFile, line);
        while(!isBeginEnd(line))
        {
            // cout << "la ligne contient : " << line << endl;
            if(sscanf(line.c_str(), "%i%i%i%i", &indexTemp, &devNull, &devNull, &devNull) != 1)
            {
                // cout << "index : " << indexTemp << endl;
                getline(importFile, line);
                // cout << "la ligne contient : " << line << endl;
                if(sscanf(line.c_str(), "%e%e%e", &xTemp, &yTemp, &zTemp) != 1)
                {
                    // cout << "coordonnées : " << xTemp << " " << yTemp << " " << zTemp << endl;
                }
                else 
                {
                    cout << "ERROR 2002, Mesh.cpp l " << __LINE__ << endl;
                }
                addNode(indexTemp, xTemp, yTemp, zTemp);
            }
            else 
            {
                cout << "ERROR 2002, Mesh.cpp l " << __LINE__ << endl;
            }
            getline(importFile, line);
        }        
        // cout << "-> " << m_nodes.size() << " nodes read" << endl;

        position = importFile.tellg();
        return position;
    }
    // Si le fichier ne s'est pas ouvert correctement on s'arrête là 
    else 
    {
        cout << "Error 2001 , Mesh.cpp,  l " << __LINE__ << endl;
        return position;
    }
    return position;
}
 
int Mesh::import2412(string unvFileToRead, int position)
{
    string line ; 
    string fileContent;

    int nbOfNodesTemp, indexTemp, nodeIndexTemp, feDTemp, physPTemp, matPTemp, colorTemp; 
    vector <Node*> nodesTemp;

    indexTemp = 0;

    ifstream importFile(unvFileToRead.c_str());

    // We carry on if the file is open 
    if (importFile) 
    {
        importFile.seekg(position);
        getline(importFile, line);// reading index, element description, number of nodes, etc
        while(!isBeginEnd(line))
        {
            if(sscanf(line.c_str(), "%i%i%i%i%i%i", &indexTemp, &feDTemp, &physPTemp, &matPTemp, &colorTemp, &nbOfNodesTemp) != 1)
            {
                getline(importFile, line);// reading node list
                nodesTemp.clear();// emptying the previous node list
                
                if (feDTemp <=24) 
                {// beams are not supported yet
					getline(importFile, line);
					//cout << line << endl;
                }
                //else 
                //{                    
                    // cout << "type : " << typeTemp << endl;
                    for(int j = 0; j < nbOfNodesTemp; j++)
                    {
                        while(line[0] == ' ') // removing the blanks
                        {
                            line = line.substr(1, line.length()-1) ;
                        }
                        if(sscanf(line.c_str(), "%i", &nodeIndexTemp) != 0) //reading the node id 
                        {
                            // cout << "noeud : " << indexTemp << endl;
                            if (m_nodes[nodeIndexTemp-1].getIndex() != nodeIndexTemp) //DANGEROUS on regarde le rang dans le vecteur et non pas l'index réel. Alors on checke en cas de souci...
                            {
                                cout << "ERROR 2000, l " << __LINE__ << "node " << nodeIndexTemp << "has index " << m_nodes[nodeIndexTemp].getIndex() << endl;
                            } 
                            // WHEREAMI
                            nodesTemp.push_back(&m_nodes[nodeIndexTemp-1]);
                            // WHEREAMI
                        }
                        else 
                        {
                            cout << "ERROR 2002, l " << __LINE__ << endl;
                        }
                        // WHEREAMI
                        while(line[0] != ' ' && line.length() > 0) // removing the read node from the string 
                        {
                            line = line.substr(1, line.length()-1) ;
                        }
                        // WHEREAMI
                    }
					
                    if (!addElement(indexTemp, nodesTemp, feDTemp, physPTemp, matPTemp, colorTemp, nbOfNodesTemp)) 
                    {
                        WHEREAMI
                    }
                //} c'etait un escape si on croisait un element segment                     
            }
            else 
            {
                cout << "ERROR 2002, l " << __LINE__ << endl;
            }
            getline(importFile, line);
        }        
        // cout << "-> " << m_elements.size() << " elements read" << endl;
        position = importFile.tellg();
        return position;
    }
    // Si le fichier ne s'est pas ouvert correctement on s'arrête là 
    else 
    {
        cout << "Error 2001 : l " << __LINE__ << endl;
        return position;
    }
    return position;
}

bool Mesh::addNode(int index, float x, float y, float z)
{
    //BETTER : CHECK IF THE INDEX DOESN'T ALREADY EXIST
    m_nodes.push_back(Node(index, x, y, z));
    return true; 
}

bool Mesh::addElement(int index, std::vector<Node*> nodes, int feDescriptor, int physicalProperty, int materialProperty, int color, int numberOfNodes)
{
    // cout << "Checking element " << index << " of fe Type " << feDescriptor << " with " << nodes.size() << " elements " << endl; 
    m_elements.push_back(Element(index, nodes, feDescriptor, physicalProperty, materialProperty, color, numberOfNodes));

    if (m_elements[m_elements.size()-1].checkElem()) 
    {
        return true;
    }
    else
    {
        cout << "Refusing element " << m_elements[m_elements.size()-1].getIndex() << " of type " << m_elements[m_elements.size()-1].getFeDescriptor() << endl; 
        m_elements.pop_back();
        // WHEREAMI
        return false; 
    }
}

bool Mesh::getConnectivities() const
{
    cout << endl << "Mesh contains " << m_elements.size() << " elements : " << endl ; 
    for(unsigned int i = 0; i < m_elements.size(); i++)
    {
        m_elements[i].getDisplay();
    }
    return true; 
}

bool Mesh::getCoordinates() const
{
    cout << endl << "Coordinate table : " << endl;
    cout << "Node           x       y       z " << endl;
    for(unsigned int i = 0; i < m_nodes.size(); i++)
    {
        m_nodes[i].getDisplay();
    }

    return true; 
}

int Mesh::typeAssign(int nbOfNodes)
{
	switch (nbOfNodes)
	{
		case 3:
            return 11;
            break;

		case 6:
            return 12;
            break;
        
        // case 4:
        //     return 21;
        //     break;

        case 8:
            return 22;
            break;
        
        case 4:
            return 111;
            break;
        
        case 10:
            return 112;
            break;
        
        // case 8: // NEVER..................
        //     return 121;
        //     break;

        default:
            return 0;

	}
}

bool Mesh::isBeginEnd(string line)
{
    int marker, devNull;
    if ((sscanf(line.c_str(), "%d%d", &marker, &devNull) == 1) && marker == -1)
    {
        return true; 
    }
    else 
    {
        return false;
    }
    
}

bool Mesh::calculateVolume() 
{
    double vol(0);
    double surf(0);
    double dist(0);
    for(unsigned int elem = 0; elem < m_elements.size() ; elem++)
    {
		// il faudrait faire un tri entre éléments de surface / longueur / volume ! 
		if (m_elements[elem].is1D())
		{
			dist = dist + m_elements[elem].getVolume();
			m_1D = true;
		}
		else if (m_elements[elem].is2D())
		{ 
			surf = surf + m_elements[elem].getVolume();
			m_2D = true;
		}
		else if (m_elements[elem].is2D())
		{ 
			vol = vol + m_elements[elem].getVolume();
			m_3D = true;
		}
        //volume = volume + m_elements[elem].getVolume();
    }
	m_vol = vol; 
	m_surf = surf; 
	m_dist = dist;
	cout << "Distance : " << dist << endl << "Surface : " << surf << endl << "Volume : " << vol << endl;
   	//cout << m_1D << m_2D << m_3D << endl;	
    return true;
}

//double Mesh::calculateArea() const 
//{
//    double area(0);
//    int count(0) ; 
//    double plus;
//    for(unsigned int elem = 0; elem < m_elements.size() ; elem++)
//    {
//        plus = m_elements[elem].calculateArea();
//        area = area + plus;
//        if (plus != 0) 
//        {
//            count++;
//        }
//    }
//    // cout << count << " elements taken into account for the surface" << endl;
//    return area;   
//}


int Mesh::getElementNumber() const
{
    //return m_elements.size();
    return m_nE;
}

int Mesh::getNodesNumber() const
{
    //return m_nodes.size();
    return m_nN;
}








bool Mesh::setInfo(string value)
{
    m_info = value;
    return true;
}

string Mesh::getInfo() const
{
    return m_info;
}

bool Mesh::writeInfo(string info) const
{
    string fileName = getInfo();
    ofstream infoFile(fileName.c_str(), ios::app);
    if (infoFile) 
    {
        infoFile << "  mesh -> " << info;
        infoFile << endl;
        return true;     
    }
    else
    {
        return false;
    }
}

bool Mesh::setError(string value)
{
    m_error = value;
    return true;
}

string Mesh::getError() const
{
    return m_error;
}

bool Mesh::writeError(string error) const
{
    writeInfo(error);
    string fileName = getError();
    ofstream errorFile(fileName.c_str(), ios::app);
    if (errorFile) 
    {
        errorFile << "  mesh -> " << error;
        errorFile << endl;
        return true;     
    }
    else
    {
        return false;
    }
}


bool Mesh::computeAspectRatio()
{
	string message("");
	for(int i = 0; i < m_nE; i++)
	{
		if (!m_elements[i].computeAspectRatio())
		{
			//message = "Aspect Ratio for Element type " + m_elements[i].getFeDescriptor() + " not supported. Development needed;
			cout << "Aspect ratio computation for Element type " << m_elements[i].getFeDescriptor() << " not supported. Development needed" << endl; 
			//writeError(message);
		}
		if (!m_elements[i].calculateVolume())
		{
			cout << "Volume computation for Element type " << m_elements[i].getFeDescriptor() << " not supported. Development needed" << endl; 
		}
	}
	return true;
}


bool Mesh::contains1D() const
{return m_1D;}
bool Mesh::contains2D() const
{return m_2D;}
bool Mesh::contains3D() const
{return m_3D;}


Element Mesh::getElement(int index) const
{
	string message("");
	if (index > m_nE)
	{
		message = "bad request. The element index you asked does not exist";
		cout << message; 
		cout << endl << "index : " << index << endl;
		writeError(message);
	}
	return m_elements[index];
}
