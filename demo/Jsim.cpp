#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include"characteristic_matrix.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

#include "util.hpp" // utility library


int k = 5;

vector<vector <bool> > CM;
vector<vector<char> > docs;
vector<string> docs_names;

void iniParams(){
    //Initialize the parameters
    cout << "What K do you want to use? (0: use default K = " << k <<")" << endl;
    int aux;
    cin >> aux;
    if (aux>0) k=aux;
}

void readDocuments(char* argv[]) {
    //Read the documents
    ifstream fileReader;
    string out;//approximation

    path dir_path = argv[1];
    directory_iterator it{dir_path};
    while (it != directory_iterator{}){
        const string filename = argv[1]+it->path().leaf().string(); // full name = folder name + file name
        docs_names.push_back(it->path().leaf().string());
        fileReader.open(filename);
        vector<char> document;
        while (!fileReader.eof()) {
            fileReader >> out;
            for (int i = 0; i < out.size(); i++) document.push_back(out[i]);
            document.push_back(' '); //in case we must include ' ' as shingle.
        }
        docs.push_back(document);
        fileReader.close();
        it++;
    }
}

void writeSimilarity() {
    //write on the screen the Jaccard similarity from the caracteristic matrix.
    printf("---------------------------------------------------------\n");
    double x = 0.0;
    double y = 0.0;
    for (int i = 0; i < CM[0].size(); i++) {
        for (int j = i+1; j < CM[0].size(); j++) {
            for (int k = 0; k < CM.size(); k++) {
              int aux = 0;
              if (CM[k][i]) aux++;
              if (CM[k][j]) aux++;
              if (aux == 2) x++;
              if (aux == 1) y++;
            }
            double jSim = x/(x+y);
            cout << "Jaccard similarity(" << docs_names[i] << ", " << docs_names[j] << ") = " << jSim << endl;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 or !is_directory(status(argv[1]))){ cout << "Usage: Parameter must be 1 directory containing at least 2 files to compare" << endl; return 0;}
    iniParams();
    readDocuments(argv);
    double rtime = 0.0;
    util::Timer timer;
    Characteristic_Matrix cm(k,docs);
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("creation of charac matrix ran in %lf seconds\n", rtime);
    CM = cm.getCM();
    timer.reset();
    writeSimilarity();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("writeSimilarity ran in %lf seconds\n", rtime);
}
