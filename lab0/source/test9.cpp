#include <iostream>
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

// 手工AVX优化
void matmul_avx(double* A, double* B, double* C, int N) {
    const int BLOCK_SIZE = 64;
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i += BLOCK_SIZE) {
        for (int j = 0; j < N; j += BLOCK_SIZE) {
            for (int k = 0; k < N; k += BLOCK_SIZE) {
                for (int ii = i; ii < min(i + BLOCK_SIZE, N); ii++) {
                    #pragma omp simd
                    for (int kk = k; kk < min(k + BLOCK_SIZE, N); kk++) {
                        const double a = A[ii * N + kk];
                        for (int jj = j; jj < min(j + BLOCK_SIZE, N); jj += 4) {
                            __m256d b = _mm256_loadu_pd(&B[kk * N + jj]);
                            __m256d c = _mm256_loadu_pd(&C[ii * N + jj]);
                            __m256d av = _mm256_set1_pd(a);
                            c = _mm256_add_pd(c, _mm256_mul_pd(av, b));
                            _mm256_storeu_pd(&C[ii * N + jj], c);
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
    matmul_avx(A, B, C, N);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "Time (AVX): " << duration.count() << " ms" << endl;

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
