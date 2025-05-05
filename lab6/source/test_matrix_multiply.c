#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "parallel_for.h"

#define EPSILON 1e-3

double get_time_in_ms() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

void random_fill(float *mat, int rows, int cols) {
    for (int i = 0; i < rows * cols; ++i) {
        mat[i] = (float)(rand() % 1000) / 100.0f;
    }
}

void serial_matmul(const float *A, const float *B, float *C, int m, int n, int k) {
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            float sum = 0.0f;
            for (int p = 0; p < n; ++p) {
                sum += A[i*n + p] * B[p*k + j];
            }
            C[i*k + j] = sum;
        }
    }
}

int compare_matrices(const float *C1, const float *C2, int size) {
    for (int i = 0; i < size; ++i) {
        if (fabs(C1[i] - C2[i]) > EPSILON) {
            return 0;
        }
    }
    return 1;
}

typedef struct {
    const float *A;
    const float *B;
    float *C;
    int m, n, k;
} MatMulArgs;

void *matmul_row(int row, void *args) {
    MatMulArgs *mat_args = (MatMulArgs*)args;
    const float *A = mat_args->A;
    const float *B = mat_args->B;
    float *C = mat_args->C;
    int n = mat_args->n, k = mat_args->k;

    for (int j = 0; j < k; ++j) {
        float sum = 0.0f;
        for (int p = 0; p < n; ++p) {
            sum += A[row*n + p] * B[p*k + j];
        }
        C[row*k + j] = sum;
    }

    return NULL;
}

int main() {
    int sizes[] = {128, 256, 384, 512, 640, 768, 896, 1024, 1152, 1280, 1408, 1536, 1664, 1792, 1920, 2048};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    int thread_counts[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int num_threads = sizeof(thread_counts) / sizeof(thread_counts[0]);

    const char *policy_name[] = {"STATIC", "DYNAMIC"};
    int policies[] = {SCHEDULE_STATIC, SCHEDULE_DYNAMIC};

    printf("MatrixSize\tThreads\tPolicy\t\tTime(ms)\tCorrect\n");

    for (int p = 0; p < 2; ++p) {
        int policy = policies[p];

        for (int s = 0; s < num_sizes; ++s) {
            int m = sizes[s], n = sizes[s], k = sizes[s];

            float *A = (float*)malloc(sizeof(float) * m * n);
            float *B = (float*)malloc(sizeof(float) * n * k);
            float *C = (float*)malloc(sizeof(float) * m * k);
            float *C_ref = (float*)malloc(sizeof(float) * m * k);

            random_fill(A, m, n);
            random_fill(B, n, k);
            serial_matmul(A, B, C_ref, m, n, k);

            for (int t = 0; t < num_threads; ++t) {
                int thread_num = thread_counts[t];
                memset(C, 0, sizeof(float) * m * k);

                MatMulArgs args = {A, B, C, m, n, k};

                double start = get_time_in_ms();
                parallel_for(0, m, 1, matmul_row, &args, thread_num, policy);
                double end = get_time_in_ms();

                int correct = compare_matrices(C_ref, C, m * k);

                printf("%d\t\t%d\t%s\t\t%.2f\t\t%s\n", m, thread_num,
                       policy_name[p], end - start, correct ? "Yes" : "No");
            }

            free(A);
            free(B);
            free(C);
            free(C_ref);
        }
    }

    return 0;
}
