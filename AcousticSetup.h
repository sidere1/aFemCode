#ifndef DEF_ACOUSTICSETUP
#define DEF_ACOUSTICSETUP

#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include "fstream"
#include "Setup.h"

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
