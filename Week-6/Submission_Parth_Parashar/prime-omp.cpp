//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------

// A prime-finding program (Sequential version).
//
// Usage:
//   linux> ./prime N
//
#include <iostream>
#include <cmath>
#include <chrono>
#include<omp.h>
#include "unistd.h"

using namespace std;

int main(int argc, char **argv) {
  int N;
  int P = 1;
  if (argc < 2) {
    cout << "Usage: ./prime N [P]\n";
    exit(0);
  }
  if(argc == 2)
  {
  if ((N = atoi(argv[1])) < 2) {
    cout << "N must be greater than 1\n";
    exit(0);
  }
  }

  if(argc == 3)
  {
          if(((N = atoi(argv[1]))<2))
          {
                  cout<<" N must be greater than 1\n";
                  exit(0);
          }
          if(((P = atoi(argv[2]))<1))
          {
                  cout<<"P must be greater than 0\n";
                  exit(0);
          }
  }
  cout << "prime (seq) over [2.." << N << "] starting ...\n";

//insert the timing routine here
auto start = chrono::steady_clock::now();


  bool candidate[N+1];

#pragma omp parallel for
  for (int i = 2; i <= N; i++)
    candidate[i] = true;

  int sqrtN = sqrt(N);
  for (int i = 2; i <= sqrt(N); i++)
  {
    if (candidate[i])
    {
#pragma omp parallel for
      for (int j = i+i; j <= N; j += i)
        candidate[j] = false;
    }
  }

  int totalPrimes = 0;
#pragma omp parallel for shared(totalPrimes)
  for (int i = 2; i <= N; i++)
    if (candidate[i])
    {
      totalPrimes++;
    }
auto end = chrono::steady_clock::now();
auto duration = chrono::duration<double,std::milli> (end-start);
//end the timing routine here
//timings will be displayed in the below cout statement
  cout << "prime (seq) found " << totalPrimes << " primes in "<< duration.count()<<"ms\n";

}