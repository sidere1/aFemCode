#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#define WHEREAMI cout << endl << "no crash until line " << __LINE__ << " in the file " __FILE__ << endl << endl;


#include "fLinSys.h"

using namespace std;

int main(void)
{	
	srand (time(NULL));
	cout << "Hello !" << endl ; 

	int N(10);
	fLinSys a(N, 1);
	//a.buildLU();	
	//a.solve();
	//a.printLU();	
	cout << endl << endl ; 
	
	cout << "done initializing" << endl; 

	cout << "Fin du programme " << endl << endl;
	return 0;
}

/*
//Objet maillage  
connectivites, cooordonnees, groupes 

enregistrer au format unv, lire au même format... 




//objet table des coordonnées 
nb de noeuds
tableau de noeuds




//objet table des connectivites
nb d'elements, differents types 
differents tableaux de types 


//Objet élément 
type (-> nb de noeuds ) -> sous classes 
liste de noeuds 

//objet table des groupes 
nombre de groupes
liste de groupes 

//objet groupe 
nombre d'eléments
type d'éléments
liste des éléments 
*/


/*

EIGEN 

multiplication * 
solving, with A = I(1 000) : 
16.2				x = A.fullPivLu().solve(b);
3.7					x = A.householderQr().solve(b);
https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html for more info 

produit scalaire a.dot(b)
produit vectoriel a.cross(b)

*/
