#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_PRODUCER_SIZE 3
#define MAX_CONSUMER_SIZE 5

typedef struct producer_s {
    sem_t *sem;
    pthread_t thread;
} producer_t;

typedef struct consumer_s {
    sem_t *sem;
    pthread_t thread;
} consumer_t;

int quit;
producer_t producers[MAX_PRODUCER_SIZE];
consumer_t consumers[MAX_CONSUMER_SIZE];

static void *producer_process_thread(void *arg) {
    producer_t *producer = (producer_t *) arg;
    while (!quit) {
        if (sem_post(producer->sem) != 0) {
            printf("sem_post failed with [%d]%s\n", errno, strerror(errno));
        } else {
            printf("producer %lu post semaphore\n", producer->thread);
        }
        sleep(1);
    }
    return NULL;
}

static void *consumer_process_thread(void *arg) {
    consumer_t *consumer = (consumer_t *) arg;
    struct timespec ts;
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    while (!quit) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        if (sem_timedwait(consumer->sem, &ts) != 0) {
            if (errno != ETIMEDOUT && errno != EINTR){
                printf("sem_wait failed with [%d]%s\n", errno, strerror(errno));
            }
        } else {
            printf("consumer %lu get semaphore\n", consumer->thread);
        }
    }
    return NULL;
}

int start_producer(sem_t *sem) {
    for (int i = 0; i < MAX_PRODUCER_SIZE; i++) {
        producer_t *producer = &producers[i];
        producer->sem = sem;
        if (pthread_create(&producer->thread, NULL, producer_process_thread, producer) != 0) {
            printf("pthread_create failed with [%d]%s\n", errno, strerror(errno));
            return -1;
        }
    }
    return 0;
}

int start_consumer(sem_t *sem) {
    for (int i = 0; i < MAX_CONSUMER_SIZE; i++) {
        consumer_t *consumer = &consumers[i];
        consumer->sem = sem;
        if (pthread_create(&consumer->thread, NULL, consumer_process_thread, consumer) != 0) {
            printf("pthread_create failed with [%d]%s\n", errno, strerror(errno));
        }
    }
    return 0;
}

sem_t sem;

int main(int argc, char *argv[]) {
    int ret;
    ret = sem_init(&sem, 0, 1);
    if (ret != 0) {
        printf("sem_init failed with [%d]%s\n", errno, strerror(errno));
        return -1;
    }

    quit = 0;
    if (start_producer(&sem) != 0) {
        printf("start producer failed\n");
        goto EXIT;
    } else {
        start_consumer(&sem);
    }

    do {
        int cmd = getchar();
        if (cmd == 'q') {
            break;
        } else {
            printf("press q to exit\n");
        }
    } while (1);

    quit = 1;

    for (size_t i = 0; i < MAX_PRODUCER_SIZE; i++) {
        producer_t *producer = &producers[i];
        pthread_join(producer->thread, NULL);
    }
    for (size_t i = 0; i < MAX_CONSUMER_SIZE; i++) {
        consumer_t *consumer = &consumers[i];
        pthread_join(consumer->thread, NULL);
    }
    EXIT:
    sem_destroy(&sem);
}