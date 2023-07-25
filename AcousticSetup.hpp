#ifndef DEF_ACOUSTICSETUP
#define DEF_ACOUSTICSETUP

#include "Mesh.hpp"
#include "Node.hpp"
#include "PointLoad.hpp"
#include "fstream"
#include "Setup.hpp"

class AcousticSetup: public Setup 
{
public:
    AcousticSetup();
    AcousticSetup(std::string setupFile, std::string path);

    ~AcousticSetup();

    virtual int addAtribute(int cursor, std::string entry, std::string value);
	virtual bool displayInfo();

private:
    
};

#endif

/*

*/
