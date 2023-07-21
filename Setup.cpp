#include <iostream>
#include "Setup.h"
#include "Node.h"
#include "PointLoad.h"
 

using namespace std;


#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


Setup::Setup()
{}

Setup::~Setup()
{}


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



bool Setup::readRotatingParams(string fileToRead)
{
    ifstream paramFile(fileToRead.c_str());
    unsigned int index(0);
    string devNull;

    writeInfo("Reading microphones");

    if (paramFile) 
    {
        do
        {//reading header 
            paramFile >> devNull ;
            index++;
            // cout << index << " ignoring " << devNull << endl;
        }while(devNull.compare("Omega") != 0 && index < 10);

        if (index == 10)
        {// checking that the last loop went well
            writeError("Please check the structure of your .femRot file");
        }
        WHEREAMI
        paramFile >> m_Omega;
        paramFile >> devNull; // axis 
        paramFile >> m_axis;
        paramFile >> devNull; // radius 
        paramFile >> m_radius;
        paramFile >> devNull; // eta 
        paramFile >> m_eta;
        paramFile >> devNull; // C 
        paramFile >> m_C;
        paramFile >> devNull; // N 
        paramFile >> m_N;
        paramFile >> devNull; // L 
        paramFile >> m_L;
        cout << "Omega : " << m_Omega << endl;
        cout << "Axis : " << m_axis << endl;
        
        writeInfo("  rotating params -> Ok");
        return true;
    }
    else
    {
        cout << "Your rotating parameter file " << fileToRead << " hasn't been found or opened" << endl;    
        writeError("I did'nt manage to read the rotating parameters \n\n");
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
	return m_microsIndex;
}

double Setup::getC()
{
	return m_c;
}

// bool Setup::displayInfo()
// {
// 	cout << m_frequencies.size() << " frequencies" << endl; 
// 	cout << m_microsIndex.size() << " micros " << endl;  
// 	cout << m_loads.size() << " loads " << endl;  
// 	cout << "rho " << m_rho << endl;  
// 	cout << "c" << m_c << endl; 
//    	cout << endl;
// 	//for (int i = 0; i < m_nCoupling; i++)
// 	//{
// 	//	cout << i+1 << " : " << m_meshFile[i] << " ; " << m_setupFile[i] << " ; type " << m_couplingType[i] << endl; 
// 	//}
// 	return true; 
// }

bool Setup::isLoaded()
{
	return m_loaded;
}
