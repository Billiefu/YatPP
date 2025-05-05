#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (double)rand() / RAND_MAX;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int scope[] = {128, 256, 384, 512, 640, 768, 896, 1024, 1152, 1280, 1408, 1536, 1664, 1792, 1920, 2048};
    const int NUM_RUNS = 5;
        
    for(int epoch = 0; epoch < 16; epoch++) {
        int M = scope[epoch];
        int N = scope[epoch];
        int P = scope[epoch];

        // 广播矩阵维度
        int dimensions[3] = {M, N, P};
        MPI_Bcast(dimensions, 3, MPI_INT, 0, MPI_COMM_WORLD);
        M = dimensions[0];
        N = dimensions[1];
        P = dimensions[2];

        // 预计算分发参数
        int remainder = M % size;
        int sum_rows = 0;
        int *sendcounts_A = (int*)malloc(size * sizeof(int));
        int *displs_A = (int*)malloc(size * sizeof(int));
        int *sendcounts_C = (int*)malloc(size * sizeof(int));
        int *displs_C = (int*)malloc(size * sizeof(int));

        for(int i = 0; i < size; i++) {
            int rows = (M / size) + (i < remainder ? 1 : 0);
            sendcounts_A[i] = rows * N;
            displs_A[i] = sum_rows * N;
            sendcounts_C[i] = rows * P;
            displs_C[i] = sum_rows * P;
            sum_rows += rows;
        }

        // 主进程记录时间
        double total_time = 0;
        if(rank == 0) {
            printf("Size: %4d | Runs: ", M);
        }

        // 运行5次求平均
        for(int run = 0; run < NUM_RUNS; run++) {
            double *A = NULL, *B = NULL, *C = NULL;
            double *B_buffer = (double*)malloc(N * P * sizeof(double));
            double start_time;

            // 主进程初始化数据
            if(rank == 0) {
                A = (double*)malloc(M * N * sizeof(double));
                B = (double*)malloc(N * P * sizeof(double));
                C = (double*)malloc(M * P * sizeof(double));
                initialize_matrix(A, M, N);
                initialize_matrix(B, N, P);
                memcpy(B_buffer, B, N * P * sizeof(double));
                start_time = MPI_Wtime();
            }

            // 广播矩阵B
            MPI_Bcast(B_buffer, N * P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            // 准备本地存储
            int local_rows = (M / size) + (rank < remainder ? 1 : 0);
            double *local_A = (double*)malloc(local_rows * N * sizeof(double));
            double *local_C = (double*)malloc(local_rows * P * sizeof(double));

            // 分发矩阵A
            MPI_Scatterv(A, sendcounts_A, displs_A, MPI_DOUBLE,
                        local_A, local_rows * N, MPI_DOUBLE,
                        0, MPI_COMM_WORLD);

            // 矩阵乘法计算
            for(int i = 0; i < local_rows; i++) {
                 for(int j = 0; j < P; j++) {
                     local_C[i*P+j] = 0.0;
                    for(int k = 0; k < N; k++) {
                        local_C[i*P+j] += local_A[i*N+k] * B_buffer[k*P+j];
                    }
                }
            }

            // 收集结果
            MPI_Gatherv(local_C, local_rows*P, MPI_DOUBLE,
                       C, sendcounts_C, displs_C, MPI_DOUBLE,
                       0, MPI_COMM_WORLD);

            // 计算耗时
            if(rank == 0) {
                double end_time = MPI_Wtime();
                double duration = (end_time - start_time) * 1000;
                total_time += duration;
                printf("%6.2fms ", duration);
                    
                free(A);
                free(B);
                free(C);
            }

            free(local_A);
            free(local_C);
            free(B_buffer);
        }

        // 输出平均时间
        if(rank == 0) {
            printf("| Avg: %6.2fms\n", total_time / NUM_RUNS);
        }

        free(sendcounts_A);
        free(displs_A);
        free(sendcounts_C);
        free(displs_C);
    }
        
    if(rank == 0) printf("\n");

    MPI_Finalize();
    return 0;
}
