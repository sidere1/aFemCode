#ifndef DEF_SETUP
#define DEF_SETUP

#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include "fstream"

class Setup
{
public:
    Setup();
    // Setup(std::string setupFile, std::string path);
    virtual ~Setup();

    // int addAtribute(int cursor, std::string entry, std::string value);
    bool set3D(bool dim);

    bool setInfo(std::string value);
    std::string getInfo() const;
    bool writeInfo(std::string info) const;

    bool setError(std::string value);
    std::string getError() const;
    bool writeError(std::string error) const;

    bool readFrequencies(std::string fileToRead, int end);
    bool readMicros(std::string fileToRead, int end);
    bool readLoads(std::string fileToRead);//, int end);

    // virtual int addAtribute(){return 0;};
    virtual int addAtribute(int cursor, std::string entry, std::string value) = 0;

    
	bool displayFrequencies();
    std::vector<double> getFrequencies();
    std::vector<int> getMics();
	double getC();

	virtual bool displayInfo() = 0;
	bool isLoaded();
    

protected:
    // general 
    std::string m_setupFile;
	std::string m_path;
	bool m_loaded;
    std::string m_info;
    std::string m_error;
    

    // Acoustics 
    std::vector<double> m_frequencies;
    std::vector<Node> m_micros;// pointeur sur des noeuds ? pas utilise
    std::vector<int> m_microsIndex;
    std::vector<PointLoad> m_loads;
    std::vector<int> m_loadsListTemp;
    bool m_3D;
    bool m_nodal; 
    bool timeOverMemory;
    double m_rho;
	double m_c;
    size_t m_couplingType; 

    // FSBC 
    size_t m_axis;
    size_t m_eta;
    size_t m_C;
    size_t m_N;
    size_t m_L;
    float m_radius;
    float m_Omega;

    //
	
	

};

#endif

/*

*/
