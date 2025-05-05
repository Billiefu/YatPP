#include <iostream>
#include <chrono>
#include <random>

extern "C" {
    #include <mkl.h>
}

void generate_matrix(double* mat, int N) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    #pragma omp parallel for
    for (int i = 0; i < N * N; i++) {
        mat[i] = dis(gen);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
        return 1;
    }
    int N = std::stoi(argv[1]);

    double* A = new double[N * N];
    double* B = new double[N * N];
    double* C = new double[N * N]();

    generate_matrix(A, N);
    generate_matrix(B, N);

    auto start = std::chrono::high_resolution_clock::now();
    cblas_dgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        N, N, N, 1.0, A, N, B, N, 0.0, C, N
    );
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Time (MKL): " << duration.count() << " ms" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
