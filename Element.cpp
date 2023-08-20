#include <iostream>
#include <cmath>

#include "Element.hpp"
#include "Node.hpp"

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;
using namespace std;

Element::Element() :  m_index(1)//, m_numberOfNodes(3)
{
	m_physicalProperty = 0;
	m_materialProperty = 0;
	m_color = 0;
}

Element::Element(int index, std::vector<Node*> nodes, int feDescriptor, int physicalProperty, int materialProperty, int color, int numberOfNodes): m_index(index),m_feDescriptor(feDescriptor), m_physicalProperty(physicalProperty), m_materialProperty(materialProperty), m_color(color),  m_numberOfNodes(numberOfNodes)
{
	m_nodes = nodes;
	m_1D = false; 
	m_2D = false; 
	m_3D = false; 
	// cout << "element added" << endl;
	if ((feDescriptor == 21) || (feDescriptor == 22))
	{
		m_1D = true; 
	}
	if ((feDescriptor > 40) && (feDescriptor < 66))
	{
		m_2D = true; 
	}
	if (feDescriptor > 111) 
	{
		m_3D = true; 
	}
	calculateVolume();
}


Element::~Element()
{}

bool Element::is1D() const 
{
	return m_1D; 
}
bool Element::is2D() const 
{
	return m_2D;
}
bool Element::is3D() const 
{
	return m_3D; 
}
bool Element::calculateVolume() 
{
	float volume(0);
	switch (m_feDescriptor)
	{
		case 22: // SE3. On considere qu'il est compose de deux segments 
		{
			float Aa;
			float Ab;
			float Ac;
			float Ba;
			float Bb;
			float Bc;
			
			Aa = m_nodes[1]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[1]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[1]->getZ()-m_nodes[0]->getZ();
			Ba = m_nodes[2]->getX()-m_nodes[1]->getX();
			Bb = m_nodes[2]->getY()-m_nodes[1]->getY();
			Bc = m_nodes[2]->getZ()-m_nodes[1]->getZ();

			volume = sqrt(pow(Aa,2) + pow(Ab, 2) + pow(Ac, 2));
			volume = volume + sqrt(pow(Ba,2) + pow(Bb, 2) + pow(Bc, 2));
			m_vol = volume;
			break;
		}

		case 41: // T3
		{
			float Aa;
			float Ab;
			float Ac;
			float Ba;
			float Bb;
			float Bc;
			
			Aa = m_nodes[1]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[1]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[1]->getZ()-m_nodes[0]->getZ();
			Ba = m_nodes[2]->getX()-m_nodes[0]->getX();
			Bb = m_nodes[2]->getY()-m_nodes[0]->getY();
			Bc = m_nodes[2]->getZ()-m_nodes[0]->getZ();

			volume = sqrt(pow(Ab*Bc-Ac*Bb,2)+pow(Ba*Ac-Bc*Aa,2)+pow(Aa*Bb-Ab*Ba,2));
			m_vol = volume*1/2;
			break;
		}

		case 42: // T6
		{// fait salement, on considère que les éléments sont plats et réguliers, mais au moins l'ordre de grandeur est la. On pourrait le decouper en 4 triangles reguliers ?  
			float Aa;
			float Ab;
			float Ac;
			float Ba;
			float Bb;
			float Bc;
			
			Aa = m_nodes[2]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[2]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[2]->getZ()-m_nodes[0]->getZ();
			Ba = m_nodes[4]->getX()-m_nodes[0]->getX();
			Bb = m_nodes[4]->getY()-m_nodes[0]->getY();
			Bc = m_nodes[4]->getZ()-m_nodes[0]->getZ();

			volume = sqrt(pow(Ab*Bc-Ac*Bb,2)+pow(Ba*Ac-Bc*Aa,2)+pow(Aa*Bb-Ab*Ba,2));
			m_vol = volume*1/2;
			break;
		}

		case 94: // quadrangle lineaire
		{ // malin : partir de la diagonale, commune aux deux triangles du quadrangle ! Smart, non ? 
			float Aa;
			float Ab;
			float Ac;
			float Ba;
			float Bb;
			float Bc;
			
			Aa = m_nodes[1]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[1]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[1]->getZ()-m_nodes[0]->getZ();
			Ba = m_nodes[2]->getX()-m_nodes[0]->getX();
			Bb = m_nodes[2]->getY()-m_nodes[0]->getY();
			Bc = m_nodes[2]->getZ()-m_nodes[0]->getZ();

			volume = sqrt(pow(Ab*Bc-Ac*Bb,2)+pow(Ba*Ac-Bc*Aa,2)+pow(Aa*Bb-Ab*Ba,2));
			
			Aa = m_nodes[3]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[3]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[3]->getZ()-m_nodes[0]->getZ();
			// Ba = m_nodes[2]->getX()-m_nodes[0]->getX();
			// Bb = m_nodes[2]->getY()-m_nodes[0]->getY();
			// Bc = m_nodes[2]->getZ()-m_nodes[0]->getZ();

			volume = volume + sqrt(pow(Ab*Bc-Ac*Bb,2)+pow(Ba*Ac-Bc*Aa,2)+pow(Aa*Bb-Ab*Ba,2));
			m_vol = volume*1/2;
			break;
		}
		case 111: // T4
		{
			float Aa;
			float Ab;
			float Ac;
			float Ba;
			float Bb;
			float Bc;
			float Ca;
			float Cb;
			float Cc;

			Aa = m_nodes[1]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[1]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[1]->getZ()-m_nodes[0]->getZ();
			Ba = m_nodes[2]->getX()-m_nodes[0]->getX();
			Bb = m_nodes[2]->getY()-m_nodes[0]->getY();
			Bc = m_nodes[2]->getZ()-m_nodes[0]->getZ();
			Ca = m_nodes[3]->getX()-m_nodes[0]->getX();
			Cb = m_nodes[3]->getY()-m_nodes[0]->getY();
			Cc = m_nodes[3]->getZ()-m_nodes[0]->getZ();

			volume = ((Ca*(Ab*Bc-Ac*Bb))+(Cb*(Ba*Ac-Bc*Aa))+(Cc*(Aa*Bb-Ab*Ba)));
			m_vol = volume*1/6;
			break;
		}

		case 115: // ??? 
		{
			float Aa;
			float Ab;
			float Ac;
			float Ba;
			float Bb;
			float Bc;
			float Ca;
			float Cb;
			float Cc;
			
			Aa = m_nodes[1]->getX()-m_nodes[0]->getX();
			Ab = m_nodes[1]->getY()-m_nodes[0]->getY();
			Ac = m_nodes[1]->getZ()-m_nodes[0]->getZ();
			Ba = m_nodes[2]->getX()-m_nodes[0]->getX();
			Bb = m_nodes[2]->getY()-m_nodes[0]->getY();
			Bc = m_nodes[2]->getZ()-m_nodes[0]->getZ();
			Ca = m_nodes[4]->getX()-m_nodes[0]->getX();
			Cb = m_nodes[4]->getY()-m_nodes[0]->getY();
			Cc = m_nodes[4]->getZ()-m_nodes[0]->getZ();

			volume = ((Ca*(Ab*Bc-Ac*Bb))+(Cb*(Ba*Ac-Bc*Aa))+(Cc*(Aa*Bb-Ab*Ba)));
			m_vol = volume;
			break;
		}

		default:
			m_vol = 0;
			return false;
			break;
	}
	return true;
}


bool Element::checkElem() const 
{
	// See Element.h for the full unv FE type list
	switch (m_feDescriptor)
	{
		case 21: // Linear Beam
			return (m_nodes.size()==3) ;
			break;

		case 22: // Tapered Beam - SE3
			return (m_nodes.size()==3) ;
			break;

		case 41: // Plane Stress Linear Triangle - T3
			return (m_nodes.size()==3) ;
			break;

		case 42: // Plane Stress Parabolic Triangle - T6
			return (m_nodes.size()==6) ;
			break;

		case 61: // Plate Linear Triangle
			return (m_nodes.size()==3) ;
			break;
		
		case 62: // Plate Parabolic Triangle
			return (m_nodes.size()==6) ;
			break;
		
		case 64: // Plate Linear Quadrilateral
			return (m_nodes.size()==4) ;
			break;
		
		case 94: // Thin Shell Linear Quadrilateral
			return (m_nodes.size()==4) ;
			break;
		
		case 65: // Plate Parabolic Quadrilateral
			return (m_nodes.size()==8) ;
			break;
		
		case 111: // Solid Linear Tetrahedron
			return (m_nodes.size()==4) ;
			break;
		
		case 115: // Solid Linear Brick
			return (m_nodes.size()==8) ;
			break;
		
		case 118: // Solid Parabolic Tetrahedron 
			return (m_nodes.size()==10) ;
			break;
		
		default:
			return false; 
			break;
	}
}

bool Element::getDisplay() const
{
	cout << "Element " << m_index << ", type : " << m_feDescriptor << endl; 
	for(unsigned int i = 0; i < m_nodes.size(); i++)
	{
		cout << m_nodes[i]->getIndex() << "	";
	}
	cout << endl; 
	return true; 
}

int Element::getIndex() const
{
	return m_index;
}
bool Element::setIndex(int newIndex) 
{
	m_index = newIndex;
	return true;
}


int Element::getFeDescriptor() const
{
	return m_feDescriptor;
}

bool Element::computeAspectRatio()
{
	m_aspectRatio = 0;
	switch (m_feDescriptor)
	{
		case 22:
			m_aspectRatio = 0;
			break;
		case 42:
			m_aspectRatio = 0;
			break;
		default:
		   return false;	
			m_aspectRatio = 0;
	}
	return true;
}

int Element::getAspectRatio() const
{
	return m_aspectRatio;
}
//bool Element::computeVolume()
//{
//	m_aspectRatio = 0;
//	switch (m_feDescriptor)
//	{
//		case 22:// SE3
//			m_vol = ();
//			break;
//		case 42:// T6 
//			m_vol = 0;
//			break;
//		default:
//			m_vol = 0;
//		   	return false;	
//	}
//	return true;
//}

int Element::getPhysicalProperty() const
{
	return m_physicalProperty;
}
int Element::getMaterialProperty() const
{
	return m_materialProperty;
}
int Element::getColor() const
{
	return m_color;
}
    


float Element::getVolume() const
{
	return m_vol;
}

int Element::getnN() const
{
	return m_numberOfNodes;
}
vector<int> Element::getNodesIds() const
{
	//vector<int> nodes(m_numberOfNodes, 1);
	vector<int> nodes;
	for (int iNode = 0; iNode < m_numberOfNodes ; iNode++)
	{
		nodes.push_back(m_nodes[iNode]->getIndex());
	}
	return nodes;
}
// fMatrix<float> Element::getCoordinates() const
Eigen::Matrix<float, Eigen::Dynamic, 3> Element::getCoordinates() const
{
	// fMatrix<float> coord((unsigned int)m_numberOfNodes, (unsigned int)3);
   	Eigen::Matrix<float, Eigen::Dynamic, 3> coord((unsigned int)m_numberOfNodes, (unsigned int)3);
	for (int iNode = 0; iNode < m_numberOfNodes ; iNode++)
	{
		coord((unsigned int)iNode, 0) = m_nodes[iNode]->getX();
		coord((unsigned int)iNode, 1) = m_nodes[iNode]->getY();
		coord((unsigned int)iNode, 2) = m_nodes[iNode]->getZ();
	}
	return coord;
}



bool Element::setNodes(std::vector<Node*> newNodes)
{
	m_nodes.clear();
	m_nodes = newNodes;
	return true;
}

