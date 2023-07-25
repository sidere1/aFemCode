#ifndef DEF_ACOUSTICROTATINGSETUP
#define DEF_ACOUSTICROTATINGSETUP

#include "Mesh.hpp"
#include "Node.hpp"
#include "PointLoad.hpp"
#include "fstream"
#include "Setup.hpp"

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
