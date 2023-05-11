#ifndef DEF_MESH
#define DEF_MESH

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include"Element.h"
#include"Node.h"

class Mesh
{

public:
    Mesh();
    Mesh(std::string info, std::string warning);
    ~Mesh();
    bool unvImport(std::string unvFileToRead);
    int import2411(std::string unvFileToRead, int position);
    int import2412(std::string unvFileToRead, int position);
    bool addNode(int index, float x, float y, float z);
    bool addElement(int index, std::vector<Node*> nodes, int feDescriptor, int physicalProperty, int materialProperty, int color, int numberOfNodes);
    bool getConnectivities() const;
    bool getCoordinates() const;
    int typeAssign(int nbOfNodes);
    bool isBeginEnd(std::string line);
    bool calculateVolume() ;
    //double calculateArea() const; // included in calculateVolume
    int getElementNumber() const;
    int getNodesNumber() const;

    bool setInfo(std::string value);
    std::string getInfo() const;
    bool writeInfo(std::string info) const;

    bool setError(std::string value);
    std::string getError() const;
    bool writeError(std::string error) const;

	bool computeAspectRatio();


private:
    std::vector<Node> m_nodes;
    std::vector<Element> m_elements;
    std::string m_info;
    std::string m_error;
	int m_nN;
	int m_nE;
	double m_dist;
	double m_surf; 
	double m_vol;
};

#endif

/*
il faudrait un vecteur de noeuds et un vecteur d'éléments. 
*/
