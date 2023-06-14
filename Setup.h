#ifndef DEF_SETUP
#define DEF_SETUP

#include "Mesh.h"
#include "Node.h"
#include "PointLoad.h"
#include "fstream"

class Setup
{
public:
    Setup();
    Setup(std::string setupFile, std::string path);
    ~Setup();

    int addAtribute(int cursor, std::string entry, std::string value);
    bool set3D(bool dim);

    bool setInfo(std::string value);
    std::string getInfo() const;
    bool writeInfo(std::string info) const;

    bool setError(std::string value);
    std::string getError() const;
    bool writeError(std::string error) const;

    //bool setMeshFile(std::string meshFile);
    //std::string getMeshFile();

    bool readFrequencies(std::string fileToRead, int end);
    bool readMicros(std::string fileToRead, int end);
    bool readLoads(std::string fileToRead);//, int end);
    
	bool displayFrequencies();
    vector<double> getFrequencies();
    vector<int> getMics();
	double getC();

	bool displayInfo();
	bool isLoaded();
    //bool buildH();
    //bool printH() const;


    //bool writeResults();
    //bool performAcoustic3DNodalResolution() const;

private:
    std::string m_setupFile;
	std::string m_path;
	bool m_loaded;
	//Mesh m_mesh;
    std::vector<double> m_frequencies;
    std::vector<Node> m_micros;// pointeur sur des noeuds ? pas utilise
    std::vector<int> m_microsIndex;
    std::vector<PointLoad> m_loads;
    std::vector<int> m_loadsListTemp;
    bool m_3D;
    bool m_nodal; 
    bool timeOverMemory;
    double m_rho;
	double m_c;
	
	std::string m_info;
    std::string m_error;
    //std::string m_meshFile;
    //Eigen::MatrixXd m_H;
    //Eigen::SparseMatrix<double> m_HSparse;

    // Mesh, 3D, freq, micros, loadCases, algo, sources, paramètre supplémentaire, nodal/modal 
    // Objet flux pour les erreurs, logs en tout genre, etc, qu'on peut passer en paramètre aux différents objets dessous 
    // voire 2 flux, un pour les logs, un pour les erreurs. Eventuellement les deux sont les mêmes.   
    // un mode verbose pour savoir si ce flux va vers cout pour débugger ou vers un fichier au cas où  

};

#endif

/*

*/
