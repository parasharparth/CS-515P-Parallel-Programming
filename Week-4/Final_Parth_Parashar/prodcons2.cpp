#include<stdio.h>
#include<iostream>
#include<thread>
#include "queue.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <sched.h>
#include <string>
using namespace std;

int BUFFSIZE = 20;
int NUMITEMS = 100;
int NUMBEROFCONSUMERS = 1;
int resultlist[1000] = {0};
Queue q(0);

mutex mtx;
condition_variable cond;

//producer single thread
void producer() {
    cout<<"Producer started on core number "<< sched_getcpu() <<endl;
    for(int i=1; i<=NUMITEMS; i++) {

        //locking the wait condition till we get the signal
        unique_lock<mutex> lck(mtx);
        while (q.isFull()) {
            cond.wait(lck);
        }

        //adding the element
        q.add(i);

        cout<<"Producer added "<<i<<"(qsz: "<<q.size()<<")"<<endl;

        //sending the notify signal to tell that the element is added and can be consumed
        cond.notify_all();
        lck.unlock();
    }
    for(int i=0; i<NUMBEROFCONSUMERS; i++) {

        //locking the wait condition
        unique_lock<mutex> lck(mtx);
        while (q.isFull()) {
            cond.wait(lck);
        }

        //adding the (-1) element
        q.add(-1);

        //notifying all about the add operation
        cond.notify_all();
        lck.unlock();
    }

    cout<<"Producer end"<<endl;
}

void consumer(int k) {

    int consumedcount = 0;
    cout<<"Consumer number ["<<k<<"] started on core number "<< sched_getcpu() <<endl;
    while (true) {

        //locking the wait condition
        unique_lock<mutex> lck(mtx);
        while (q.isEmpty()) {
            cond.wait(lck);
        }

        //removing the element from the queue
        int elementremoved = q.remove();

        //sending the notify signal that element has been removed
        cond.notify_all();

        //checking the ending condition
        if (elementremoved == -1) {
            lck.unlock();
            break;
        }

        cout<<"Consumer["<<k<<"] rem'd "<<elementremoved<<"(qsz: "<<q.size()<<")"<<endl;
        consumedcount += 1;

        lck.unlock();
    }

    //updating the result list
    resultlist[k] = consumedcount;
    cout<<"Stopping Consumer number : -"<<k<<endl;
}

int main(int argc, char **argv) {
    q = Queue(BUFFSIZE);

    //getting the number of consumer threads from the user
    if (argc == 2) {
        int numberofconsumers = stoi(argv[1]);
        if (numberofconsumers > 0) {
            NUMBEROFCONSUMERS = numberofconsumers;
        }
    }

    int i;

    //initializing the threads
    thread *th = new thread[NUMBEROFCONSUMERS+1];

    //starting the threads for the consumers
    for(i=0; i<NUMBEROFCONSUMERS; i++) {
        th[i] = thread(consumer, i);
    }

    //starting the thread for the producer
    th[i] = thread(producer);

    //joining the threads
    for(i=0;i<NUMBEROFCONSUMERS+1;i++) {
        th[i].join();
    }

    string output = "the statistics for the consumers are:- [";
    int countofconsumedconsumers = 0;
    for(i=0;i<NUMBEROFCONSUMERS;i++){
        countofconsumedconsumers += resultlist[i];
        output += to_string(resultlist[i]) + ",";
    }
    output += "] total = " + to_string(countofconsumedconsumers);
    cout<<output<<endl;
    cout<<"Main: all done!"<<endl;
    return 0;
}