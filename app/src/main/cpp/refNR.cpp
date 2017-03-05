//
//  refNR.cpp
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

#include "refNR.h"

#define CLAMP(x, ll, ul) (((x)<(ll)) ? (ll) : (((x) >(ul)) ? (ul) : (x)))

#define oneOver255 0.00392156862745f
#define sigmaDomain 3.0f
#define sigmaRange  0.2f

#define filterWidth 2

void bilateralFilter(float *srcPixels, float *dstPixels,
                     const int width, const int height) {
    //loop over the image
    for (int y = 0; y < height-1; y++) {
        for (int x = 0; x < width-1; x++) {

            if ((x >= filterWidth) && (x < (width - filterWidth)) &&
                //avoid reading outside of buffer
                (y >= filterWidth) && (y < (height - filterWidth))) {
                int centerIndex = 4 * (y * width + x);
                float centerPixel[4] = {srcPixels[centerIndex], srcPixels[centerIndex + 1],
                                        srcPixels[centerIndex + 2], srcPixels[centerIndex + 3]};
                float sum4[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                float normalizeCoeff = 0.0f;

                for (int yy = -filterWidth; yy <= filterWidth; yy++) {
                    for (int xx = -filterWidth; xx <= filterWidth; xx++) {
                        int thisIndex = 4 * ((y + yy) * width + (x + xx));
                        float currentPixel[4] = {srcPixels[thisIndex], srcPixels[thisIndex + 1],
                                                 srcPixels[thisIndex + 2],
                                                 srcPixels[thisIndex + 3]};
                        double domainDistance = sqrt(
                                (float) (xx) * (float) (xx) + (float) (yy) * (float) (yy));
                        double domainWeight = exp(-0.5f * pow((domainDistance / sigmaDomain), 2.0f));

                        double rangeDistance = 0.0f;
                        for (int c = 0; c < 3; ++c)
                            rangeDistance += (currentPixel[c] - centerPixel[c]) *
                                             (currentPixel[c] - centerPixel[c]);

                        rangeDistance = sqrt(rangeDistance);
                        double rangeWeight = exp(-0.5f * pow((rangeDistance / sigmaRange), 2.0f));

                        double totalWeight = domainWeight * rangeWeight;
                        normalizeCoeff += totalWeight;

                        for (int c = 0; c < 4; c++)
                            sum4[c] += totalWeight * currentPixel[c];
                    }
                }

                for (int c = 0; c < 3; c++)
                    dstPixels[centerIndex + c] = CLAMP((sum4[c] / normalizeCoeff), 0.0f, 1.0f);
                dstPixels[centerIndex + 3] = 1.0f; // set alpha to fully opaque
            }
        }
    }

}

void refNR(unsigned char *bufIn, unsigned char *bufOut, int *info) {
    LOGI("\n\nStart refNR (i.e., CPU native plain C code)");

    int width, height;
    clock_t startTimer, stopTimer;

    width = info[0];
    height = info[1];
	LOGI("image width = %d, image height = %d\n", width, height);

    float *srcPixels = new float[width * height * 4];
    float *dstPixels = new float[width * height * 4];

    //convert input image buffer to float
    int index1 = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 4; c++) {
                srcPixels[index1] = oneOver255 * (float) bufIn[index1];
                //dstPixels[index1] = 0;// reset dstPixels
                index1++;
            }
        }
    }
    LOGI("index1= %d \n", index1);

    startTimer = clock();
    //run the bilateral filter three times to get a stronger effect
    bilateralFilter(srcPixels, dstPixels, width, height);
    bilateralFilter(dstPixels, srcPixels, width, height);
    bilateralFilter(srcPixels, dstPixels, width, height);

    stopTimer = clock();
    double elapse = 1000.0 * (double) (stopTimer - startTimer) / (double) CLOCKS_PER_SEC;
    info[2] = (int) elapse;

    LOGI("C++ code on the CPU took %g ms\n\n",
         1000.0 * (double) (stopTimer - startTimer) / (double) CLOCKS_PER_SEC);

    //convert to uchar and write to bufOut
    index1 = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) {
                bufOut[index1] = (unsigned char) CLAMP(255.0f * dstPixels[index1], 0.0f, 255.0f);
                index1++;
            }
            bufOut[index1++] = 255; // set alpha to fully opaque
        }
    }

    delete srcPixels;
    delete dstPixels;

    return;
}
