#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <condition_variable>
using namespace std;
int N;
int P;
int totalPrimes;
vector<int> sievePartition;
bool* candidate;
mutex mtx;
condition_variable cvar;
map<int, int> statistics;
map<int, bool> progressMap;
int index1=0;

void compute(int threadid)
{
    for (int i = 0; i < sievePartition.size(); i++)
    {
        unique_lock<mutex> lck(mtx);
        while (sievePartition[i] == 0)
        {
            cvar.wait(lck);
        }
        if (sievePartition[i] == -1)
        {
            break;
        }
        if (progressMap.at(i))
        {
            continue;
        }
        progressMap.at(i) = true;
        if (candidate[sievePartition[i]])
        {
            int temp = sqrt(N) + 1;
            while (temp % sievePartition[i] != 0)
            {
                temp++;
            }
            statistics.at(threadid) = statistics.at(threadid) + 1;
            for (int j = temp; j <= N; j += sievePartition[i])
            {
                candidate[j] = false;
            }
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

    int sqrtN = sqrt(N);
    cout<< "prime ("<< P<< ") threads over [2.." << N << "] starting ...";
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

    for (int i = 2; i <= N; i++)
    {
        candidate[i] = true;
    }

    for (int i = 2; i <= sqrt(sqrtN); i++)
    {
        if (candidate[i])
        {
            for (int j = i + i; j <= sqrtN; j += i)
            {
                candidate[j] = false;
            }
        }
    }

    for (int i = 0; i <= sqrtN; i++)
    {
        progressMap.insert({ i, false });
    }

    int index1 = 0;
    for (int i = 2; i <= sqrtN; i++)
    {
        if (candidate[i])
        {
            unique_lock<mutex> lck(mtx);
            sievePartition[index1] = i;
            index1++;
            totalPrimes++;
            lck.unlock();
            cvar.notify_all();
        }
    }
 for (int i = 0; i < sqrtN; i++)
    {
        if (sievePartition[i] == 0)
        {
            sievePartition[i] = -1;
            cvar.notify_all();
        }
    }

    for (int i = 0; i < P; i++)
    {
        t[i].join();
    }

    for (int i= sqrtN + 1; i <= N; i++)
    {
        if (candidate[i])
        {
            totalPrimes++;
        }
    }

    cout<< "prime (" << P << ") threads found " << totalPrimes << " primes"<<endl;
    cout<<"Worker Stats: [ ";
    for (int i = 0; i < P; i++)
    {
        cout<<std::to_string(statistics[i])<<", ";
    }
    cout<<"]\n";
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration<double,std::milli> (end-start);
    cout<<"time taken "<<duration.count()<<"ms"<<endl;
}