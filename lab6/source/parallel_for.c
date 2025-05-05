#include <pthread.h>
#include <stdlib.h>
#include "parallel_for.h"

typedef struct {
    int thread_id;
    int start;
    int end;
    int inc;
    void *(*functor)(int, void*);
    void *arg;
    schedule_t schedule;
    int *shared_idx;
    pthread_mutex_t *lock;
} ThreadArgs;

void *thread_func_static(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    for (int i = args->start; i < args->end; i += args->inc) {
        args->functor(i, args->arg);
    }
    return NULL;
}

void *thread_func_dynamic(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    while (1) {
        int idx;
        pthread_mutex_lock(args->lock);
        idx = *(args->shared_idx);
        *(args->shared_idx) += args->inc;
        pthread_mutex_unlock(args->lock);

        if (idx >= args->end) break;
        args->functor(idx, args->arg);
    }
    return NULL;
}

void parallel_for(int start, int end, int inc, 
                  void *(*functor)(int, void*), void *arg, 
                  int num_threads, schedule_t schedule) {

    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    if (schedule == SCHEDULE_STATIC) {
        int total_iters = (end - start + inc - 1) / inc;
        int iters_per_thread = total_iters / num_threads;
        int remainder = total_iters % num_threads;

        int current_start = start;
        for (int i = 0; i < num_threads; ++i) {
            int my_iters = iters_per_thread + (i < remainder ? 1 : 0);
            int my_start = current_start;
            int my_end = my_start + my_iters * inc;

            thread_args[i].thread_id = i;
            thread_args[i].start = my_start;
            thread_args[i].end = my_end;
            thread_args[i].inc = inc;
            thread_args[i].functor = functor;
            thread_args[i].arg = arg;
            thread_args[i].schedule = schedule;

            pthread_create(&threads[i], NULL, thread_func_static, &thread_args[i]);
            current_start = my_end;
        }
    } 
    else if (schedule == SCHEDULE_DYNAMIC) {
        int shared_idx = start;
        pthread_mutex_t lock;
        pthread_mutex_init(&lock, NULL);

        for (int i = 0; i < num_threads; ++i) {
            thread_args[i].thread_id = i;
            thread_args[i].start = start;
            thread_args[i].end = end;
            thread_args[i].inc = inc;
            thread_args[i].functor = functor;
            thread_args[i].arg = arg;
            thread_args[i].schedule = schedule;
            thread_args[i].shared_idx = &shared_idx;
            thread_args[i].lock = &lock;

            pthread_create(&threads[i], NULL, thread_func_dynamic, &thread_args[i]);
        }

        for (int i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], NULL);
        }
        pthread_mutex_destroy(&lock);
        return;
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}

