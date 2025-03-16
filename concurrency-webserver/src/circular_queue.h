#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdbool.h>
#include <pthread.h>

typedef struct {
    int *buffer;
    int head;
    int tail;
    int max_size;
    int current_size;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} CircularQueue;

// Initialize the circular queue
void init_queue(CircularQueue *q, int *buffer, int size);

// Check if the queue is full
bool is_full(CircularQueue *q);

// Check if the queue is empty
bool is_empty(CircularQueue *q);

// Enqueue an element
bool enqueue(CircularQueue *q, int value);

// Dequeue an element
bool dequeue(CircularQueue *q, int *value);

// Free the queue resources
void free_queue(CircularQueue *q);

#endif // CIRCULAR_QUEUE_H