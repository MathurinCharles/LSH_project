#ifndef CHARACTERISTIC_MATRIX_H
#define CHARACTERISTIC_MATRIX_H

 // CHARACTERISTIC_MATRIX_H


#include<vector>
#include<map>
#include<string>
#include<functional>
#include<fstream>
#include<cstdlib>

#include<iostream>

using namespace std;

class Characteristic_Matrix {


private:
    int k;
    int ndocs;
    int kshingsize;
    vector<vector<char> > docs;
    vector< vector<string> >kshings;
    vector<vector <int> > CM;

    void generateKShingles();

    void computeCM();



public:

    Characteristic_Matrix(int k, vector<vector<char> > docs);
    vector<vector<int> > getCM();

};

#endif
