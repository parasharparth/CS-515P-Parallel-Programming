//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------

// A Pthread condition variable demo program.
//
//
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mtx;
pthread_cond_t cvar;
int done = 1;

void sender() {
  printf("Sender (tid:%ld) starts ...\n", pthread_self());

  // ... add code ...
  printf("Signalling condition variable \n");
  pthread_cond_signal(&cvar);
  printf("Signal sent!\n");
  pthread_mutex_unlock(&mtx);
}

void receiver() {
  printf("Receiver (tid:%ld) starts ...\n", pthread_self());

  // ... add code ...
  pthread_mutex_lock(&mtx);
  if(done==1)
  {
          printf("wait on the signal to come... \n");
          done=2;
          pthread_cond_wait(&cvar,&mtx);
  }
  printf("Signal received by the receiver!\n");
}

int main(int argc, char **argv) {
  pthread_t tid1, tid2, tid3, tid4;

  pthread_mutex_init(&mtx, NULL);
  pthread_cond_init(&cvar, NULL);
  //pthread_create(&tid1, NULL, (void *)sender, NULL);
  //pthread_create(&tid4, NULL, (void *)sender, NULL);
  //pthread_create(&tid2, NULL, (void *)receiver, NULL);
  //pthread_create(&tid3, NULL, (void *)receiver, NULL);
  pthread_create(&tid2, NULL, (void *)receiver, NULL);
  pthread_create(&tid1, NULL, (void *)sender, NULL);
  pthread_create(&tid3, NULL, (void *)receiver, NULL);
  pthread_create(&tid4, NULL, (void *)sender, NULL);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);
  pthread_join(tid4, NULL);
}