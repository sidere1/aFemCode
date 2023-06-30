#include <iostream>
#include <complex>
#include "PointLoad.h"


using namespace std;


PointLoad::PointLoad()
{

}

PointLoad::PointLoad(int index, std::vector<double> frequencies, std::vector<std::complex<double> > spectrum, std::vector<int> nodesTemp)
{
	m_index = index ; 
	m_frequencies = frequencies;
	m_spectrum = spectrum;
	m_nodesTemp = nodesTemp;
}

PointLoad::~PointLoad()
{

}
bool PointLoad::setNodes()
{
	
	return true;
}



bool PointLoad::printLoad()
{
	cout << "Definition of load " << m_index << " on nodes " ;
	for(unsigned int i = 0; i < m_nodesTemp.size(); i++)
	{
		cout << m_nodesTemp[i] << ", " ;
	}
	cout << endl << "spectrum : "; 
	for(unsigned int i = 0; i < m_frequencies.size(); i++)
	{
		cout << m_spectrum[i] << " - " ;
	}
	return true;	
}



