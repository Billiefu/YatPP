#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    #define M 500
    #define N 500

    double diff;
    double epsilon = 0.001;
    int i, j;
    int iterations;
    double mean;
    double my_diff;
    double u[M][N];
    double w[M][N];
    double wtime_start, wtime_end, elapsed_time;

    char* schedule_types[2] = {"static", "dynamic"}; 

    for (int num_threads = 1; num_threads <= 16; num_threads++) {
        for (int type = 0; type < 2; type++) {
            char* schedule_type = schedule_types[type];

            omp_set_num_threads(num_threads);

            if (strcmp(schedule_type, "static") == 0) {
                omp_set_schedule(omp_sched_static, 0);
            } else if (strcmp(schedule_type, "dynamic") == 0) {
                omp_set_schedule(omp_sched_dynamic, 0);
            }

            mean = 0.0;

            #pragma omp parallel shared(w) private(i, j)
            {
                #pragma omp for schedule(runtime)
                for (i = 1; i < M - 1; i++) {
                    w[i][0] = 100.0;
                }
                #pragma omp for schedule(runtime)
                for (i = 1; i < M - 1; i++) {
                    w[i][N-1] = 100.0;
                }
                #pragma omp for schedule(runtime)
                for (j = 0; j < N; j++) {
                    w[M-1][j] = 100.0;
                }
                #pragma omp for schedule(runtime)
                for (j = 0; j < N; j++) {
                    w[0][j] = 0.0;
                }

                #pragma omp for reduction(+:mean)
                for (i = 1; i < M - 1; i++) {
                    mean = mean + w[i][0] + w[i][N-1];
                }
                #pragma omp for reduction(+:mean)
                for (j = 0; j < N; j++) {
                    mean = mean + w[M-1][j] + w[0][j];
                }
            }

            mean = mean / (double)(2 * M + 2 * N - 4);

            #pragma omp parallel shared(mean, w) private(i, j)
            {
                #pragma omp for schedule(runtime)
                for (i = 1; i < M - 1; i++) {
                    for (j = 1; j < N - 1; j++) {
                        w[i][j] = mean;
                    }
                }
            }

            iterations = 0;
            diff = epsilon;

            wtime_start = omp_get_wtime();

            while (epsilon <= diff) {
                #pragma omp parallel shared(u, w) private(i, j)
                {
                    #pragma omp for schedule(runtime)
                    for (i = 0; i < M; i++) {
                        for (j = 0; j < N; j++) {
                            u[i][j] = w[i][j];
                        }
                    }

                    #pragma omp for schedule(runtime)
                    for (i = 1; i < M - 1; i++) {
                        for (j = 1; j < N - 1; j++) {
                            w[i][j] = (u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1]) / 4.0;
                        }
                    }
                }

                diff = 0.0;
                #pragma omp parallel shared(diff, u, w) private(i, j, my_diff)
                {
                    my_diff = 0.0;
                    #pragma omp for schedule(runtime)
                    for (i = 1; i < M - 1; i++) {
                        for (j = 1; j < N - 1; j++) {
                            if (my_diff < fabs(w[i][j] - u[i][j])) {
                                my_diff = fabs(w[i][j] - u[i][j]);
                            }
                        }
                    }
                    #pragma omp critical
                    {
                        if (diff < my_diff) {
                            diff = my_diff;
                        }
                    }
                }

                iterations++;
            }

            wtime_end = omp_get_wtime();
            elapsed_time = wtime_end - wtime_start;

            printf("Matrix: %dx%d | Threads: %d | Schedule: %s | Time: %.6f sec | Iterations: %d\n",
                   M, N, num_threads, schedule_type, elapsed_time, iterations);
        }
    }

    return 0;

    #undef M
    #undef N
}
