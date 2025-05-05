#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define MAX_POINTS 65536
#define MIN_POINTS 1024
#define STEP 1024
#define MAX_THREADS 8

typedef struct {
    long points_per_thread;
    long points_in_circle;
    unsigned int seed;
} ThreadData;

void* monte_carlo_worker(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long in_circle = 0;
    for (long i = 0; i < data->points_per_thread; ++i) {
        double x = rand_r(&data->seed) / (double)RAND_MAX;
        double y = rand_r(&data->seed) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0)
            in_circle++;
    }
    data->points_in_circle = in_circle;
    return NULL;
}

double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

void save_point_plot(long total_points, long total_in_circle) {
    FILE* file = fopen("points_plot_data.txt", "a");  // Append to avoid overwriting
    if (!file) {
        perror("fopen");
        return;
    }

    // Save points for the circle and the outside of the circle
    long points_outside = total_points - total_in_circle;
    for (long i = 0; i < total_points; ++i) {
        double x = rand() / (double)RAND_MAX;
        double y = rand() / (double)RAND_MAX;
        if (x * x + y * y <= 1.0) {
            fprintf(file, "%lf %lf 1\n", x, y);  // Point inside the circle
        } else {
            fprintf(file, "%lf %lf 0\n", x, y);  // Point outside the circle
        }
    }
    fclose(file);
}

int main() {
    FILE* file = fopen("pi_estimates.csv", "w");
    if (!file) {
        perror("fopen");
        return 1;
    }
    fprintf(file, "total_points,points_in_circle,pi_estimate,time_ms\n");

    for (long total_points = MIN_POINTS; total_points <= MAX_POINTS; total_points += STEP) {
        pthread_t threads[MAX_THREADS];
        ThreadData data[MAX_THREADS];
        long points_per_thread = total_points / MAX_THREADS;

        double start = get_time_ms();

        for (int i = 0; i < MAX_THREADS; ++i) {
            data[i].points_per_thread = points_per_thread;
            data[i].points_in_circle = 0;
            data[i].seed = rand();  // Ensure each thread has a different seed
            pthread_create(&threads[i], NULL, monte_carlo_worker, &data[i]);
        }

        long total_in_circle = 0;
        for (int i = 0; i < MAX_THREADS; ++i) {
            pthread_join(threads[i], NULL);
            total_in_circle += data[i].points_in_circle;
        }

        double end = get_time_ms();
        double pi_estimate = 4.0 * total_in_circle / total_points;

        printf("n=%ld, in_circle=%ld, pi=%.6f, time=%.3f ms\n", total_points, total_in_circle, pi_estimate, end - start);
        fprintf(file, "%ld,%ld,%.8f,%.3f\n", total_points, total_in_circle, pi_estimate, end - start);

        // Save points plot for each batch (appending)
        if (total_points == MAX_POINTS || total_points == MIN_POINTS) {
            save_point_plot(total_points, total_in_circle);
        }
    }

    fclose(file);
    return 0;
}
