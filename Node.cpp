#include <iostream>
#include "Node.hpp"

using namespace std;

Node::Node() : m_id(1)
{

}

Node::Node(int index, float x, float y, float z):m_id(index)//, m_coordinates {x,y,z}
{
    m_coordinates[0] = x; 
    m_coordinates[1] = y;
    m_coordinates[2] = z;

	m_groupId = 0;

    // cout << "Création du noeud n°" << m_id << endl ; 
    // cout << m_coordinates[0] << "   " << m_coordinates[1] << "   " << m_coordinates[2] << endl;

}

Node::Node(int index, float x, float y, float z, int groupId):m_id(index), m_groupId(groupId)//, m_coordinates {x,y,z}
{
    m_coordinates[0] = x; 
    m_coordinates[1] = y;
    m_coordinates[2] = z;

    // cout << "Création du noeud n°" << m_id << endl ; 
    // cout << m_coordinates[0] << "   " << m_coordinates[1] << "   " << m_coordinates[2] << endl;
}

Node::~Node()
{

}

int Node::getIndex()
{
    return m_id;
}
bool Node::setIndex(int newId)
{
    m_id = newId; 
    return true;
}

bool Node::getDisplay() const
{
	cout << m_id << "		"; 
	for(unsigned int i = 0; i < 3; i++)
	{
		cout << m_coordinates[i] << "	";
	}
	cout << endl << endl; 
	return true;
}

float Node::getX() const
{
	return m_coordinates[0];
}

float Node::getY() const
{
	return m_coordinates[1];
}

float Node::getZ() const
{
	return m_coordinates[2];
}
