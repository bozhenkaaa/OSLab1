#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define PHILOSOPHERS 5
#define MAX_MEALS 10

pthread_mutex_t forks[PHILOSOPHERS];
pthread_t philosophers[PHILOSOPHERS];

void think_or_eat(int number, const char* action) {
    printf("Philosopher %d %s.\n", number, action);
    sleep(1 + rand() % 2); // Waits for a random amount of time, simulating activity
}

void* philosopher(void* num) {
    int philosopher_number = *(int*)num;
    int meals = 0;

    while (meals < MAX_MEALS) {
        // philosopher is thinking
        think_or_eat(philosopher_number, "is thinking");

        // Randomly choose which fork to try to pick up first
        int first = philosopher_number;
        int second = (philosopher_number + 1) % PHILOSOPHERS;

        // block of first fork
        pthread_mutex_lock(&forks[first]);
        printf("Philosopher %d took the fork %d.\n", philosopher_number, first);

        // try to take another fork
        if (pthread_mutex_trylock(&forks[second]) == 0) {
            printf("Philosopher %d took the fork %d and starts eating.\n", philosopher_number, second);
            think_or_eat(philosopher_number, "is eating");
            meals++;
            pthread_mutex_unlock(&forks[second]); // the second fork
            printf("Philosopher %d put down the fork %d.\n", philosopher_number, second);
        } else {
            // If unable to take the second fork, put down the first one
            printf("Philosopher %d failed to take fork %d and continues to think.\n", philosopher_number, second);
        }
        pthread_mutex_unlock(&forks[first]); // the first fork
        printf("Philosopher %d put down the fork %d.\n", philosopher_number, first);

        if (meals >= MAX_MEALS) {
            printf("Philosopher %d has finished eating and is leaving the table.\n", philosopher_number);
            break;
        }
    }

    return NULL;
}


int main() {
    int i, numbers[PHILOSOPHERS];

    srand(time(NULL));

    for (i = 0; i < PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    for (i = 0; i < PHILOSOPHERS; i++) {
        numbers[i] = i;
        if (pthread_create(&philosophers[i], NULL, philosopher, &numbers[i])) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for (i = 0; i < PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}