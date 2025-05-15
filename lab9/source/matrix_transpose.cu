#include <iostream>
#include <cstdlib>
#include <cuda_runtime.h>

#define BLOCK_SIZE 16  // 可调，尝试 16、32 等不同大小

// CUDA 核函数：进行矩阵转置
__global__ void transpose(float* A, float* A_T, int n) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < n) {
        A_T[col * n + row] = A[row * n + col];
    }
}

// 初始化矩阵
void generateMatrix(float* mat, int n) {
    for (int i = 0; i < n * n; ++i) {
        mat[i] = static_cast<float>(rand() % 100);
    }
}

// 打印矩阵
void printMatrix(const float* mat, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            std::cout << mat[i * n + j] << "\t";
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./matrix_transpose <matrix_size>\n";
        return 1;
    }

    int n = atoi(argv[1]);
    size_t size = n * n * sizeof(float);

    // 分配主机内存
    float *h_A = (float*)malloc(size);
    float *h_A_T = (float*)malloc(size);

    generateMatrix(h_A, n);

    // 分配设备内存
    float *d_A, *d_A_T;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_A_T, size);

    // 复制数据到设备
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);

    // 设置 CUDA 定时器
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // 启动核函数
    dim3 blockSize(BLOCK_SIZE, BLOCK_SIZE);
    dim3 gridSize((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);

    cudaEventRecord(start);
    transpose<<<gridSize, blockSize>>>(d_A, d_A_T, n);
    cudaEventRecord(stop);

    // 复制结果回主机
    cudaMemcpy(h_A_T, d_A_T, size, cudaMemcpyDeviceToHost);

    // 计算耗时
    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    // 打印结果
    // std::cout << "Matrix A:\n";
    // printMatrix(h_A, n);

    // std::cout << "\nTranspose of A (A^T):\n";
    // printMatrix(h_A_T, n);

    std::cout << "\nTime taken for CUDA transpose: " << milliseconds << " ms\n";

    // 清理
    free(h_A);
    free(h_A_T);
    cudaFree(d_A);
    cudaFree(d_A_T);

    return 0;
}
