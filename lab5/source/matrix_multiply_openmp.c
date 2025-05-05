#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void generate_matrix(double* mat, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        mat[i] = (double)(rand() % 1000) / 100.0;
    }
}

void print_matrix(double* mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f ", mat[i * cols + j]);
        }
        printf("\n");
    }
}

void matrix_multiply(double* A, double* B, double* C, int m, int n, int k, int num_threads, int schedule_mode) {
    omp_set_num_threads(num_threads);

    #pragma omp parallel for collapse(2) schedule(runtime)
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            double sum = 0.0;
            for (int l = 0; l < n; l++) {
                sum += A[i * n + l] * B[l * k + j];
            }
            C[i * k + j] = sum;
        }
    }
}

int main() {
    int m, n, k;
    int size[16] = {128, 256, 384, 512, 640, 768, 896, 1024, 1152, 1280, 1408, 1536, 1664, 1792, 1920, 2048};

    for(int epoch = 0; epoch < 16; epoch ++) {
        m = n = k = size[epoch];
        printf("\r\n =============== Epoch %d ===============\r\nsize = %d\r\n", epoch + 1, size[epoch]);

        double* A = (double*)malloc(m * n * sizeof(double));
        double* B = (double*)malloc(n * k * sizeof(double));
        double* C = (double*)malloc(m * k * sizeof(double));

        generate_matrix(A, m, n);
        generate_matrix(B, n, k);

        // printf("Matrix A:\n");
        // print_matrix(A, m, n);
        // printf("\r\nMatrix B:\n");
        // print_matrix(B, n, k);

        int thread_counts[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        const char* schedule_names[] = {"default", "static", "dynamic"};

        for (int s = 0; s < 3; s++) {
            printf("\nSchedule Mode: %s\n", schedule_names[s]);
            for (int t = 0; t < 16; t++) {
                int num_threads = thread_counts[t];

                if (s == 1) omp_set_schedule(omp_sched_static, 0);
                else if (s == 2) omp_set_schedule(omp_sched_dynamic, 0);
                else /* default: do not set */;

                double start_time = omp_get_wtime();
                matrix_multiply(A, B, C, m, n, k, num_threads, s);
                double end_time = omp_get_wtime();

                printf("Threads: %d, Time: %.2f ms\n", num_threads, (end_time - start_time) * 1000);
            }
        }

        // printf("\r\nMatrix C:\n");
        // print_matrix(C, m, k);

        free(A);
        free(B);
        free(C);

    }
    
    return 0;
}
