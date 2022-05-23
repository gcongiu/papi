/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/**
 * @file    hello_cupti.cu
 * @author  Heike Jagode
 *          jagode@eecs.utk.edu
 * Mods:    Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 * test case for Example component
 *
 *
 * @brief
 *  This file is a very simple HelloWorld C example which serves (together
 *  with its Makefile) as a guideline on how to add tests to components.
 *  The papi configure and papi Makefile will take care of the compilation
 *  of the component tests (if all tests are added to a directory named
 *  'tests' in the specific component dir).
 *  See components/README for more details.
 *
 *  The string "Hello Cupti!" is mangled and then restored.
 */

#include <stdio.h>
#include "papi.h"

#define NUM_EVENTS 1

// Prototypes
__global__ void helloCupti(char*);

// Host function
int main(int argc, char** argv)
{
    int retval, i;
    int quiet;

    char str[] = "Hello Cupti!";

    int j;
    for (j = 0; j < 12; j++) {
        /* mangle contents of output */
        str[j] -= j;
    }

    cudaSetDevice(0);

    char *d_str;
    size_t size = sizeof(str);
    cudaMalloc((void**)&d_str, size);
    cudaMemcpy(d_str, str, size, cudaMemcpyHostToDevice);

    dim3 dimGrid(2);  // one block per word
    dim3 dimBlock(6); // one thread per character

    helloCupti<<<dimGrid, dimBlock>>>(d_str);

    cudaDeviceSynchronize();
    cudaMemcpy(str, d_str, size, cudaMemcpyDeviceToHost);
    cudaFree(d_str);

    return 0;
}

__global__ void
helloCupti(char* str)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    str[idx] += idx;
}
