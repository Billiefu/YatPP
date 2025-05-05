#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void generate_matrix(double* mat, int N) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    #pragma omp parallel for  // 并行生成随机数
    for (int i = 0; i < N * N; i++) {
        mat[i] = dis(gen);
    }
}

// 仅多线程并行（并行化i循环）
void matmul_parallel(double* A, double* B, double* C, int N) {
    #pragma omp parallel for  // 并行化外层i循环
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            double a = A[i * N + k];
            for (int j = 0; j < N; j++) {
                C[i * N + j] += a * B[k * N + j];
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
    double* C = new double[N * N];

    generate_matrix(A, N);
    generate_matrix(B, N);

    auto start = high_resolution_clock::now();
    matmul_parallel(A, B, C, N);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "Time (Parallel): " << duration.count() << " ms" << endl;

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
