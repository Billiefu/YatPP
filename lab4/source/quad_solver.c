#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define N 500000  // 批量处理方程数量
#define NUM_WORKERS 8  // 并行线程数

typedef struct {
    double a, b, c;
    double x1, x2;
    int index;
} Equation;

Equation equations[N];

// ================= 时间测量函数 =================
double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

// ========== 1. 串行求解单个方程 ==========
void solve_serial_single(double a, double b, double c, double* x1, double* x2) {
    double delta = b * b - 4 * a * c;
    if (delta < 0 || a == 0) {
        *x1 = *x2 = NAN;
    } else {
        double sqrt_delta = sqrt(delta);
        *x1 = (-b + sqrt_delta) / (2 * a);
        *x2 = (-b - sqrt_delta) / (2 * a);
    }
}

// ========== 2. 串行求解多个方程 ==========
void solve_serial_batch() {
    for (int i = 0; i < N; ++i) {
        solve_serial_single(equations[i].a, equations[i].b, equations[i].c, &equations[i].x1, &equations[i].x2);
    }
}

// ========== 3. 多线程求解单个方程 ==========
double a, b, c;
double delta, sqrt_delta, x1, x2;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_delta = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_sqrt = PTHREAD_COND_INITIALIZER;
int delta_ready = 0, sqrt_ready = 0;

void* compute_delta(void* _) {
    pthread_mutex_lock(&lock);
    delta = b * b - 4 * a * c;
    delta_ready = 1;
    pthread_cond_signal(&cond_delta);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* compute_sqrt_delta(void* _) {
    pthread_mutex_lock(&lock);
    while (!delta_ready)
        pthread_cond_wait(&cond_delta, &lock);
    sqrt_delta = (delta < 0) ? -1 : sqrt(delta);
    sqrt_ready = 1;
    pthread_cond_signal(&cond_sqrt);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* compute_roots(void* _) {
    pthread_mutex_lock(&lock);
    while (!sqrt_ready)
        pthread_cond_wait(&cond_sqrt, &lock);
    if (sqrt_delta < 0 || a == 0) {
        x1 = x2 = NAN;
    } else {
        x1 = (-b + sqrt_delta) / (2 * a);
        x2 = (-b - sqrt_delta) / (2 * a);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

void solve_parallel_single(double in_a, double in_b, double in_c, double* out_x1, double* out_x2) {
    a = in_a;
    b = in_b;
    c = in_c;
    delta_ready = sqrt_ready = 0;

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, compute_delta, NULL);
    pthread_create(&t2, NULL, compute_sqrt_delta, NULL);
    pthread_create(&t3, NULL, compute_roots, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    *out_x1 = x1;
    *out_x2 = x2;
}

// ========== 4. 多线程求解多个方程 ==========
void* batch_worker(void* arg) {
    int tid = *(int*)arg;
    for (int i = tid; i < N; i += NUM_WORKERS) {
        Equation* eq = &equations[i];
        double d = eq->b * eq->b - 4 * eq->a * eq->c;
        if (d < 0 || eq->a == 0) {
            eq->x1 = eq->x2 = NAN;
        } else {
            double s = sqrt(d);
            eq->x1 = (-eq->b + s) / (2 * eq->a);
            eq->x2 = (-eq->b - s) / (2 * eq->a);
        }
    }
    return NULL;
}

void solve_parallel_batch() {
    pthread_t threads[NUM_WORKERS];
    int ids[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; ++i) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, batch_worker, &ids[i]);
    }
    for (int i = 0; i < NUM_WORKERS; ++i) {
        pthread_join(threads[i], NULL);
    }
}

// ================= 主程序入口 =================
int main() {
    
    srand(42);
    for (int i = 0; i < N; ++i) {
        equations[i].a = (rand() % 200 - 100);
        equations[i].b = (rand() % 200 - 100);
        equations[i].c = (rand() % 200 - 100);
    }

    printf("========== 单个方程 ==========\n");
    double t1 = get_time_ms();
    solve_serial_single(equations[0].a, equations[0].b, equations[0].c, &x1, &x2);
    double t2 = get_time_ms();
    printf("串行: x1=%.5f x2=%.5f, time=%.2f ns\n", x1, x2, (t2 - t1) * 1000);

    double t3 = get_time_ms();
    solve_parallel_single(equations[0].a, equations[0].b, equations[0].c, &x1, &x2);
    double t4 = get_time_ms();
    printf("并行: x1=%.5f x2=%.5f, time=%.2f ns\n", x1, x2, (t4 - t3) * 1000);

    printf("\n========== 多个方程 ==========\n");
    double t5 = get_time_ms();
    solve_serial_batch();
    double t6 = get_time_ms();
    printf("串行批量处理 %d 个方程: %.2f ns\n", N, (t6 - t5) * 1000);

    double t7 = get_time_ms();
    solve_parallel_batch();
    double t8 = get_time_ms();
    printf("并行批量处理 %d 个方程: %.2f ns\n", N, (t8 - t7) * 1000);

    return 0;
}
