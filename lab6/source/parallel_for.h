#ifndef PARALLEL_FOR_H
#define PARALLEL_FOR_H

typedef enum {
    SCHEDULE_STATIC,
    SCHEDULE_DYNAMIC
} schedule_t;

void parallel_for(int start, int end, int inc,
                  void *(*functor)(int, void*), void *arg,
                  int num_threads, schedule_t schedule);

#endif
