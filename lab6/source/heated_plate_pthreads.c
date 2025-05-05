#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "parallel_for.h"

#define M 500
#define N 500

static double now_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void set_boundary_left_right(int i, void* arg) {
    double (*w)[N] = (double (*)[N])arg;
    w[i][0] = 100.0;
    w[i][N-1] = 100.0;
}

void set_boundary_top_bottom(int j, void* arg) {
    double (*w)[N] = (double (*)[N])arg;
    w[M-1][j] = 100.0;
    w[0][j] = 0.0;
}

typedef struct {
    double (*w)[N];
    double* boundary_sum;
} BoundarySumArg;

void sum_boundary_left_right(int i, void* arg) {
    BoundarySumArg* bsa = (BoundarySumArg*)arg;
    *(bsa->boundary_sum) += bsa->w[i][0] + bsa->w[i][N-1];
}

void sum_boundary_top_bottom(int j, void* arg) {
    BoundarySumArg* bsa = (BoundarySumArg*)arg;
    *(bsa->boundary_sum) += bsa->w[M-1][j] + bsa->w[0][j];
}

typedef struct {
    double (*w)[N];
    double mean;
} InitInnerArg;

void initialize_inner(int i, void* arg) {
    InitInnerArg* iia = (InitInnerArg*)arg;
    for (int j = 1; j < N-1; j++) {
        iia->w[i][j] = iia->mean;
    }
}

typedef struct {
    double (*w)[N];
    double (*u)[N];
} CopyArg;

void copy_matrix(int i, void* arg) {
    CopyArg* ca = (CopyArg*)arg;
    for (int j = 0; j < N; j++) {
        ca->u[i][j] = ca->w[i][j];
    }
}

void compute_new_values(int i, void* arg) {
    CopyArg* ca = (CopyArg*)arg;
    for (int j = 1; j < N-1; j++) {
        ca->w[i][j] = 0.25 * (ca->u[i-1][j] + ca->u[i+1][j] + ca->u[i][j-1] + ca->u[i][j+1]);
    }
}

typedef struct {
    double (*w)[N];
    double (*u)[N];
    double* max_diff;
} DiffArg;

void compute_max_diff(int i, void* arg) {
    DiffArg* da = (DiffArg*)arg;
    double local_diff = 0.0;
    for (int j = 1; j < N-1; j++) {
        double temp = fabs(da->w[i][j] - da->u[i][j]);
        if (temp > local_diff) {
            local_diff = temp;
        }
    }
    if (local_diff > *(da->max_diff)) {
        *(da->max_diff) = local_diff;
    }
}

void heated_plate_pthreads(int num_threads, int schedule_type, double* out_time, int* out_iterations) {
    double epsilon = 0.001;
    double w[M][N];
    double u[M][N];
    double diff;
    double mean = 0.0;
    int iterations = 0;
    double wtime;

    parallel_for(1, M-2, 1, (void (*)(int, void*))set_boundary_left_right, (void*)w, num_threads, schedule_type);
    parallel_for(0, N-1, 1, (void (*)(int, void*))set_boundary_top_bottom, (void*)w, num_threads, schedule_type);

    double boundary_sum = 0.0;
    BoundarySumArg bsa = { w, &boundary_sum };
    parallel_for(1, M-2, 1, (void (*)(int, void*))sum_boundary_left_right, &bsa, num_threads, schedule_type);
    parallel_for(0, N-1, 1, (void (*)(int, void*))sum_boundary_top_bottom, &bsa, num_threads, schedule_type);

    mean = boundary_sum / (2 * M + 2 * N - 4);

    InitInnerArg iia = { w, mean };
    parallel_for(1, M-2, 1, (void (*)(int, void*))initialize_inner, &iia, num_threads, schedule_type);

    wtime = now_time();
    diff = epsilon;

    while (epsilon <= diff) {
        CopyArg ca = { w, u };
        parallel_for(0, M-1, 1, (void (*)(int, void*))copy_matrix, &ca, num_threads, schedule_type);
        parallel_for(1, M-2, 1, (void (*)(int, void*))compute_new_values, &ca, num_threads, schedule_type);

        double max_diff = 0.0;
        DiffArg da = { w, u, &max_diff };
        parallel_for(1, M-2, 1, (void (*)(int, void*))compute_max_diff, &da, num_threads, schedule_type);

        diff = max_diff;
        iterations++;
    }

    wtime = now_time() - wtime;

    *out_time = wtime;
    *out_iterations = iterations;
}

void print_result(int m, int n, int threads, int schedule_type, double time, int iterations) {
    const char* schedule_name = (schedule_type == 0) ? "static" : "dynamic";
    printf("Matrix: %dx%d | Threads: %d | Schedule: %s | Time: %.6f sec | Iterations: %d\n",
           m, n, threads, schedule_name, time, iterations);
}

int main() {
    int thread_list[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int matrix_sizes[][2] = {{500, 500}};
    int num_matrices = sizeof(matrix_sizes) / sizeof(matrix_sizes[0]);
    int num_threads = sizeof(thread_list) / sizeof(thread_list[0]);

    for (int m = 0; m < num_matrices; m++) {
        for (int t = 0; t < num_threads; t++) {
            int threads = thread_list[t];
            for (int sched = 0; sched <= 1; sched++) {
                double time = 0.0;
                int iterations = 0;
                heated_plate_pthreads(threads, sched, &time, &iterations);
                print_result(M, N, threads, sched, time, iterations);
            }
        }
    }

    return 0;
}
