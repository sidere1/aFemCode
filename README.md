aFemCode, a FEM code. 

built initially only to learn cpp, and eventually to use for FSBC development 


on construit un objet
* FemCase
	attributs : 
	* mesh 
		* nodes
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
