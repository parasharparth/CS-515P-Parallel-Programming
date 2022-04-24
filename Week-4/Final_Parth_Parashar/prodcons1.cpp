//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------
//
// A Pthread producer-consumer program written in c++
// Author:- Parth Parashar

#include<iostream>
#include<thread>
#include"queue.h"
#include<mutex>
#include<condition_variable>
using namespace std;

#define BUFFSIZE 20
#define NUMITEMS 100
Queue q(0);
mutex mtx;
condition_variable cvar;


void producer()
{
        int core = sched_getcpu();
        cout<<"producer started on core "<<core<<endl;
        for(int i=0; i<=NUMITEMS; i++)
        {
                unique_lock<mutex> lck(mtx);
                while(q.isFull()== true)
                {
                        cout<<"the queue is full"<<endl;
                        cvar.wait(lck);
                        cout<<"producer() : waiting"<<endl;
                        lck.unlock();
                }

                        q.add(i);
                        cout<<"producer: qsize():- "<<q.size()<<endl;
                        cvar.notify_one();

        }
        cout<<"producer ends"<<endl;
}

void consumer()
{
        int core = sched_getcpu();
        cout<<"consumer started on core "<<core<<endl;
        for(int i=0 ;i< NUMITEMS; i++)
        {
                unique_lock<mutex> lck(mtx);
                while(q.isEmpty() == true)
                {
                        cvar.wait(lck);
                        cout<<"consumer(): waiting"<<endl;
                        lck.unlock();
                }

                        q.remove();
                        cout<<"consumer: size():- "<<q.size()<<endl;
                        cvar.notify_one();
        }
        cout<<"consumer ends"<<endl;
}

int main(int argc, char **argv)
{
        cout<<"this is the main program"<<endl;
        q=Queue(BUFFSIZE);
        cout<<"the size of the queue is: -"<<q.size()<<endl;
        thread thread1(producer);
        thread thread2(consumer);
        thread1.join();
        thread2.join();
        cout<<"Main: all done()"<<endl;
        return 1;
}