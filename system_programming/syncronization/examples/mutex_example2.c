#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct node {
    int value;
    struct node *next;
} node_t;

node_t *head = NULL;
pthread_mutex_t lock;

int match_pattern(int v) {
    return (v % 10) == 0;   // example: multiples of 10
}

void *producer(void *arg) {
    int i = 1;

    for (;;) {
        node_t *n = malloc(sizeof(node_t));
        if (!n) {
            perror("malloc");
            exit(1);
        }

        n->value = i++;

        pthread_mutex_lock(&lock);

        n->next = head;
        head = n;

        pthread_mutex_unlock(&lock);
	printf("Created node with value %d\n", n->value);

        usleep(10000);
    }

    return NULL;
}

void *consumer(void *arg) {
    for (;;) {

        //pthread_mutex_lock(&lock);

        node_t *prev = NULL;
        node_t *cur = head;

        while (cur) {
            if (match_pattern(cur->value)) {

                printf("Deleting node with value %d\n", cur->value);

                if (prev)
                    prev->next = cur->next;
                else
                    head = cur->next;

                node_t *tmp = cur;
                cur = cur->next;
                free(tmp);
            }
            else {
                prev = cur;
                cur = cur->next;
            }
        }

        //pthread_mutex_unlock(&lock);

        usleep(15000);
    }

    return NULL;
}

int main(void) {

    pthread_t prod_thread, cons_thread;

    pthread_mutex_init(&lock, NULL);

    if (pthread_create(&prod_thread, NULL, producer, NULL) != 0) {
        perror("pthread_create producer");
        return 1;
    }

    if (pthread_create(&cons_thread, NULL, consumer, NULL) != 0) {
        perror("pthread_create consumer");
        return 1;
    }

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}
