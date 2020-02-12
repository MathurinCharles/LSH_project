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
    // generateKShingles();
    // rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    // printf("---------------------------------------------------------\n");
    // printf("generateKShingles ran in %lf seconds\n", rtime);
    // timer.reset();
    computeCM();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("computeCM ran in %lf seconds\n", rtime);
}

vector<vector<int> > Characteristic_Matrix::getCM() {
    return CM;
}

void Characteristic_Matrix::generateKShingles() {
    //Generate the k-shingles of the two documents

    // for (int index=0; index<ndocs; index++){
    //     vector<char> kshingle(k);
    //    timer.reset();
    //     for (int i = 0; i < k; i++) {
    //         kshingle[i] = docs[index][i];
    //     }
    //
    //     string srt(kshingle.begin(), kshingle.end());
    //     kshings[index].push_back(srt);
    //
    //     for (int i = k; i < docs[index].size(); i++) {
    //         kshingle.erase(kshingle.begin());
    //         kshingle.push_back(docs[index][i]);
    //         string srt(kshingle.begin(), kshingle.end());
    //         kshings[index].push_back(srt);
    //     }
    // }
}

void Characteristic_Matrix::computeCM() {
    //Calculation of the Characteristic Matrix

    // hash<string> h;
    // int kshingsize = kshings[0].size();
    // for (int index=1; index<ndocs; index++){
    //    if (kshings[index].size() > kshingsize) kshingsize = kshings[index].size();
    // }
    // CM= vector<vector <int> >(kshingsize, vector<int> (ndocs,0));
    // for (int index=0; index<ndocs; index++){
    //     for (int i = 0; i < kshings[index].size(); i++) {
    //         CM[h(kshings[index][i]) % kshingsize][index] = 1;
    //     }
    // }

    int kshingsize = docs[0].size();
    for (int index=1; index<ndocs; index++){
       if (docs[index].size() > kshingsize) kshingsize = docs[index].size();
    }
    kshingsize = kshingsize -(k-1);
    cout << "kshingsize init: " << kshingsize << endl;
    kshings = vector<vector<string> > (ndocs);
    CM = vector<vector <int> >(ndocs, vector<int> (kshingsize,0));
    cl::Buffer doc;                        // device memory used for the input document vector
    cl::Buffer cm_row;

    try
    {
        cl_uint deviceIndex = 0;
        // parseArguments(argc, argv, &deviceIndex);
        // Get list of devices
        std::vector<cl::Device> devices;
        unsigned numDevices = getDeviceList(devices);
        // Check device index in range
        if (deviceIndex >= numDevices)
        {
            std::cout << "Invalid device index (try '--list')\n";
            throw 1;
        }
        cl::Device device = devices[deviceIndex];

        std::string name;
        getDeviceName(device, name);
        std::cout << "\nUsing OpenCL device: " << name << "\n";

        std::vector<cl::Device> chosen_device;
        chosen_device.push_back(device);
        cl::Context context(chosen_device);

        // Load in kernel source, creating a program object for the context
        cl::Program program(context, util::loadProgram("src/compute_cm.cl"), true);
        // Get the command queue
        cl::CommandQueue queue(context);

        // Create the kernel functor
        auto compute_cm = cl::make_kernel<cl::Buffer, cl::Buffer, int, int>(program, "compute_cm");

        for (int index=0; index<ndocs; index++){
            unsigned int n = docs[index].size();
            doc = cl::Buffer(context, begin(docs[index]), end(docs[index]), true);
            cm_row = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*kshingsize);
            compute_cm(cl::EnqueueArgs(queue, cl::NDRange(n-(k-1))), doc, cm_row, kshingsize,k);
            queue.finish();
            cl::copy(queue, cm_row, begin(CM[index]), end(CM[index]));
        }
    }
    catch (cl::Error err) {
        std::cout << "Exception\n";
        std::cerr
            << "ERROR: "
            << err.what()
            << "("
            << err_code(err.err())
            << ")"
            << std::endl;
    }
}
