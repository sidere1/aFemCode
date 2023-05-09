#ifndef DEF_FEMCASE
#define DEF_FEMCASE

#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include "fstream"
#include "Setup.h"
#include "fMatrix.h"
#include <vector> 

class FemCase
{
public:
    FemCase();
    FemCase(std::string setupFile);
    ~FemCase();

    int addAtribute(int cursor, std::string entry, std::string value);

    bool setInfo(std::string value);
    std::string getInfo() const;
    bool writeInfo(std::string info) const;

    bool setError(std::string value);
    std::string getError() const;
    bool writeError(std::string error) const;

    bool setMeshFile(std::string meshFile, int iCoupling);
    std::string getMeshFile(int iCoupling);


	bool displayInfo(); // for debugging purposes 
	
	bool prepareComputation(); // read meshn, renumber, compute volumes, aspect ratios, jacobs, etc. 
	bool buildFLinSys(); // assemblage
	bool buildK(); 
	bool buildM();
	bool buildB();
	bool buildF();
	bool performCoupling(); // pour la projection - construction du systeme couple 
	bool performResolution(); // solve 
	bool exportResults();

    //bool writeResults();
    //bool performAcoustic3DNodalResolution() const;

private:
    std::string m_info;
    std::string m_error;
	int m_nCoupling;
    std::string m_mainSetup;
	std::vector<std::string> m_meshFile; 
	std::vector<Mesh*> m_mesh; 
	std::vector<std::string> m_setupFile; 
	std::vector<Setup> m_setup; 
	std::vector<int> m_couplingType; // 1 : harmonic acoustics, 2 : time acoustics, ... 
	std::string m_path;
	fMatrix K;
	fMatrix M;
	fMatrix F;
	fMatrix B;
	fMatrix currentSys;
};

#endif

/*

*/
