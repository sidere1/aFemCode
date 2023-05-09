#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <cassert>

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


#include "fLinSys.h"
#include "Node.h"
#include "Element.h"
#include "Mesh.h"
#include "PointLoad.h"
#include "FemCase.h"


using namespace std;

int main(int argc, char** argv)
{
	cout << endl << endl; 	
	assert(argc == 2 && "please provide the name of the setup file as parameter in the command line");

	// il faut vérifier que les dossiers existent, les crééer sinon, et enregistrer le chemin dans un attribut : c'est fait dans le constructeur de FemCase  
	string setupFile (argv[1]);
	cout << "Reading setup file " << argv[1] << endl; 
	FemCase fc(setupFile);
	fc.displayInfo();	
	fc.prepareComputation();	
	srand (time(NULL));

	int N(3);
	fLinSys a(N, 1);
	//a.buildLU();	
	//a.solve();
	//a.printMatrix();	
	//a.printLU();


	
	//cout << "Reading Mesh" << endl;
	//Mesh m1("log.info", "log.warning");
	//Mesh m2("log.info", "log.warning");
	//m1.unvImport("/home/silouane/Documents/femCases/stupidCircle/stupidCircle_noBeams.unv");
	//m2.unvImport("/home/silouane/Documents/femCases/stupidCircle/stupidCircle.unv");
	

	cout << "Fin du programme " << endl << endl;
	return 0;
}


