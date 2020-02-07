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
    timer.reset();
}

vector<vector<bool> > Characteristic_Matrix::getCM() {
    return CM;
}

void Characteristic_Matrix::generateKShingles() {
    //Generate the k-shingles of the two documents
    kshings = vector<vector<string> > (ndocs);

    cl::Buffer doc;                        // device memory used for the input  document vector
    cl::Buffer shingles;                       // device memory used for the output shingles vector

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
        std::cout << 1 << std::endl;
        cl::Program program(context, util::loadProgram("src/generate_shingles.cl"), true);
        std::cout << 2 << std::endl;
        // Get the command queue
        cl::CommandQueue queue(context);

        // Create the kernel functor
        auto generate_shingles = cl::make_kernel<cl::Buffer, cl::Buffer, int>(program, "generate_shingles");

        for (int index=0; index<ndocs; index++){
            unsigned int n = docs[index].size();
            doc = cl::Buffer(context, begin(docs[index]), end(docs[index]), true);
            shingles = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(char)*k*(n-(k-1)));
            std::cout << index << std::endl;
            generate_shingles(cl::EnqueueArgs(queue, cl::NDRange(n-(k-1))), doc, shingles,k);
            printf("reussi");
            queue.finish();
            cl::copy(queue, shingles, begin(kshings[index]), end(kshings[index]));
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

    // for (int index=0; index<ndocs; index++){
    //     vector<char> kshingle(k);
    //
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
