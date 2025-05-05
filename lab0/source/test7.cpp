#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>
#include <immintrin.h>

using namespace std;
using namespace std::chrono;

void generate_matrix(double* mat, int N) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    #pragma omp parallel for
    for (int i = 0; i < N * N; i++) {
        mat[i] = dis(gen);
    }
}

// 分块优化 + 并行化（循环顺序 i-k-j）
void matmul_blocked_parallel(double* A, double* B, double* C, int N) {
    const int BLOCK_SIZE = 64;  // 分块大小（根据CPU缓存调整）
    #pragma omp parallel for collapse(2)  // 外层双循环并行
    for(int i = 0; i < N; i += BLOCK_SIZE) {
        for(int j = 0; j < N; j += BLOCK_SIZE) {
            for(int k = 0; k < N; k += BLOCK_SIZE) {
                for(int il = 0; il < BLOCK_SIZE; il ++) {
                    for(int kl = 0; kl < BLOCK_SIZE; kl ++) {
                        for(int jl = 0; jl < BLOCK_SIZE; jl ++) {
                            C[(i + il) * N + (j + jl)] += A[(i + il) * N + (k + kl)] * B[(k + kl) * N + (j + jl)];
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <N>" << endl;
        return 1;
    }
    int N = stoi(argv[1]);

    double* A = new double[N * N];
    double* B = new double[N * N];
    double* C = new double[N * N]();  // 初始化为0

    generate_matrix(A, N);
    generate_matrix(B, N);

    auto start = high_resolution_clock::now();
    matmul_blocked_parallel(A, B, C, N);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "Time (Blocked + Parallel): " << duration.count() << " ms" << endl;

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
