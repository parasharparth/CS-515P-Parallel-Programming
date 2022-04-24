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
int NUMBEROFPRODUCERS = 1;
int producerresults[1000] = {0};
int consumerresults[1000] = {0};
Queue q(0);

mutex mtx;
condition_variable cond;


void producer(int k) {

    //division in segment per producer
    int segment[2];
    int per_producer = NUMITEMS/NUMBEROFPRODUCERS;
    segment[0] = per_producer * k + 1;
    segment[1] = per_producer * (k+1);
    if (k == NUMBEROFPRODUCERS - 1){
        segment[1] = NUMITEMS;
    }
    int produced = 0;

    cout<<"Produce number["<<k<<"] for segment ["<<segment[0]<<".."<<segment[1]<<"] starting on core "<< sched_getcpu() <<endl;
    for(int i=segment[0]; i<=segment[1]; i++) {
        unique_lock<mutex> lck(mtx);
        while (q.isFull()) {
            cond.wait(lck);
        }

        //adding the element to the queue
        q.add(i);

        //incrementing the produced count
        produced += 1;

        //notifying the threads about produced data
        cond.notify_all();

        //unlocking the locked condition
        lck.unlock();

        cout<<"Producer number ["<<k<<"] added to the producer list "<<i<<"(qsz: "<<q.size()<<")"<<endl;
    }
    producerresults[k] = produced;
    cout<<"Producer number["<<k<<"] end"<<endl;
}

void consumer(int k) {
    int consumed = 0;
    cout<<"Consumer number ["<<k<<"] starting on core number "<< sched_getcpu() <<endl;
    while (true) {

        //locking the wait condition
        unique_lock<mutex> lck(mtx);
        while (q.isEmpty()) {
            cond.wait(lck);
        }

        //removing the element from the queue
        int element_removed = q.remove();
        cond.notify_all();

        //checking the element removed and if it is -1, then it is the ending condition
        if (element_removed == -1) {
            lck.unlock();
            cout<<"Consumer number["<<k<<"] rem'd (qsz: "<<q.size()<<")"<<endl;
            break;
        }

        //incrementing the consumed count
        consumed += 1;
        lck.unlock();
        cout<<"Consumer number["<<k<<"] rem'd "<<element_removed<<"(qsz: "<<q.size()<<")"<<endl;
    }

    //putting the consumer results for building the statistics
    consumerresults[k] = consumed;
    cout<<"Consumer number :- "<<k<<"] end"<<endl;
}

int main(int argc, char **argv) {
    q = Queue(BUFFSIZE);

    if (argc == 2) {
        int numberofconsumers = stoi(argv[1]);
        if (numberofconsumers > 0) {
            NUMBEROFCONSUMERS = numberofconsumers;
        }
    } else if (argc == 3) {
        int numberofconsumers = stoi(argv[1]);
        if (numberofconsumers > 0) {
            NUMBEROFCONSUMERS = numberofconsumers;
        }

        int numberofproducers = stoi(argv[2]);
        if (numberofproducers > 0) {
            NUMBEROFPRODUCERS = numberofproducers;
        }
    }

    cout<<"Prodcons3 with "<<NUMBEROFCONSUMERS<<" consumers, "<<NUMBEROFPRODUCERS<<" producers"<<endl;

    thread *ct = new thread[NUMBEROFCONSUMERS];
    thread *pt = new thread[NUMBEROFPRODUCERS];

    int i;

    // starting the consumers
    for(i=0; i<NUMBEROFCONSUMERS; i++) {
        ct[i] = thread(consumer, i);
    }
    // starting the producers
    for(i=0; i<NUMBEROFPRODUCERS; i++) {
        pt[i] = thread(producer, i);
    }

    // joining the producers
    for(i=0;i<NUMBEROFPRODUCERS;i++) {
        pt[i].join();
    }

    // locking for termination messages
    for(i=0; i<NUMBEROFCONSUMERS; i++) {
        unique_lock<mutex> lck(mtx);
        while (q.isFull()) {
            cond.wait(lck);
        }
        q.add(-1);
        cond.notify_all();
        lck.unlock();
    }

    // joining the consumers
    for(i=0;i<NUMBEROFCONSUMERS;i++) {
        ct[i].join();
    }

        //producing the statistics for the consumers
    string consumerstatsoutput = "the statistics for consumers are: [";
    int consumed_count = 0;
    for(i=0;i<NUMBEROFCONSUMERS;i++){
        consumed_count += consumerresults[i];
        consumerstatsoutput += to_string(consumerresults[i]) + ",";
    }
    consumerstatsoutput += "] total = " + to_string(consumed_count);
    cout<<consumerstatsoutput<<endl;


    //producing the statistics for the producers
    string producerstatsoutput = "the statistics for producer are: [";
    int produced_count = 0;
    for(i=0;i<NUMBEROFPRODUCERS;i++){
        produced_count += producerresults[i];
        producerstatsoutput += to_string(producerresults[i]) + ",";
    }
    producerstatsoutput += "] total = " + to_string(produced_count);
    cout<<producerstatsoutput<<endl;
    cout<<"Main: all done!"<<endl;
    return 0;
}