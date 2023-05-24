#ifndef DEF_ELEMENT
#define DEF_ELEMENT

#include <vector>

#include"Node.h"
#include "fMatrix.h"

class Element
{

public:
    Element();
    // Element(int type, int index, std::vector<Node*> nodes);
    Element(int index, std::vector<Node*> nodes, int feDescriptor, int physicalProperty, int materialProperty, int color, int numberOfNodes);
    // Element(int type, int index, std::vector<Node*> nodes, int feDescriptor, int physicalProperty, int materialProperty, int color, int numberOfNodes);
    ~Element();
    bool is1D() const ;
    bool is2D() const ;
    bool is3D() const ;
    bool calculateVolume();
    bool checkElem() const ;
    bool getDisplay() const ; 
    int getIndex() const;
    int getFeDescriptor() const;
	bool computeAspectRatio();
	int getAspectRatio() const;
	float getVolume() const;
	int getnN() const;
	std::vector<int> getNodesIds() const;
	fMatrix getCoordinates() const;
private:
    // int m_type;
    int m_index;
    std::vector<Node*> m_nodes; 
    int m_feDescriptor ; 
    int m_physicalProperty;
    int m_materialProperty;
    int m_color ;
    int m_numberOfNodes; 
    int m_aspectRatio;
  	float m_vol;
	bool m_1D;
	bool m_2D; 
	bool m_3D; 	
};

#endif

/*
FALSE ! OLDER VERSION. NOW WE STICK TO THE UNV STANDARDS BELOW
- An element has m_numberOfNodes nodes according to his type : 
- A type below 100 means that the element is 2D. Above 100, 3D. 
- 3D types are the following : 
	- 111 tetraeder 	m_numberOfNodes = 4
	- 112 tetraeder		m_numberOfNodes = 10
    - 121 hexaeder      m_numberOfNodes = 8
- 2D types are the following :
	- 11 triangle 		m_numberOfNodes = 3
	- 12 triangle    	m_numberOfNodes = 6
    - 21 quadrangle     m_numberOfNodes = 4
    - 22 quadrangle     m_numberOfNodes = 8
*/

/*



Universal Dataset Number 2412

Name:   Elements
Status: Current
Owner:  Simulation
Revision Date: 14-AUG-1992
-----------------------------------------------------------------------
 
Record 1:        FORMAT(6I10)
                 Field 1       -- element label
                 Field 2       -- fe descriptor id
                 Field 3       -- physical property table number
                 Field 4       -- material property table number
                 Field 5       -- color
                 Field 6       -- number of nodes on element
 
Record 2:  *** FOR NON-BEAM ELEMENTS ***
                 FORMAT(8I10)
                 Fields 1-n    -- node labels defining element
 
Record 2:  *** FOR BEAM ELEMENTS ONLY ***
                 FORMAT(3I10)
                 Field 1       -- beam orientation node number
                 Field 2       -- beam fore-end cross section number
                 Field 3       -- beam  aft-end cross section number
 
Record 3:  *** FOR BEAM ELEMENTS ONLY ***
                 FORMAT(8I10)
                 Fields 1-n    -- node labels defining element
 
Records 1 and 2 are repeated for each non-beam element in the model.
Records 1 - 3 are repeated for each beam element in the model.
 
Example:
 
    -1
  2412
         1        11         1      5380         7         2
         0         1         1
         1         2
         2        21         2      5380         7         2
         0         1         1
         3         4
         3        22         3      5380         7         2
         0         1         2
         5         6
         6        91         6      5380         7         3
        11        18        12
         9        95         6      5380         7         8
        22        25        29        30        31        26        24        23
        14       136         8         0         7         2
        53        54
        36       116        16      5380         7        20
       152       159       168       167       166       158       150       151
       154       170       169       153       157       161       173       172
       171       160       155       156
    -1

FE Descriptor Id definitions
____________________________

   11  Rod
   21  Linear beam
   22  Tapered beam
   23  Curved beam
   24  Parabolic beam
   31  Straight pipe
   32  Curved pipe
   41  Plane Stress Linear Triangle
   42  Plane Stress Parabolic Triangle
   43  Plane Stress Cubic Triangle
   44  Plane Stress Linear Quadrilateral
   45  Plane Stress Parabolic Quadrilateral
   46  Plane Strain Cubic Quadrilateral
   51  Plane Strain Linear Triangle
   52  Plane Strain Parabolic Triangle
   53  Plane Strain Cubic Triangle
   54  Plane Strain Linear Quadrilateral
   55  Plane Strain Parabolic Quadrilateral
   56  Plane Strain Cubic Quadrilateral
   61  Plate Linear Triangle
   62  Plate Parabolic Triangle
   63  Plate Cubic Triangle
   64  Plate Linear Quadrilateral
   65  Plate Parabolic Quadrilateral
   66  Plate Cubic Quadrilateral
   71  Membrane Linear Quadrilateral
   72  Membrane Parabolic Triangle
   73  Membrane Cubic Triangle
   74  Membrane Linear Triangle
   75  Membrane Parabolic Quadrilateral
   76  Membrane Cubic Quadrilateral
   81  Axisymetric Solid Linear Triangle
   82  Axisymetric Solid Parabolic Triangle
   84  Axisymetric Solid Linear Quadrilateral
   85  Axisymetric Solid Parabolic Quadrilateral
   91  Thin Shell Linear Triangle
   92  Thin Shell Parabolic Triangle
   93  Thin Shell Cubic Triangle
   94  Thin Shell Linear Quadrilateral
   95  Thin Shell Parabolic Quadrilateral
   96  Thin Shell Cubic Quadrilateral
   101 Thick Shell Linear Wedge
   102 Thick Shell Parabolic Wedge
   103 Thick Shell Cubic Wedge
   104 Thick Shell Linear Brick
   105 Thick Shell Parabolic Brick
   106 Thick Shell Cubic Brick
   111 Solid Linear Tetrahedron
   112 Solid Linear Wedge
   113 Solid Parabolic Wedge
   114 Solid Cubic Wedge
   115 Solid Linear Brick
   116 Solid Parabolic Brick
   117 Solid Cubic Brick
   118 Solid Parabolic Tetrahedron
   121 Rigid Bar
   122 Rigid Element
   136 Node To Node Translational Spring
   137 Node To Node Rotational Spring
   138 Node To Ground Translational Spring
   139 Node To Ground Rotational Spring
   141 Node To Node Damper
   142 Node To Gound Damper
   151 Node To Node Gap
   152 Node To Ground Gap
   161 Lumped Mass
   171 Axisymetric Linear Shell
   172 Axisymetric Parabolic Shell
   181 Constraint
   191 Plastic Cold Runner
   192 Plastic Hot Runner
   193 Plastic Water Line
   194 Plastic Fountain
   195 Plastic Baffle
   196 Plastic Rod Heater
   201 Linear node-to-node interface
   202 Linear edge-to-edge interface
   203 Parabolic edge-to-edge interface
   204 Linear face-to-face interface
   208 Parabolic face-to-face interface
   212 Linear axisymmetric interface
   213 Parabolic axisymmetric interface
   221 Linear rigid surface
   222 Parabolic rigin surface
   231 Axisymetric linear rigid surface
   232 Axisymentric parabolic rigid surface

------------------------------------------------------------------------------

Notice of Non-Discrimination Copyright Information © University of Cincinnati 2008-2019

University of Cincinnati Department of Mechanical and Materials Engineering

2600 Clifton Ave., Cincinnati, Ohio 45221 | Phone 513 556 2738
*/
