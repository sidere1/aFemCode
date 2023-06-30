aFemCode is a FEM code. 

It was used initially only to learn cpp, and eventually to use for FSBC development 
documentation available at : https://afemcode.readthedocs.io/en/latest/content.html

on construit un objet
* FemCase
	attributs : 
	* mesh // une liste de maillages plutot ? 
		* Node
			* id
			* [x y z] 
			* groupId 
		* elements 
		* renumbering() pour séparer les groupes en vue de la suite ? 
	* fLinSys
		* la matrice c'est un pointeur vers une matrice ?
	* storedResults
		* K, M, T, B, F, currentSys (k-k^2M), etc.
	* setup
		* tous les parametres physiques, lus dans un fichier .femCase (faut que ce soit réutilisable pour les cas couplés) 
		* 
	méthodes : 
	* assemble() 
	* export()

le programme principal devrait ressembler à ça : 

* fc.s.read();
* fc.m.read();
* fc.m.prepare(); // pour renuméroter, calculer les volumes, les aspect ratios, les jacobiens, tout ce qui peut etre prepare en amont 
* fc.assemble();
* fc.s.solve();
* fc.export();

pour un FSBC : 

* CoupledFemCases
	attributs : 
	* tableau de pointeurs sur des FemCases
	* setup 
		* uniquement les paramètres liés à la rotation
		* une vérification que les cas test sont compatibles, mêmes fréquences, même tout 




remarques : 
* pour désigner la source, une locationInMesh et recherche du noeud le plus proche plutot qu'un node id
* les noeuds sont forcément donnés avec 3 coordonnées, même pour un cas en 2D 
* uniformiser et généraliser les writeInfos et les writeError, mais ou ? femCase ? 
* un gestion des sources dirac qui facilite l'aéro : une liste de points source avec spectres associés. Comme ça, une source, plusieurs sources, ou des sources partout, c'est la même chose. 
* la fonction Element::computeAspectRatio n'est pas écrite, il faut la remplir et mettre un warning si l'élément n'est pas supporté.


