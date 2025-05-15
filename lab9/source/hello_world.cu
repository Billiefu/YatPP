#include <iostream>
#include <cuda_runtime.h>

// CUDA kernel
__global__ void helloWorldKernel() {
    int blockId = blockIdx.x;
    int threadRow = threadIdx.y;
    int threadCol = threadIdx.x;
    
    printf("Hello World from Thread (%d, %d) in Block %d!\n", threadRow, threadCol, blockId);
}

int main() {
    int n, m, k;

    // 从标准输入读取参数
    std::cout << "Enter n (blocks), m (threads per block in y), k (threads per block in x): ";
    std::cin >> n >> m >> k;

    if (n < 1 || n > 32 || m < 1 || m > 32 || k < 1 || k > 32) {
        std::cerr << "Error: All values must be between 1 and 32." << std::endl;
        return 1;
    }

    std::cout << "Hello World from the host!" << std::endl;

    dim3 blockDim(k, m);  // x=k, y=m
    dim3 gridDim(n);      // 1D grid with n blocks

    helloWorldKernel<<<gridDim, blockDim>>>();
    cudaDeviceSynchronize();  // 等待GPU执行完毕

    return 0;
}
