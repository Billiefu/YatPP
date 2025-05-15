#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <mpi.h>

using namespace std;

using Complex = complex<double>;
const double PI = acos(-1);

// 计算单位根 Wn^k
Complex W(int N, int k) {
    return Complex(cos(2 * PI * k / N), -sin(2 * PI * k / N));
}

// 原地 Cooley-Tukey FFT（用于小块局部数据）
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

// 并行 FFT 主程序
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 1024; // FFT 输入大小（必须是 2 的幂）
    vector<Complex> global_data;

    // 进程 0 生成数据
    if (rank == 0) {
        global_data.resize(N);
        for (int i = 0; i < N; ++i) {
            global_data[i] = Complex(i, 0); // 例如实部为 i，虚部为 0
        }
    }

    // 每个进程接收 N/size 个元素
    int local_N = N / size;
    vector<Complex> local_data(local_N);

    // 数据分发
    MPI_Scatter(global_data.data(), local_N, MPI_DOUBLE_COMPLEX,
                local_data.data(), local_N, MPI_DOUBLE_COMPLEX,
                0, MPI_COMM_WORLD);

    // 每个进程对局部数据做 FFT
    fft_serial(local_data, local_N, 0);

    // 全局蝶形合并（log(size) 步）
    for (int step = 1; step < size; step <<= 1) {
        int partner = rank ^ step;

        vector<Complex> recv_data(local_N);
        MPI_Sendrecv(local_data.data(), local_N, MPI_DOUBLE_COMPLEX, partner, 0,
                     recv_data.data(), local_N, MPI_DOUBLE_COMPLEX, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // 合并：对 local_data 和 recv_data 执行蝶形操作
        for (int i = 0; i < local_N; ++i) {
            int global_i = i + rank * local_N;
            Complex w = W(N, (global_i % (step * local_N)));
            Complex u = local_data[i];
            Complex t = w * recv_data[i];
            local_data[i] = u + t;
        }
    }

    // 收集结果到进程 0
    MPI_Gather(local_data.data(), local_N, MPI_DOUBLE_COMPLEX,
               global_data.data(), local_N, MPI_DOUBLE_COMPLEX,
               0, MPI_COMM_WORLD);

    // 打印结果（仅限进程 0）
    if (rank == 0) {
        for (int i = 0; i < N; ++i) {
            cout << "FFT[" << i << "] = " << global_data[i] << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
