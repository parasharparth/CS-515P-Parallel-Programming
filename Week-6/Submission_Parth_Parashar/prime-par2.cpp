#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <condition_variable>
#include <atomic>
using namespace std;
int N;
int P;
int totalPrimes;
vector<int> sievePartition;
bool* candidate;
mutex mtx;
condition_variable cvar;
int totalSieves = 1;
map<int, int> statistics;
int index1=0;

void compute(int threadid)
{
    int startingidx, endingidx;
    int remaining = N - sqrt(N);
    int div = (remaining) / P;
    startingidx = (sqrt(N)) + (div * threadid) + 1;

    if (threadid == (P - 1))
    {
        endingidx = N;
    }
    else
    {
        endingidx = (sqrt(N)) + (div * (threadid + 1));
    }

    for (int i = startingidx; i <= endingidx; i++)
    {
        candidate[i] = true;
    }

    for (int i = 0; i < sievePartition.size(); i++)
    {

        if (i >= totalSieves && i>1)
        {
            break;
        }
        unique_lock<mutex> lck(mtx);
        while (sievePartition[i] == 0)
        {
            cvar.wait(lck);
        }
        lck.unlock();

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
                //cout<< "threadID: " << threadid << ". i: " << i << " j: " <<j<<endl;
            }
        }
       // cvar.notify_all();
    }
    for (int i = startingidx; i <= endingidx; i++)
    {
        if (candidate[i])
        {
            unique_lock<mutex> lck(mtx);
            totalPrimes++;
            lck.unlock();
        }
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

    int sqrtN = sqrt(N);
    cout<<"prime (seq) over [2.." << N << "] starting ..."<<endl;
    auto start = chrono::steady_clock::now();
    candidate = new bool[N + 1];
    sievePartition.resize(sqrtN);
    for (int i = 0; i < P; i++)
    {
        statistics.insert({ i, 0 });
    }
    thread t[P];
    for (int i = 0; i < P; i++)
    {
        t[i] = thread(compute, i);
    }
    for (int i = 2; i <= sqrtN; i++)
    {
        candidate[i] = true;
    }

    for(int i = 2; i<sqrtN; i++)
    {
            if(candidate[i])
            {
                    for(int j=i+1; j<=sqrtN; j+=i)
                    {
                        candidate[j] = false;
                    }
            }
    }

    int index1 = 0;
    unique_lock<mutex> lck(mtx);
    for (int i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            sievePartition[index1] = i;
            index1++;
            totalPrimes++;
        }
    }

    totalSieves = index1;
    lck.unlock();
    cvar.notify_all();

    for (int i = 0; i < P; i++)
    {
        t[i].join();
    }
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration<double,std::milli> (end-start);
    cout<<"prime (seq) found "<< totalPrimes<< " primes"<<endl;
    cout<<"time"<<duration.count()<<"ms"<<endl;
}