//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// OMP nested parallel region demo.
//
#include <stdio.h>
#include <omp.h>

void f() {
#pragma omp parallel num_threads(2)
  printf("Hi from thread %d\n", omp_get_thread_num());
}

int main() {
#pragma omp parallel num_threads(2)
  {
  printf("Hello from thread %d\n", omp_get_thread_num());
  f();
  }
}
