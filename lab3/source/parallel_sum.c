#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_THREADS 16
#define MAX_SIZE (128 * 1000 * 1000)  // 128M

int* A;                   // 整型数组 A
long long global_sum = 0; // 全局求和结果
int n;                    // 数组长度
int thread_count;         // 线程数量

pthread_t threads[MAX_THREADS];
pthread_mutex_t mutex;

// 聚合方式
typedef enum {
    LOCK_AGGREGATION,  // 使用锁聚合
    ATOMIC_AGGREGATION, // 使用原子操作聚合
    LOCAL_AGGREGATION   // 使用局部汇总合并
} AggregationType;

AggregationType aggregation_type = LOCK_AGGREGATION; // 默认使用锁聚合

typedef struct {
    int tid;
    int start;
    int end;
    long long local_sum;
} ThreadArg;

void* partial_sum(void* arg) {
    ThreadArg* t_arg = (ThreadArg*)arg;
    long long sum = 0;
    for (int i = t_arg->start; i < t_arg->end; ++i) {
        sum += A[i];
    }

    // 聚合方式：根据类型选择合并方式
    if (aggregation_type == LOCK_AGGREGATION) {
        // 使用锁聚合
        pthread_mutex_lock(&mutex);
        global_sum += sum;
        pthread_mutex_unlock(&mutex);
    } else if (aggregation_type == ATOMIC_AGGREGATION) {
        // 使用原子操作聚合
        __sync_fetch_and_add(&global_sum, sum);
    } else if (aggregation_type == LOCAL_AGGREGATION) {
        // 使用局部汇总合并，保存局部和
        t_arg->local_sum = sum;
    }

    return NULL;
}

void generate_array(int n) {
    A = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; ++i)
        A[i] = rand() % 100;
}

void print_array(int n) {
    for (int i = 0; i < n && i < 20; ++i)
        printf("%d ", A[i]);
    if (n > 20)
        printf("... ");
    printf("\n");
}

long long final_sum(ThreadArg* args) {
    long long total_sum = 0;
    if (aggregation_type == LOCAL_AGGREGATION) {
        // 局部和汇总
        for (int i = 0; i < thread_count; ++i) {
            total_sum += args[i].local_sum;
        }
    }
    return total_sum;
}

int main(int argc, char* argv[]) {

    // if (argc < 4) {
    //     printf("用法: %s 数组大小(单位:百万) 线程数 聚合方式(0:锁聚合, 1:原子聚合, 2:局部汇总)\n", argv[0]);
    //     return 1;
    // }

    int ns[] = {1, 2, 4, 8, 16, 32, 64, 128};

    for(int type = 0; type < 3; type ++) {
        for(int count = 1; count <= 16; count ++) {
            for(int epoch = 0; epoch < 8; epoch ++) {

                n = ns[epoch] * 1000 * 1000;
                thread_count = count;
                aggregation_type = type;

                // if (n > MAX_SIZE || thread_count > MAX_THREADS || thread_count <= 0 || aggregation_type < 0 || aggregation_type > 2) {
                //     printf("错误：数组大小最大128M，线程数1-16，聚合方式（0: 锁聚合，1: 原子聚合，2: 局部汇总）\n");
                //     return 1;
                // }

                srand(123);
                generate_array(n);
                pthread_mutex_init(&mutex, NULL);
            
                struct timeval start, end;
                gettimeofday(&start, NULL);
            
                ThreadArg args[MAX_THREADS];
                int chunk = n / thread_count;
            
                for (int i = 0; i < thread_count; ++i) {
                    args[i].tid = i;
                    args[i].start = i * chunk;
                    args[i].end = (i == thread_count - 1) ? n : (i + 1) * chunk;
                    pthread_create(&threads[i], NULL, partial_sum, &args[i]);
                }
            
                for (int i = 0; i < thread_count; ++i) {
                    pthread_join(threads[i], NULL);
                }
            
                // 如果使用局部汇总合并，需要在主线程汇总
                if (aggregation_type == LOCAL_AGGREGATION) {
                    global_sum = final_sum(args);
                }
            
                gettimeofday(&end, NULL);
                double time_taken = (end.tv_sec - start.tv_sec) * 1e3 +
                                    (end.tv_usec - start.tv_usec) / 1e3;
            
                printf("数组前20项：");
                print_array(n);
                printf("数组总大小: %dM 项\t", n / 1000 / 1000);
                printf("线程数: %d\t", thread_count);
                printf("聚合方式: ");
                if (aggregation_type == LOCK_AGGREGATION) {
                    printf("锁聚合\t");
                } else if (aggregation_type == ATOMIC_AGGREGATION) {
                    printf("原子操作聚合\t");
                } else {
                    printf("局部汇总合并\t");
                }
                printf("元素和: %lld\t", global_sum);
                printf("求和耗时: %.3f ms\n", time_taken);
            
                pthread_mutex_destroy(&mutex);
                free(A);
            }
        }
        printf("\n");
    }
    return 0;
}
