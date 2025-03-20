#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include "circular_queue.c"
#include <pthread.h>

void *thread_handle(void *arg) {
  CircularQueue *q = (CircularQueue *) arg;
  int conn_fd;
  while (1) {
    pthread_mutex_lock(&q->lock);
    while (q->current_size == 0) {
      printf("Queue is empty\n");
      pthread_cond_wait(&q->not_empty, &q->lock);
    }
    dequeue(q, &conn_fd);
    if (q->current_size == q->max_size - 1) {
      pthread_cond_signal(&q->not_full);
    }
    pthread_mutex_unlock(&q->lock);
    request_handle(conn_fd);
    printf("Request handled by a thread\n");
    close_or_die(conn_fd);
  }
  return NULL;
}

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    int num_threads = 1;
    int buffer_size = 1;
    
    while ((c = getopt(argc, argv, "d:p:t:b:")) != -1) // was "d:p:"
      switch (c) {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 't':
            num_threads = atoi(optarg);
            break;
        case 'b':
            buffer_size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
            exit(1);
      }

    // run out of this directory
    chdir_or_die(root_dir);

    // initialize the queue
    int buffer[buffer_size];
    CircularQueue q;
    init_queue(&q, buffer, buffer_size);

    // initialize the threads
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
      pthread_create(&threads[i], NULL, thread_handle, &q);
    }

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    printf("number of threads: %d\n", num_threads);
    printf("buffer size: %d\n", buffer_size);
    while (1) {
      struct sockaddr_in client_addr;
      int client_len = sizeof(client_addr);
      int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
      printf("Accepted connection from %d \n", client_addr.sin_addr.s_addr);
      pthread_mutex_lock(&q.lock);
      while (is_full(&q)) {
        printf("Queue is full\n");
        pthread_cond_wait(&q.not_full, &q.lock);
      }
      enqueue(&q, conn_fd); // signals the condition variable when the queue is not empty
      printf("Connection added to the queue\n");
      printf("Front of the queue: %d\n", q.buffer[q.head]);
      pthread_mutex_unlock(&q.lock);
    }
    
    return 0;
}


    


 
