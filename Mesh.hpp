#ifndef DEF_MESH
#define DEF_MESH

#include <string>
#include <iostream>
#include <fstream>
// #include <vector>
#include"Element.hpp"

class Mesh
{

public:
    Mesh();
    Mesh(std::string info, std::string error);
    Mesh(Mesh m1, Mesh m2);

    ~Mesh();
    bool unvImport(std::string unvFileToRead);
    int import2411(std::string unvFileToRead, int position);
    int import2412(std::string unvFileToRead, int position);
    int import2467(std::string unvFileToRead, int position);
    bool addNode(int index, float x, float y, float z);
    bool addElement(int index, std::vector<Node*> nodes, int feDescriptor, int physicalProperty, int materialProperty, int color, int numberOfNodes);
	Element getElement(size_t index) const;
	std::vector<Element> getElements() const;
    bool printConnectivities() const;
    bool printCoordinates() const;
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> getConnectivities() const;
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> getConecAndNN() const;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> getCoordinates() const;
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> getElemTypesVtk() const;
    
    int typeAssign(int nbOfNodes);
    bool isBeginEnd(std::string line);
    bool calculateVolume() ;
    int getElementNumber() const;
    int getNodesNumber() const;
    std::vector<Node> getNodes() const;

    bool setInfo(std::string value);
    std::string getInfo() const;
    bool writeInfo(std::string info) const;

    bool setError(std::string value);
    std::string getError() const;
    bool writeError(std::string error) const;

	bool computeAspectRatio();
	bool computeJacobians(); // the aim is just to check if all elements are correctly oriented and not swiped 
	bool contains1D() const;
	bool contains2D() const;
	bool contains3D() const;
    bool displayInfo() const;

private:
    std::vector<Node> m_nodes;
    std::vector<Element> m_elements;
    std::string m_info;
    std::string m_error;
	size_t m_nN;
	size_t m_nE;
	float m_dist;
	float m_surf; 
	float m_vol;
	bool m_1D;
	bool m_2D;
	bool m_3D;
    std::vector<std::string> m_groupNames;
    std::vector<std::vector<size_t>> m_groups;
    size_t m_nGroups;
};

#endif

/*
il faudrait un vecteur de noeuds et un vecteur d'éléments. 
*/
