#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define MAX_PHILOSOPHERS 27

pthread_mutex_t chopstick[MAX_PHILOSOPHERS];
pthread_cond_t condition[MAX_PHILOSOPHERS];

int num_philosophers;
int min_think, max_think, min_dine, max_dine;
int dining_times;
char *distribution;
int hungry_durations[MAX_PHILOSOPHERS];

int random_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void think(int id) {
    int think_time;
    if (strcmp(distribution, "uniform") == 0) {
        think_time = random_range(min_think, max_think);
    } else if (strcmp(distribution, "exponential") == 0) {
        double mean = (min_think + max_think) / 2.0;
        do {
            think_time = -log(1.0 - ((double)rand() / RAND_MAX)) * mean;
        } while (think_time < min_think || think_time > max_think);
    } else {
        fprintf(stderr, "Invalid distribution type.\n");
        exit(EXIT_FAILURE);
    }

    printf("Philosopher %d is thinking for %d ms.\n", id, think_time);
    usleep(think_time * 1000);
}

void eat(int id) {
    int dine_time;
    if (strcmp(distribution, "uniform") == 0) {
        dine_time = random_range(min_dine, max_dine);
    } else if (strcmp(distribution, "exponential") == 0) {
        double mean = (min_dine + max_dine) / 2.0;
        do {
            dine_time = -log(1.0 - ((double)rand() / RAND_MAX)) * mean;
        } while (dine_time < min_dine || dine_time > max_dine);
    } else {
        fprintf(stderr, "Invalid distribution type.\n");
        exit(EXIT_FAILURE);
    }

    printf("Philosopher %d is eating for %d ms.\n", id, dine_time);
    usleep(dine_time * 1000);
}

void *philosopher(void *arg) {
    int id = *(int *)arg;
    struct timespec start_time, end_time;
    
    for (int i = 0; i < dining_times; ++i) {
        think(id);

        pthread_mutex_lock(&chopstick[id]);
        pthread_mutex_lock(&chopstick[(id + 1) % num_philosophers]);

        printf("Philosopher %d is picking up chopsticks %d and %d.\n", id, id, (id + 1) % num_philosophers);

        pthread_mutex_unlock(&chopstick[id]);
        pthread_mutex_unlock(&chopstick[(id + 1) % num_philosophers]);

        eat(id);

        pthread_mutex_lock(&chopstick[id]);
        pthread_mutex_lock(&chopstick[(id + 1) % num_philosophers]);

        printf("Philosopher %d is putting down chopsticks %d and %d.\n", id, id, (id + 1) % num_philosophers);

        pthread_mutex_unlock(&chopstick[id]);
        pthread_mutex_unlock(&chopstick[(id + 1) % num_philosophers]);

        // Avoid immediate rethinking by introducing a small delay
        usleep(1000);
    }
    
    // Record start time
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	pthread_mutex_lock(&chopstick[id]);
	pthread_mutex_lock(&chopstick[(id + 1) % num_philosophers]);

    // Record end time
	clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate hungry state duration in milliseconds
	int hungry_duration = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                      (end_time.tv_nsec - start_time.tv_nsec) / 1000000;

	printf("Philosopher %d was hungry for %d ms.\n", id, hungry_duration);

    // Other philosopher actions (eating, putting down chopsticks) continue...

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "Usage: %s <num_phsp> <min_think> <max_think> <min_dine> <max_dine> <dst> <num>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    num_philosophers = atoi(argv[1]);
    min_think = atoi(argv[2]);
    max_think = atoi(argv[3]);
    min_dine = atoi(argv[4]);
    max_dine = atoi(argv[5]);
    distribution = argv[6];
    dining_times = atoi(argv[7]);

    if (num_philosophers <= 0 || num_philosophers > MAX_PHILOSOPHERS || min_think <= 0 || max_think <= 0 ||
        min_dine <= 0 || max_dine <= 0 || dining_times <= 0) {
        fprintf(stderr, "Invalid input parameters.\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize hungry_durations array
    int hungry_durations[MAX_PHILOSOPHERS];
    for (int i = 0; i < num_philosophers; ++i) {
        hungry_durations[i] = 0;
    }

    pthread_t philosophers[MAX_PHILOSOPHERS];
    int philosopher_ids[MAX_PHILOSOPHERS];

    for (int i = 0; i < num_philosophers; ++i) {
        pthread_mutex_init(&chopstick[i], NULL);
        pthread_cond_init(&condition[i], NULL);
    }
    
    for (int i = 0; i < num_philosophers; ++i) {
        philosopher_ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, (void *)&philosopher_ids[i]);
    }

    for (int i = 0; i < num_philosophers; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < num_philosophers; ++i) {
        pthread_mutex_destroy(&chopstick[i]);
        pthread_cond_destroy(&condition[i]);
    }
    
    int total_hungry_duration = 0;

    for (int i = 0; i < num_philosophers; ++i) {
    	total_hungry_duration += hungry_durations[i];
    }

    double average_hungry_duration = (double)total_hungry_duration / num_philosophers;

    double sum_squared_differences = 0.0;
    for (int i = 0; i < num_philosophers; ++i) {
    	sum_squared_differences += pow(hungry_durations[i] - average_hungry_duration, 2);
    }

    double standard_deviation = sqrt(sum_squared_differences / num_philosophers);

    printf("Average Hungry State Duration: %.2f ms\n", average_hungry_duration);
    printf("Standard Deviation of Hungry State: %.2f ms\n", standard_deviation);

    return 0;
}
