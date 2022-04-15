//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------

// A Pthread barrier demo program.
//
//
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

int a[3] = {5,5,5};
pthread_mutex_t mtx;
pthread_barrier_t barr;

void worker(long k) {
  pthread_mutex_lock(&mtx);
  a[k] = k;
  int i = a[(k+1)%3];
  a[k] = i;
  pthread_mutex_unlock(&mtx);
  printf("worker ended%ld\n",k);
  pthread_barrier_wait(&barr);
}

int main(int argc, char **argv) {

  pthread_t thread[3];
  pthread_mutex_init(&mtx,NULL);
  pthread_barrier_init(&barr,NULL,4);

  for (long k = 0; k < 3; k++)
    pthread_create(&thread[k], NULL, (void*)worker, (void*)k);
 // for (long k = 0; k < 3; k++)
 //  pthread_join(thread[k], NULL);
  pthread_barrier_wait(&barr);

  printf("a = [%d,%d,%d]\n", a[0], a[1], a[2]);
}