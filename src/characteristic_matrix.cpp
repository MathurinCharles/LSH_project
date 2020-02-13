#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"
#include "util.hpp" // utility library
#include "device_picker.hpp"
#include "err_code.h"
#include"characteristic_matrix.h"

// pick up device type from compiler command line or from the default type
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_CPU
#endif

//PUBLIC METHODS
Characteristic_Matrix::Characteristic_Matrix(int k, vector<vector<char> > docs) {
    this->k = k;
    this->docs = docs;
    ndocs = docs.size();
    double rtime = 0.0;
    util::Timer timer;
    generateKShingles();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("generateKShingles ran in %lf seconds\n", rtime);
    timer.reset();
    computeCM();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("computeCM ran in %lf seconds\n", rtime);
}

vector<vector<bool> > Characteristic_Matrix::getCM() {
    return CM;
}

void Characteristic_Matrix::generateKShingles() {
    //Generate the k-shingles of the two documents
    kshings = vector<vector<string> > (ndocs);

    for (int index=0; index<ndocs; index++){
        vector<char> kshingle(k);

        for (int i = 0; i < k; i++) {
            kshingle[i] = docs[index][i];
        }

        string srt(kshingle.begin(), kshingle.end());
        kshings[index].push_back(srt);

        for (int i = k; i < docs[index].size(); i++) {
            kshingle.erase(kshingle.begin());
            kshingle.push_back(docs[index][i]);
            string srt(kshingle.begin(), kshingle.end());
            kshings[index].push_back(srt);
        }

    }


}

void Characteristic_Matrix::computeCM() {
    //Calculation of the Characteristic Matrix
    hash<string> h;
    int kshingsize = kshings[0].size();
    for (int index=1; index<ndocs; index++){
       if (kshings[index].size() > kshingsize) kshingsize = kshings[index].size();
    }
    CM= vector<vector <bool> >(kshingsize, vector<bool> (ndocs,false));
    for (int index=0; index<ndocs; index++){
        for (int i = 0; i < kshings[index].size(); i++) {
            CM[h(kshings[index][i]) % kshingsize][index] = true;
        }
    }
}
