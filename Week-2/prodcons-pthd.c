//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------

// A Pthread producer-consumer program.
//
//
#include <stdio.h>
#include <pthread.h>
#define BUFSIZE  20
#define NUMITEMS 100
int buffer[BUFSIZE];
int idx = 0;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void producer() {
  printf("Producer starting\n");
  pthread_mutex_lock(&lock1);
  if(idx == BUFSIZE)
  {
      pthread_cond_wait(&cond1, &lock1);
  }
  for (int i = 1; i <= NUMITEMS; i++) {
    buffer[idx++] = i;
    pthread_cond_signal(&cond2);
    pthread_mutex_unlock(&lock2);
    printf("Producer added %d (bsz: %d)\n", i, idx);
  }
  printf("Producer ending\n");
}

void consumer() {
  printf("Consumer starting\n");
  pthread_mutex_lock(&lock2);
  if(idx == 0)
  {
      pthread_cond_wait(&cond2,&lock2);
  }
  for (int i = 1; i <= NUMITEMS; i++) {
    int val = buffer[--idx];
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&lock1);
    printf("Consumer rem'd %d (bsz: %d)\n", val, idx);
  }
  printf("Consumer ending\n");
}

int main(int argc, char **argv) {
  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, (void*)producer, NULL);
  pthread_create(&tid2, NULL, (void*)consumer, NULL);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  printf("Main: all done!\n");
}