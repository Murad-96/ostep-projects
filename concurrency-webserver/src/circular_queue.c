#include "circular_queue.h"
#include <stdlib.h>
#include <pthread.h>

// Initialize the circular queue with a pre-allocated buffer
void init_queue(CircularQueue *q, int *buffer, int size) {
    q->buffer = buffer;
    q->max_size = size;
    q->head = 0;
    q->tail = 0;
    q->current_size = 0;
    q->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    q->not_empty = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    q->not_full = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
}

// Check if the queue is full
bool is_full(CircularQueue *q) {
    return q->current_size == q->max_size;
}

// Check if the queue is empty
bool is_empty(CircularQueue *q) {
    return q->current_size == 0;
}

// Enqueue an element
bool enqueue(CircularQueue *q, int value) {
    if (is_full(q)) {
        return false;
    }
    q->buffer[q->tail] = value;
    q->tail = (q->tail + 1) % q->max_size;
    q->current_size++;
    if (q->current_size == 1) {
        pthread_cond_signal(&q->not_empty);
    }
    return true;
}

// Dequeue an element
bool dequeue(CircularQueue *q, int *value) {
    if (is_empty(q)) {
        return false;
    }
    *value = q->buffer[q->head];
    q->head = (q->head + 1) % q->max_size;
    q->current_size--;
    return true;
}

// Free the queue resources
void free_queue(CircularQueue *q) {
    // No need to free the buffer as it is allocated on the stack
}