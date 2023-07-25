#ifndef DEF_LOAD
#define DEF_LOAD

#include "Node.h"
#include <vector>
#include <complex>

class PointLoad
{
public:
    PointLoad();
    PointLoad(int index, std::vector<double> frequencies, std::vector<std::complex<double> > spectrum, std::vector<int> nodesTemp);
    ~PointLoad();
    bool setNodes();
    bool printLoad();
    // int getIndex();
    // bool getDisplay() const;
    // float getX() const;
    // float getY() const;
    // float getZ() const;

private:
    int m_index;
    std::vector<double> m_frequencies;
    std::vector<std::complex<double> > m_spectrum;
    std::vector<int> m_nodesTemp;
    std::vector<Node*> m_nodes;

};

#endif

/*

*/
