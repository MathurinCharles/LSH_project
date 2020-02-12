#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<math.h>
#include"characteristic_matrix.h"
#include"minhash_signature.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;


int k = 5; //number of buckets of the hash table
int N = 500; //number of hash functions
int b = 100; //number of bands
int r = 5;   //number of row per band
double t;    //threshold

vector<string> docs_names;
int ndocs;

vector<vector<char> > docs;

map<pair<int,int>, double> candidates;

vector<vector <int> > CM;
vector<vector<int> > SM;


void iniParams(){
    //Initialization of parameters
    cout << "Which K are you using? (0: use K by default = " << k <<")" << endl;
    int auxK;
    cin >> auxK;
    if (auxK>0) k=auxK;

    cout << "How many bands are you using? (0: use B by default = " << b <<")" << endl;
    int auxB;
    cin >> auxB;
    if (auxB>0) b=auxB;
    cout << "How many files per band are you using? (0: use R by default = " << r <<")" << endl;
    int auxR;
    cin >> auxR;
    if (auxR>0) r=auxR;

    N = b*r;

    double b1 = b;
    double r1 = r;
    t = pow((1/b1),(1/r1)); //t ~ (1/b)^(1/r) [threshold auto-calculado]

    cout << "Which threshold are you using ? (0: use self-calculated T= " << t <<")" << endl;
    double auxT;
    cin >> auxT;
    if (auxT!=0) t=auxT;
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
    ndocs = docs_names.size();
}

double jaccardSimilarity(int d1, int d2) {
    double value = 0;
    for (int k = 0; k < N; k++) {
        if (SM[k][d1] == SM[k][d2]) value++;
    }
    value = value / N;
    return value;
}

void generateCandidates() {
    //Generate the candidates by the LSH
    hash<string> h; //to hash all rows of all bands.
    vector <vector<int> > buckets(b, vector<int> (ndocs)); //vector[i] -> vector<int> where each int is the bucket of the row r

    for (int band = 0; band < b; band++) {
        for (int c = 0; c < ndocs; c++) {
            char aux[r];
            for (int row = 0; row < r; row++) {
                aux[row] = SM[band*r+row][c];
            }
            //at this point, a single row is computed.
            string aux1 = aux;
            buckets[band][c] = h(aux1);
        }
    }

    //processing candidates;
    for (int band = 0; band < b; band++) {
        for (int c = 0; c < ndocs-1; c++) {
            for (int k = c+1; k < ndocs; k++) {
                if (buckets[band][c] == buckets[band][k]) {
                    pair<int,int> p;
                    p.first = c;
                    p.second = k;
                    candidates[p]= 0;
                }
            }
        }
    }

    std::map<pair<int,int>,double>::iterator it = candidates.begin();

    while (it != candidates.end()) {
        it->second = jaccardSimilarity(it->first.first, it->first.second);
        it++;
    }

}

void writeSimilarity() {
    //Write the Similarity of all candidates similar to the threshold on screen
    printf("---------------------------------------------------------\n");
    cout <<"Value of the threshold: "<< t << endl;
    int similars = 0;
    std::map<pair<int,int>,double>::iterator it = candidates.begin();
    while (it != candidates.end()) {
        if(it->second >= t){
            cout << "Similitude(" << docs_names[(it->first.first)] << ", " <<
            docs_names[(it->first.second)] << ") = " << it->second << endl;
            similars++;
        }
        it++;
    }
    if (similars == 0) cout << "There are no pairs of significantly similar documents."<<endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2 or !is_directory(status(argv[1]))){ cout << "Usage: Parameter must be 1 directory containing at least 2 files to compare" << endl; return 0;}
    iniParams();
    readDocuments(argv);
    Characteristic_Matrix cm(k,docs);
    CM = cm.getCM();
    MinHash_Signature sm(CM,N);
    SM = sm.getSM();
    generateCandidates();
    writeSimilarity();
}
