#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_SIZE 2048

int **A, **B, **C;
int N; // 矩阵规模
int thread_count;

typedef struct {
    int thread_id;
} ThreadArgs;

void* multiply_worker(void *args) {
    int id = ((ThreadArgs *)args)->thread_id;
    int rows_per_thread = N / thread_count;
    int start = id * rows_per_thread;
    int end = (id == thread_count - 1) ? N : start + rows_per_thread;

    for (int i = start; i < end; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return NULL;
}

void* block_multiply(void* arg) {
    int id = *(int*)arg;

    // 取整时确保 sqrt 后不为 0 且能整除
    int grid_size = (int)sqrt(thread_count);
    if (grid_size * grid_size != thread_count || grid_size == 0) {
        // fprintf(stderr, "线程数 %d 不能构成正方形线程网格（如 1, 4, 9, 16...），当前线程 id = %d\n", thread_count, id);
        pthread_exit(NULL);
    }

    int block_size = N / grid_size;

    int row_block = id / grid_size;
    int col_block = id % grid_size;

    int row_start = row_block * block_size;
    int col_start = col_block * block_size;

    int row_end = (row_block == grid_size - 1) ? N : row_start + block_size;
    int col_end = (col_block == grid_size - 1) ? N : col_start + block_size;

    for (int i = row_start; i < row_end; ++i) {
        for (int j = col_start; j < col_end; ++j) {
            int sum = 0;
            for (int k = 0; k < N; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    return NULL;
}

void* sparse_multiply(void *args) {
    int id = ((ThreadArgs *)args)->thread_id;
    int rows_per_thread = N / thread_count;
    int start = id * rows_per_thread;
    int end = (id == thread_count - 1) ? N : start + rows_per_thread;

    for (int i = start; i < end; ++i) {
        int* rowA = A[i];
        int* rowC = C[i];
        for (int j = 0; j < N; ++j) {
            rowC[j] = 0;
        }
        for (int k = 0; k < N; ++k) {
            int a_ik = rowA[k];
            if (a_ik != 0) {
                int* rowB = B[k];
                for (int j = 0; j < N; ++j) {
                    rowC[j] += a_ik * rowB[j];
                }
            }
        }
    }
    return NULL;
}

int **allocate_matrix(int size) {
    int **mat = malloc(size * sizeof(int *));
    for (int i = 0; i < size; ++i)
        mat[i] = malloc(size * sizeof(int));
    return mat;
}

void free_matrix(int **mat, int size) {
    for (int i = 0; i < size; ++i)
        free(mat[i]);
    free(mat);
}

void fill_matrix(int **mat, int size) {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            mat[i][j] = rand() % 10;
            // mat[i][j] = (rand() % 100 < 10) ? rand() % 10 : 0;
}

int main(int argc, char *argv[]) {

    // if (argc != 3) {
    //     printf("用法: %s <矩阵大小N> <线程数>\n", argv[0]);
    //     return -1;
    // }

    int Ns[] = {128, 256, 384, 512, 640, 768, 896, 1024, 1152, 1280, 1408, 1536, 1664, 1792, 1920, 2048};

    for(int count = 1; count <= 16; count ++) {
        for(int epoch = 0; epoch < 16; epoch ++) {

            N = Ns[epoch];
            thread_count = count;

            srand(time(NULL));
            A = allocate_matrix(N);
            B = allocate_matrix(N);
            C = allocate_matrix(N);
            fill_matrix(A, N);
            fill_matrix(B, N);

            pthread_t threads[thread_count];
            ThreadArgs args[thread_count];

            struct timeval start, end;
            gettimeofday(&start, NULL);

            for (int i = 0; i < thread_count; ++i) {
                args[i].thread_id = i;
                pthread_create(&threads[i], NULL, multiply_worker, &args[i]);
                // pthread_create(&threads[i], NULL, block_multiply, &args[i]);
                // pthread_create(&threads[i], NULL, sparse_multiply, &args[i]);
            }

            for (int i = 0; i < thread_count; ++i) {
                pthread_join(threads[i], NULL);
            }

            gettimeofday(&end, NULL);
            double time_taken = (end.tv_sec - start.tv_sec) * 1e3 +
                                (end.tv_usec - start.tv_usec) / 1e3;

            printf("矩阵大小: %d, 线程数: %d, 用时: %.2f ms\n", N, thread_count, time_taken);

            /*
            printf("矩阵 C:\n");
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j)
                    printf("%d ", C[i][j]);
                printf("\n");
            }
            */

            free_matrix(A, N);
            free_matrix(B, N);
            free_matrix(C, N);
        }
    }
    return 0;
}
