#define __CL_ENABLE_EXCEPTIONS

#include "cl.hpp"
#include "util.hpp" // utility library
#include "device_picker.hpp"
#include "err_code.h"

#include <numeric>
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include"../src/characteristic_matrix.h"
#include <boost/filesystem.hpp>

// pick up device type from compiler command line or from the default type
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_CPU
#endif

using namespace std;
using namespace boost::filesystem;

int k = 5;

vector<vector <int> > CM;
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
    cout << "size cm: " << CM.size() << endl;
    cout << "size cm[0]: " << CM[0].size() << endl;
    double x = 0;
    double y = 0;
    for (int i = 0; i < CM.size(); i++) {
        for (int j = i+1; j < CM.size(); j++) {
            x = 0;
            y = 0;
            for (int k = 0; k < CM[i].size(); k++) {
              int aux = 0;
              if (CM[i][k]) aux++;
              if (CM[j][k]) aux++;
              if (aux == 2) x++;
              if (aux == 1) y++;
            }
            double jSim = x/(x+y);
            // cout << x << ", " << x+y << endl;
            cout << "Jaccard similarity(" << docs_names[i] << ", " << docs_names[j] << ") = " << jSim << endl;
        }
    }

    // double inter = 0.0;
    // double uni = 0.0;
    // for (int i = 0; i < CM.size(); i++) {
    //     for (int j = i+1; j < CM.size(); j++) {
    //         inter = 0.0;
    //         uni = 0.0;
    //         for (int k = 0; k < CM[i].size(); k++) {
    //           int prod = CM[i][k]*CM[j][k];
    //           int sum = CM[i][k]+CM[j][k];
    //           inter += prod;
    //           uni += sum - prod;
    //         }
    //         double jSim = inter/uni;
    //         cout << "Jaccard similarity(" << docs_names[i] << ", " << docs_names[j] << ") = " << jSim << endl;
    //     }
    // }

    // cl::Buffer buf_doc1;
    // cl::Buffer buf_doc2;
    // cl::Buffer buf_inter;
    // cl::Buffer buf_union;
    // vector <int> inter(CM[0].size(),0);
    // vector <int> uni(CM[0].size(),0);
    // try
    // {
    //     cl_uint deviceIndex = 0;
    //     // parseArguments(argc, argv, &deviceIndex);
    //     // Get list of devices
    //     std::vector<cl::Device> devices;
    //     unsigned numDevices = getDeviceList(devices);
    //     // Check device index in range
    //     if (deviceIndex >= numDevices)
    //     {
    //         std::cout << "Invalid device index (try '--list')\n";
    //         throw 1;
    //     }
    //     cl::Device device = devices[deviceIndex];
    //
    //     std::string name;
    //     getDeviceName(device, name);
    //     std::cout << "\nUsing OpenCL device: " << name << "\n";
    //
    //     std::vector<cl::Device> chosen_device;
    //     chosen_device.push_back(device);
    //     cl::Context context(chosen_device);
    //
    //     // Load in kernel source, creating a program object for the context
    //     cl::Program program(context, util::loadProgram("demo/write_sim.cl"), true);
    //     // Get the command queue
    //     cl::CommandQueue queue(context);
    //
    //     // Create the kernel functor
    //     auto write_sim = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer>(program, "write_sim");
    //     for (int i = 0; i < CM.size(); i++) {
    //         for (int j = i+1; j < CM.size(); j++) {
    //             buf_doc1 = cl::Buffer(context, begin(CM[i]), end(CM[i]), true);
    //             buf_doc2 = cl::Buffer(context, begin(CM[j]), end(CM[j]), true);
    //             buf_inter = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*CM[0].size());
    //             buf_union = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*CM[0].size());
    //             write_sim(cl::EnqueueArgs(queue, cl::NDRange(CM[i].size())), buf_doc1, buf_doc2, buf_inter,buf_union);
    //             queue.finish();
    //             cl::copy(queue, buf_inter, begin(inter), end(inter));
    //             cl::copy(queue, buf_union, begin(uni), end(uni));
    //             double rtime = 0.0;
    //             util::Timer timer;
    //             double sum_inter = accumulate(inter.begin(), inter.end(), 0);
    //             double sum_uni = accumulate(uni.begin(), uni.end(), 0);
    //             rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    //             printf("---------------------------------------------------------\n");
    //             printf("accumulate ran in %lf seconds\n", rtime);
    //             double jSim = sum_inter/sum_uni;
    //             cout << "Jaccard similarity(" << docs_names[i] << ", " << docs_names[j] << ") = " << jSim << endl;
    //         }
    //     }
    // }
    // catch (cl::Error err) {
    //     std::cout << "Exception\n";
    //     std::cerr
    //         << "ERROR: "
    //         << err.what()
    //         << "("
    //         << err_code(err.err())
    //         << ")"
    //         << std::endl;
    // }
}

int main(int argc, char *argv[]) {
    if (argc != 2 or !is_directory(status(argv[1]))){ cout << "Usage: Parameter must be 1 directory containing at least 2 files to compare" << endl; return 0;}
    iniParams();
    readDocuments(argv);
    Characteristic_Matrix cm(k,docs);
    CM = cm.getCM();
    double rtime = 0.0;
    util::Timer timer;
    writeSimilarity();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("writeSimilarity ran in %lf seconds\n", rtime);
}
