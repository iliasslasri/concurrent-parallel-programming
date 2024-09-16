#include <iostream>
#include <cuda_runtime.h>

__global__ void kernel() {
    printf("Hello, CUDA!\n");
}

void checkCudaError(cudaError_t result, const char* msg) {
    if (result != cudaSuccess) {
        std::cerr << "CUDA Error: " << msg << " (" << cudaGetErrorString(result) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    kernel<<<1, 1>>>();
    checkCudaError(cudaGetLastError(), "Kernel launch");
    checkCudaError(cudaDeviceSynchronize(), "Device synchronization");
    return 0;
}

/**
    * Compile with:
    *  nvcc -o first first.cu
    * Run with:
    * ./first
    * Expected output:
    * Hello, CUDA!
 */
