#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <chrono>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

using Complex = complex<double>;
const double PI = acos(-1.0);

// ==============================
// 工具函数
// ==============================
Complex W(int N, int k) {
    return Complex(cos(2 * PI * k / N), -sin(2 * PI * k / N));
}

void fft_serial(vector<Complex>& a, int n, int invert) {
    if (n == 1) return;
    vector<Complex> a0(n / 2), a1(n / 2);
    for (int i = 0; i < n / 2; ++i) {
        a0[i] = a[i * 2];
        a1[i] = a[i * 2 + 1];
    }
    fft_serial(a0, n / 2, invert);
    fft_serial(a1, n / 2, invert);

    for (int k = 0; k < n / 2; ++k) {
        Complex w = W(n, (invert ? -k : k));
        a[k] = a0[k] + w * a1[k];
        a[k + n / 2] = a0[k] - w * a1[k];
    }
}

// ==============================
// 串行 FFT 性能测试
// ==============================
double run_serial_fft(int N) {
    vector<Complex> data(N);
    for (int i = 0; i < N; ++i)
        data[i] = Complex(i, 0);

    auto start = high_resolution_clock::now();
    fft_serial(data, N, 0);
    auto end = high_resolution_clock::now();

    duration<double> elapsed = end - start;
    return elapsed.count();
}

// ==============================
// 并行 FFT 性能测试（MPI）
// ==============================
double run_mpi_fft(int N, int rank, int size) {
    vector<Complex> global_data;
    int local_N = N / size;
    vector<Complex> local_data(local_N);

    if (rank == 0) {
        global_data.resize(N);
        for (int i = 0; i < N; ++i)
            global_data[i] = Complex(i, 0);
    }

    MPI_Scatter(global_data.data(), local_N, MPI_DOUBLE_COMPLEX,
                local_data.data(), local_N, MPI_DOUBLE_COMPLEX,
                0, MPI_COMM_WORLD);

    double start = MPI_Wtime();
    fft_serial(local_data, local_N, 0);  // 本地 FFT

    for (int step = 1; step < size; step <<= 1) {
        int partner = rank ^ step;
        vector<Complex> recv_data(local_N);
        MPI_Sendrecv(local_data.data(), local_N, MPI_DOUBLE_COMPLEX, partner, 0,
                     recv_data.data(), local_N, MPI_DOUBLE_COMPLEX, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < local_N; ++i) {
            int global_i = rank * local_N + i;
            Complex w = W(N, global_i % (step * local_N));
            Complex u = local_data[i];
            Complex t = w * recv_data[i];
            local_data[i] = u + t;
        }
    }

    MPI_Gather(local_data.data(), local_N, MPI_DOUBLE_COMPLEX,
               global_data.data(), local_N, MPI_DOUBLE_COMPLEX,
               0, MPI_COMM_WORLD);
    double end = MPI_Wtime();
    return end - start;
}

// ==============================
// 主程序：MPI vs Serial 对比
// ==============================
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 1 << 20; // 1048576 个点，支持命令行修改
    if (argc >= 2) {
        N = atoi(argv[1]);
        if ((N & (N - 1)) != 0) {
            if (rank == 0) cerr << "N must be a power of 2.\n";
            MPI_Finalize();
            return 1;
        }
    }

    double serial_time = 0;
    if (rank == 0) {
        cout << "===== FFT Performance Comparison =====\n";
        cout << "Input size N = " << N << ", Processes = " << size << endl;
        serial_time = run_serial_fft(N);
        cout << "[Serial FFT] Time: " << serial_time << " s" << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD); // 等待串行完成
    double mpi_time = run_mpi_fft(N, rank, size);

    if (rank == 0) {
        cout << "[MPI FFT]    Time: " << mpi_time << " s" << endl;
        cout << "[Speedup]    Serial / MPI = " << serial_time / mpi_time << endl;
    }

    MPI_Finalize();
    return 0;
}
