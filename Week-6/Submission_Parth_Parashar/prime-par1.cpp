#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <string>
using namespace std;
int N;
int P;
int totalPrimes;
int* sievePartition;
bool* candidate;
mutex consoleMtx;   //lock for locking the console outputs (couts)
mutex mtx;          //lock for locking the given critical sections
bool isPrimeNum(int i);  //function for checking whether the given number is prime or not

void compute(int threadid)
{
    int startingidx, endingidx;
    int remaining = N - sqrt(N);
    int div = (remaining) / P;
    startingidx = (sqrt(N)) + (div * threadid) + 1;
    if (threadid  == (P-1))
    {
        endingidx = N;
    }
    else
    {
        endingidx = (sqrt(N)) + (div * (threadid + 1));
    }

    consoleMtx.lock();
    cout<<"Starting thread with ID: " << threadid << ". Starting index: "<< startingidx << " Ending index: " <<endingidx << endl;
    consoleMtx.unlock();

    for (int i = startingidx; i <= endingidx; i++)
    {
        candidate[i] = true;
    }
    for (int i = 0; i < sqrt(N); i++)
    {
        if (sievePartition[i] == -1)
            break;
        if (candidate[sievePartition[i]])
        {
                int temp = startingidx;
                while (temp % sievePartition[i] != 0)
                {
                        temp++;
                }
            for (int j = temp; j <= endingidx; j += sievePartition[i])
            {
                candidate[j] = false;
            }
        }
    }
for (int i = startingidx; i <= endingidx; i++)
    {
        unique_lock<mutex> lck(mtx);
        if (candidate[i])
        {
            totalPrimes++;
        }
        lck.unlock();
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: ./prime N\n";
        exit(0);
    }
    if (argc == 2)
    {
        P = 1;
        if ((N = atoi(argv[1])) < 2) {
            cout << "N must be greater than 1\n";
            exit(0);
        }
    }
    if (argc > 2)
    {
        if ((N = atoi(argv[1])) < 2) {
            cout << "N must be greater than 1\n";
            exit(0);
        }
        if ((P = atoi(argv[2])) <=0 )
        {
            cout << "P must be greater than 0\n";
            exit(0);
        }
    }

consoleMtx.lock();
cout<<"prime (seq) over [2.." << N << "] starting ..."<<endl;
consoleMtx.unlock();

//timings will start from here
auto start = chrono::steady_clock::now();

    int sqrtN = sqrt(N);
    candidate = new bool[N + 1];
    sievePartition = new int [sqrtN];
    for (int i = 2; i <= sqrtN; i++)
    {
        candidate[i] = true;
    }
    for (int i = 2; i <= sqrt(sqrtN); i++)
    {
        if (candidate[i])
        {
            for (int j = i+i; j <= sqrtN; j += i)
            {
                candidate[j] = false;
            }
        }
    }
    int index = 0;
    for (int i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            sievePartition[index] = i;
            index++;
            totalPrimes++;
        }
    }
 for (int i = 0; i < sqrtN; i++)
    {
        if (i+1 > totalPrimes)
        {
            sievePartition[i] = -1;
        }
    }

 //creating the threads
    thread t[P];
    for (int i = 0; i < P; i++)
    {
        t[i] = thread(compute, i);
    }
    for (int i = 0; i < P; i++)
    {
        t[i].join();
    }

auto end = chrono::steady_clock::now();
auto duration = chrono::duration<double,std::milli> (end-start);
//timings will end here

    consoleMtx.lock();
    cout<<"prime (seq) found " << totalPrimes<< " primes"<<endl;
    cout<<"Time taken is:  "<<duration.count()<<"ms\n"<<endl;
    consoleMtx.unlock();
}