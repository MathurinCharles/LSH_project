#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include"characteristic_matrix.h"
#include"minhash_signature.h"

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;


int k = 5;
int N = 500; //number of hash functions

vector<string> docs_names;

vector<vector<char> > docs;

vector<vector <bool> > CM;
vector<vector<int> > SM;

vector<int> coefs;  //ith position stores Coefficient  of ith hash function.
vector<int> tInd;   //ith position stores Independent of ith hash function.

void iniParams(){
    //Initialize the Parameters
    cout << "What K do you want to use? (0: use default K =" << k <<")" << endl;
    int auxK;
    cin >> auxK;
    if (auxK>0) k=auxK;
    cout << "How many hash features do you want to use? (0: use N by default =" << N <<")" << endl;
    int auxN;
    cin >> auxN;
    if (auxN>0) N=auxN;
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
    // Type on the screen the Similarity of all pairs of documents
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < SM[0].size(); i++) {
        for (int j = i+1; j < SM[0].size(); j++) {
            double value = 0;
            for (int k = 0; k < N; k++) {
                if (SM[k][i] == SM[k][j]) value++;
            }
            value = value / N;
            cout << "Similarity(" << docs_names[i] << ", " << docs_names[j]  << ") = " << value << endl;
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2 or !is_directory(status(argv[1]))){ cout << "Usage: Parameter must be 1 directory containing at least 2 files to compare" << endl; return 0;}
    iniParams();
    readDocuments(argv);
    Characteristic_Matrix cm(k,docs);
    CM = cm.getCM();
    MinHash_Signature sm(CM,N);
    SM = sm.getSM();
    writeSimilarity();
}
