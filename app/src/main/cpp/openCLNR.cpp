//
//  openCLNR.cpp
//  OpenCL Example1
//
//  Created by Rasmusson, Jim on 18/03/13.
//
//  Copyright (c) 2013, Sony Mobile Communications AB
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Sony Mobile Communications AB nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#define __CL_ENABLE_EXCEPTIONS

#include "openCLNR.h"

inline std::string loadProgram(std::string input) {
    std::ifstream stream(input.c_str());
    if (!stream.is_open()) {
        LOGE("Cannot open input file\n");
        exit(1);
    }
    return std::string(std::istreambuf_iterator<char>(stream),
                       (std::istreambuf_iterator<char>()));
}

void openCLNR(unsigned char *bufIn, unsigned char *bufOut, int *info) {

    LOGI("\n\nStart openCLNR (i.e., OpenCL on the GPU)");

    int width = info[0];
    int height = info[1];
    unsigned int imageSize = width * height * 4 * sizeof(cl_uchar);

    cl_int err = CL_SUCCESS;
    try {

        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            std::cout << "Platform size 0\n";
            return;
        }

        cl_context_properties properties[] =
                {CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, properties);

        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        cl::CommandQueue queue(context, devices[0], 0, &err);

        std::string kernelSource = loadProgram(
                "/data/data/com.cscao.apps.opencldemo/app_execdir/bilateralKernel.cl");

        cl::Program::Sources source(1, std::make_pair(kernelSource.c_str(),
                                                      kernelSource.length() + 1));
        cl::Program program(context, source);
        const char *options = "-cl-fast-relaxed-math";
        program.build(devices, options);

        cl::Kernel kernel(program, "bilateralFilterKernel", &err);
        cl::Buffer bufferIn = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                         imageSize, (void *) &bufIn[0], &err);
        cl::Buffer bufferOut = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          imageSize, (void *) &bufOut[0], &err);

        kernel.setArg(0, bufferIn);
        kernel.setArg(1, bufferOut);
        kernel.setArg(2, width);
        kernel.setArg(3, height);

        cl::Event event;

        clock_t startTimer1, stopTimer1;
        startTimer1 = clock();

// 		one time
        queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   cl::NDRange(width, height),
                                   cl::NullRange,
                                   NULL,
                                   &event);

//		swap in and out buffer pointers and run a 2nd time
        kernel.setArg(0, bufferOut);
        kernel.setArg(1, bufferIn);
        queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   cl::NDRange(width, height),
                                   cl::NullRange,
                                   NULL,
                                   &event);

//		swap in and out buffer pointers and run a 3rd time
        kernel.setArg(0, bufferIn);
        kernel.setArg(1, bufferOut);
        queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   cl::NDRange(width, height),
                                   cl::NullRange,
                                   NULL,
                                   &event);

        queue.finish();

        stopTimer1 = clock();
        double elapse = 1000.0 * (double) (stopTimer1 - startTimer1) / (double) CLOCKS_PER_SEC;
        info[2] = (int) elapse;
        LOGI("OpenCL code on the GPU took %g ms\n\n",
             1000.0 * (double) (stopTimer1 - startTimer1) / (double) CLOCKS_PER_SEC);

        queue.enqueueReadBuffer(bufferOut, CL_TRUE, 0, imageSize, bufOut);
    }
    catch (cl::Error err) {
        LOGE("ERROR: %s\n", err.what());
    }
    return;
}
