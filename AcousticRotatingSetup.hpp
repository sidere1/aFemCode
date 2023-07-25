#ifndef DEF_ACOUSTICROTATINGSETUP
#define DEF_ACOUSTICROTATINGSETUP

#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include "fstream"
#include "Setup.h"

class AcousticRotatingSetup: public Setup 
{
public:
    AcousticRotatingSetup();
    AcousticRotatingSetup(std::string setupFile, std::string path);

    ~AcousticRotatingSetup();

    virtual int addAtribute(int cursor, std::string entry, std::string value);
	virtual bool displayInfo();

protected:    
    
};

#endif

/*

*/
