// #include <fstream>
// #include <string>
#include <sstream>
#include <iostream>
#include <cmath>

//#include "Element.h"// already included via Mesh.h
//#include "Node.h" // already included via Mesh.h
#include "Mesh.hpp"

#define WHEREAMI cout << endl << "No crash yet, line " << __LINE__ << " in the file " __FILE__ << endl << endl;


using namespace std;


Mesh::Mesh()
{

}

Mesh::Mesh(std::string info, std::string error) :m_info(info), m_error(error), m_1D(false), m_2D(false), m_3D(false)
{

}

Mesh::Mesh(Mesh m1, Mesh m2) :m_info(m1.getInfo()), m_error(m2.getError()), m_1D(false), m_2D(false), m_3D(false)
{
    // this constructor builds a mesh by concatenating two ones 

    size_t nN1 = m1.getNodesNumber();
    size_t nN2 = m2.getNodesNumber(); 
    size_t nE1 = m1.getElementNumber();
    size_t nE2 = m2.getElementNumber(); 

    // concatenate nodes and update indexes
    m_nodes = m1.getNodes();
    m_nodes.resize(nN1+nN2);
    std::vector<Node> nodes2 = m2.getNodes();
    for (size_t iNode = 0; iNode < nN2 ; iNode++)
    {
        m_nodes[iNode+nN1] = nodes2[iNode];
        m_nodes[iNode+nN1].setIndex(m_nodes[iNode+nN1].getIndex()+nN1);
        // cout << "node " << iNode << " becomes " << iNode+nN1;
    }

    // concatenate elements and update connectivity 
    m_elements = m1.getElements();
    m_elements.resize(nE1+nE2);
    std::vector<Element> elements2 = m2.getElements();

    // trucs temporaires pour la creation de nouveaux éléments 
    std::vector<Node*> nodes;
    std::vector<int> previousNodes;
    int nN(0);
    int feD(0);
    int phP(0);
    int maP(0);
    int col(0);
    for (size_t iElem = 0 ; iElem < nE2 ; iElem++)
    {
        nodes.clear();
        previousNodes = elements2[iElem].getNodesIds();
        nN = elements2[iElem].getnN();
        for (int iNode = 0; iNode < nN; iNode++)
        {
            nodes.push_back(&m_nodes[previousNodes[iNode]+nN1-1]);
        }
        feD = elements2[iElem].getFeDescriptor();
        phP = elements2[iElem].getPhysicalProperty();
        maP = elements2[iElem].getMaterialProperty();
        col = elements2[iElem].getColor();
        m_elements[iElem+nE1] = Element(iElem, nodes, feD, phP, maP, col, nN);
    }

    // check que la taille des tableaux sont bien cohérentes ? 
    m_nN = m_nodes.size();
	m_nE = m_elements.size();	
    calculateVolume();
    computeAspectRatio();
    displayInfo();
}

Mesh::~Mesh()
{

}

bool Mesh::unvImport(string unvFileToRead )
{
    int position;
    int unvBlock;
    string line;
    //string message;// a eradiquer ! faut le remplacer partout par msg 
	stringstream msg;
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
					msg << "The unv file does not look like it's supposed to. I expected a block number and found " << unvBlock;
                    writeError(msg.str());
                    cout << msg.str();
                }
                
                position = importFile.tellg();
                switch (unvBlock)
                {
                    case (2411):
                        writeInfo("Reading nodes (block unv 2411)") ;
                        importFile.seekg(import2411(unvFileToRead, position));
                        msg << "-> " << m_nodes.size() << " nodes read";
                        writeInfo(msg.str());
                        cout << msg.str() << endl; 
                        msg.str("");
                        break;

                    case (2412):
                        writeInfo("Reading connectivities (block unv 2412)");
                        importFile.seekg(import2412(unvFileToRead, position));
                        msg << "-> " << m_elements.size() << " elements read";
                        writeInfo(msg.str());
                        cout << msg.str() << endl; 
                        msg.str("");                        
						break;
                    
                    case (2467):
                        writeInfo("Reading groups (block unv 2467)");
                        importFile.seekg(import2467(unvFileToRead, position));
                        if (m_groupNames.size() > 1)
                            msg << "-> " << m_groupNames.size() << " groups read :" << endl;
                        else 
                            msg << "-> " << m_groupNames.size() << " group read :" << endl;
                        for (size_t iGroup = 0; iGroup < m_groupNames.size(); ++iGroup)
                        {
                            msg << "    * " << m_groupNames[iGroup] << " containing " << m_groups[iGroup].size() << " elements" << endl;
                        }
                        writeInfo(msg.str());
                        cout << msg.str(); 
                        msg.str("");
						break;
                
                    default:
                        // cout << "Unknown block : " << line << endl ; 
                        msg << "Unknown block : " << line << endl; 
                        writeInfo(msg.str());
                        msg.str("");
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
        // cout << "Mesh file " << unvFileToRead << " could not be openend, file " << __FILE__ << " line "  << __LINE__ << endl;
        msg << "Mesh file " << unvFileToRead << " could not be openend, file " << __FILE__  << " line " << __LINE__;
        WHEREAMI
        writeError(msg.str());
		cout << msg.str() << endl;
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
                addNode(indexTemp-1, xTemp, yTemp, zTemp); // unv nodes are numbered beginning at 1 and not 0 
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
                }

                for(int j = 0; j < nbOfNodesTemp; j++)
                {
                    while(line[0] == ' ') // removing the blanks
                    {
                        line = line.substr(1, line.length()-1) ;
                    }
                    if(sscanf(line.c_str(), "%i", &nodeIndexTemp) != 0) //reading the node id 
                    {
                        if (m_nodes[nodeIndexTemp-1].getIndex() != nodeIndexTemp-1) //DANGEROUS on regarde le rang dans le vecteur et non pas l'index réel. Alors on checke en cas de souci...
                        {
                            cout << "ERROR 2000, l " << __LINE__ << ", node " << nodeIndexTemp-1 << "has index " << m_nodes[nodeIndexTemp].getIndex() << endl;
                        } 
                        nodesTemp.push_back(&m_nodes[nodeIndexTemp-1]);
                    }
                    else 
                    {
                        cout << "ERROR 2002, l " << __LINE__ << endl;
                    }
                    while(line[0] != ' ' && line.length() > 0) // removing the read node from the string 
                    {
                        line = line.substr(1, line.length()-1) ;
                    }
                }
                cout << "adding index" << indexTemp << endl;
                if (!addElement(indexTemp-1, nodesTemp, feDTemp, physPTemp, matPTemp, colorTemp, nbOfNodesTemp)) 
                {
                    WHEREAMI
                }
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




int Mesh::import2467(string unvFileToRead, int position)
{
    string line ; 
    string fileContent;
    int groupIdTemp, nElemTemp, devNull, elemTemp1, elemTemp2, nLine, elemRead;
    vector<size_t> elemIdsTemp;
    string groupNameTemp; 
    size_t nGroups(0);
    stringstream line2;


    groupIdTemp = 0;
    nElemTemp = 0;

    ifstream importFile(unvFileToRead.c_str());

    if (!importFile) 
    {
        cout << "Error 2001 : l " << __LINE__ << endl;
        cout << "could not open unv file " << unvFileToRead << endl;
        return position;
    }
    importFile.seekg(position);
    getline(importFile, line);// reading first group id and number of elements
    while(!isBeginEnd(line))
    {
        // cout << "processing identification line " << line << endl;         
        if (sscanf(line.c_str(), "%i%i%i%i%i%i%i%i", &groupIdTemp, &devNull, &devNull, &devNull, &devNull, &devNull, &devNull, &nElemTemp) == 1)
        {
            cout << line << " does not correspond to the expected format" << endl; 
            return position; 
        }
        getline(importFile, line);// reading group name 
        line2.clear(); // je sais pas pourquoi c'est pas .str(""), mais n'empêche que ça marche pas si je fais ça ! je laisse tomber pour l'instant
        line2 << line; 
        // cout << "processing group name line " << line2.str() << endl;         
        line2 >> groupNameTemp;
        // cout << "AAAAA Reading group " << groupNameTemp  << " containing " << nElemTemp << " elements"<< endl;
        m_groupNames.push_back(groupNameTemp);
        nLine = nElemTemp/2;
        elemRead = 0;
        if ((nElemTemp % 2 )== 1)
            nLine++;
        // cout << "Expecting " << nLine << " lines" << endl;
        for (int i = 0; i< nLine ; i++)
        {
            getline(importFile, line);
            if (nElemTemp-elemRead > 1)
            {
                // cout << "nElemTemp-elemRead = " << nElemTemp-elemRead << endl;
                sscanf(line.c_str(), "%i%i%i%i%i%i%i%i", &devNull, &elemTemp1, &devNull, &devNull, &devNull, &elemTemp2, &devNull, &devNull);
                elemIdsTemp.push_back(elemTemp1);
                elemIdsTemp.push_back(elemTemp2);
                elemRead+=2;
                // cout << elemTemp1 << " " << elemTemp2 << " ";
            }
            else
            {
                sscanf(line.c_str(), "%i%i%i%i", &devNull, &elemTemp1, &devNull, &devNull);
                elemIdsTemp.push_back(elemTemp1);
                elemRead++;
                // cout << elemTemp1 << " ";

            }
            // cout << elemRead << " elements read for now, out of " << nElemTemp << endl;
        }
        // WHEREAMI
        m_groups.push_back(elemIdsTemp);
        nGroups++;
        getline(importFile, line);// trying to read another group id and number of elements
    }
    m_nGroups = nGroups;
    // cout << nGroups << " groups have been read" << endl;
    position = importFile.tellg();
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

bool Mesh::printConnectivities() const
{
    cout << endl << "Mesh contains " << m_elements.size() << " elements : " << endl ; 
    for(size_t i = 0; i < m_elements.size(); i++)
    {
        m_elements[i].getDisplay();
    }
    return true; 
}

bool Mesh::printCoordinates() const
{
    cout << endl << "Coordinate table : " << endl;
    cout << "Node           x       y       z " << endl;
    for(size_t i = 0; i < m_nodes.size(); i++)
    {
        m_nodes[i].getDisplay();
    }

    return true; 
}

Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> Mesh::getConnectivities() const
// fMatrix<int> Mesh::getConnectivities() const
{
	// return a matrix with the connectivities
	int maxNElem(0);
	vector<int> nodes;
	for(size_t i = 0 ; i < m_nE ; i++)
	{
		if(m_elements[i].getnN() > maxNElem)
			maxNElem = m_elements[i].getnN();
	}
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> conec(m_nE, maxNElem);    
	// fMatrix<int> conec(m_nE, maxNElem);    
	for(size_t iE = 0; iE < m_nE ; iE++)
	{
		nodes = m_elements[iE].getNodesIds();
		for(size_t iNode = 0 ; iNode < nodes.size() ; iNode++)
		{
			conec(iE, iNode) = nodes[iNode];
		}
	}
	return conec; 
}
Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> Mesh::getConecAndNN() const
// fMatrix<int> Mesh::getConecAndNN() const
{
	// return a matrix with the connectivities, for VTK file 
	// the first row indicates the number of nodes for the corresponding element
	// the order corresponds to the one required by wtk file format 
	// in VTK file standard, the first node index is 0, therefore all indexes are shifted 
		
	int maxNElem(0);
	vector<int> nodes;
	vector<int> n;

	for(size_t i = 0 ; i < m_nE ; i++)
	{
		if(m_elements[i].getnN() > maxNElem)
			maxNElem = m_elements[i].getnN();
	}
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> conec(m_nE, maxNElem+1);
	conec.setZero();	
	// fMatrix<int> conec(m_nE, maxNElem+1);    
	for(size_t iE = 0; iE < m_nE ; iE++)
	{
		n = m_elements[iE].getNodesIds();
		// reorganizing the nodes in order to match the vtk file format
		switch (m_elements[iE].getFeDescriptor())
		{
			case 22: 
				// nodes = {n[0]-1,n[2]-1,n[1]-1}; // update : node indices begin at zero to match cpp notation
				nodes = {n[0],n[2],n[1]};
				break;
			case 42:
				nodes = {n[0],n[2],n[4],n[1],n[3],n[5]};
				// nodes = {n[0]-1,n[2]-1,n[4]-1,n[1]-1,n[3]-1,n[5]-1};
				break;
			default: 
				cout << "Oui ben faut l'coder mon pote, on peut pas utiliser ça pour l'instant ! Allez hop hop hop au boulot !" << endl; 
				cout << "Mesh::getConecAndNN, not supported element type " << m_elements[iE].getFeDescriptor();
				break;
		}
		conec(iE, 0) = nodes.size();
		for(size_t iNode = 0 ; iNode < nodes.size() ; iNode++)
		{
			conec(iE, iNode+1) = nodes[iNode];
		}
	}
	return conec; 
}
Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> Mesh::getElemTypesVtk() const
// fMatrix<int> Mesh::getElemTypesVtk() const
{
	// return a row vector with the elements types for vtk file
	
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> elemTypes(m_nE, 1);    
	// fMatrix<int> elemTypes(m_nE, 1);    
	for(size_t iE = 0; iE < m_nE ; iE++)
	{
		//see https://examples.vtk.org/site/VTKFileFormats/
		switch (m_elements[iE].getFeDescriptor())
		{
			case 22: // SE3
				elemTypes(iE, 0) = 21;
				break;
			case 42: // T6
				elemTypes(iE, 0) = 22;
				break;
			case 118 : // T10
				elemTypes(iE, 0) = 24;
				break;
			default:
				cout << "unsupported element type " << m_elements[iE].getFeDescriptor() << " in Mesh::getElemTypesVtk" << endl;
				break;
		}
	}
	return elemTypes; 
}

Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Mesh::getCoordinates() const
// fMatrix<float> Mesh::getCoordinates() const
{
	// return a matrix with the coordinates 
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> coord(m_nN, 3);
	// fMatrix<float> coord(m_nN, 3);
	for(size_t iNode = 0; iNode < m_nN ; iNode++)
	{
		coord(iNode, 0) = m_nodes[iNode].getX();
		coord(iNode, 1) = m_nodes[iNode].getY();
		coord(iNode, 2) = m_nodes[iNode].getZ();
	}
    return coord; 
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
        
        default:
            return 0;

	}
}

bool Mesh::isBeginEnd(string line)
{
    // returns true if line corresponds to the unv marker of beginning or end of a block 
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
    float vol(0);
    float surf(0);
    float dist(0);
    for(size_t elem = 0; elem < m_elements.size() ; elem++)
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
	// cout << "Distance : " << dist << endl << "Surface : " << surf << endl << "Volume : " << vol << endl;
   	//cout << m_1D << m_2D << m_3D << endl;	
    return true;
}



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
std::vector<Node> Mesh::getNodes() const
{
    return m_nodes;
}

Node Mesh::getNode(size_t iNode) const
{
    return m_nodes[iNode];
}

std::vector<Element> Mesh::getElements() const
{
    return m_elements;
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
	for(size_t i = 0; i < m_nE; i++)
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


Element Mesh::getElement(size_t index) const
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

bool Mesh::displayInfo() const
{
    cout << "Mesh contains " << m_nN << " nodes, " << m_nE << " elements. " << endl;
    cout << "Total distance " << m_dist << ", total surface " << m_surf << ", total volume " << m_vol << endl;
    return true;
}



std::vector<std::string> Mesh::getGroupNames()
{
    return m_groupNames;
}

std::vector<std::vector<size_t>> Mesh::getGroups()
{
    return m_groups;
}

std::vector<size_t> Mesh::getGroup(size_t iG)
{
    return m_groups[iG];
}
