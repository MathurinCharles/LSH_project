#include"characteristic_matrix.h"
#include "util.hpp" // utility library

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
    hashShingles();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("hashShingles ran in %lf seconds\n", rtime);
    timer.reset();
    generateUniversal();
    rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    printf("---------------------------------------------------------\n");
    printf("generateUniversal ran in %lf seconds\n", rtime);
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

void Characteristic_Matrix::hashShingles() {
    //Make Hashing of the k-shingles
    hash<string> h;
    buckets = vector<map<int,int> > (ndocs);
    int kshingsize = kshings[0].size();
    for (int index=1; index<ndocs; index++){
       if (kshings[index].size() > kshingsize) kshingsize = kshings[index].size();
    }

    for (int index=0; index<ndocs; index++){
        for (int i = 0; i < kshings[index].size(); i++) {
            buckets[index][h(kshings[index][i]) % kshingsize] = h(kshings[index][i]) % kshingsize;
            // buckets[index][h(kshings[index][i])] = h(kshings[index][i]);
        }
    }
}

void Characteristic_Matrix::generateUniversal() {
    //Generate the Universal k-shingle
    for (int index=0; index<ndocs; index++){
        std::map<int,int>::iterator it=buckets[index].begin();
        while ((it != buckets[index].end())) {
            // universal_kshingles[it->first] = it->first;
            universal_kshingles[it->first] = it->second;
            it++;
        }
    }
}

void Characteristic_Matrix::computeCM() {
    //Calculation of the Characteristic Matrix
    CM= vector<vector <bool> >(universal_kshingles.size(), vector<bool> (ndocs,false));
    std::map<int,int>::iterator u_it = universal_kshingles.begin();
    int i_uni = 0;

    // try
    // {
    //     cl_uint deviceIndex = 0;
    //     parseArguments(argc, argv, &deviceIndex);
    //
    //     // Get list of devices
    //     std::vector<cl::Device> devices;
    //     unsigned numDevices = getDeviceList(devices);
    //
    //     // Check device index in range
    //     if (deviceIndex >= numDevices)
    //     {
    //         std::cout << "Invalid device index (try '--list')\n";
    //         return EXIT_FAILURE;
    //     }
    //
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
    //     cl::Program program(context, util::loadProgram("compute.cl"), true);
    //
    //     // Get the command queue
    //     cl::CommandQueue queue(context);
    //
    //     // Create the kernel functor
    //     auto vadd = cl::make_kernel<cl::Buffer, cl::Buffer, int, int>(program, "computecm");
    //
    //     d_b = cl::Buffer(context, buckets.begin(), buckets.end(), true);
    //     d_cm = cl::Buffer(context, CM.begin(), CM.end(), false);
    //
    //     util::Timer timer;
    //
    //     computecm(cl::EnqueueArgs(queue, cl::NDRange(ndocs)), d_b, d_cm)
    //
    //     vadd(
    //             cl::EnqueueArgs(
    //                 queue,
    //                 cl::NDRange(count)),
    //             d_a,
    //             d_b,
    //             d_c,
    //             count);
    //
    //     queue.finish();
    //
    //     double rtime = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    //     printf("\nThe kernels ran in %lf seconds\n", rtime);
    //
    //     cl::copy(queue, d_c, begin(h_c), end(h_c));
    //
    //
    //     // ------------------------------------------------------------------
    //     // Setup the buffers, initialize matrices, and write them into global memory
    //     // ------------------------------------------------------------------
    //
    //
    //     // ------------------------------------------------------------------
    //     // OpenCL matrix multiplication ... Naive
    //     // ------------------------------------------------------------------
    //
    //
    //     // Load in kernel source, creating a program object for the context
    //     cl::Program program(context, util::loadProgram("computecm.cl"), true);
    //
    //     // Create the compute kernel from the program
    //     cl::Kernel kernel_cm = cl::Kernel(program, "computecm");
    //
    //     // Display max group size for execution
    //     std::cout << "\nWork Group Size " << kernel_cm.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device) << std::endl;
    //     std::cout << "Work Group Memory size " << kernel_cm.getWorkGroupInfo<CL_KERNEL_LOCAL_MEM_SIZE>(device) << std::endl;
    //
    //     std::cout << "\n===== OpenCL, matrix mult, C(i,j) per work item, order %d ======" << N << std::endl;
    //
    //     // Do the multiplication COUNT times
    //     for (int i = 0; i < COUNT; i++)
    //     {
    //         // Set output matrix to 0
    //         zero_mat(N, h_C);
    //
    //         // Initialize arguments of kernel
    //         kernel_cm.setArg(0, N);
    //         kernel_cm.setArg(1, d_a);
    //         kernel_cm.setArg(2, d_b);
    //         kernel_cm.setArg(3, d_c);
    //
    //         // Set workspace and workgroup topologies
    //         cl::NDRange global(N, N);
    //         cl::NDRange local(32, 32);
    //
    //         start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
    //
    //         // Execute the kernel over the entire range of C matrix elements ... computing
    //         // a dot product for each element of the product matrix.  The local work
    //         // group size is set to NULL ... so I'm telling the OpenCL runtime to
    //         // figure out a local work group size for me.
    //
    //         queue.enqueueNDRangeKernel(kernel_cm, cl::NullRange, global, local);
    //
    //         queue.finish();
    //
    //         run_time = (static_cast<double>(timer.getTimeMilliseconds()) / 1000.0) - start_time;
    //
    //         cl::copy(queue, d_c, h_C.begin(), h_C.end());
    //
    //         results(N, h_C, run_time);
    //
    //     } // end for loop
    // }
    // catch (cl::Error err)
    // {
    //     std::cout << "Exception\n";
    //     std::cerr << "ERROR: "
    //               << err.what()
    //               << "("
    //               << err_code(err.err())
    //               << ")"
    //               << std::endl;
    // }

    while (u_it != universal_kshingles.end()) { //row iterator

        for (int index = 0; index < ndocs; index++) {//column iterator
            if (buckets[index].find(u_it->second) != buckets[index].end()) { //means element is in Set1
                CM[i_uni][index] = true;
            }
        }
        ++i_uni;
        ++u_it;
    }
}
