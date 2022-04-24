//package prodcons1;

import java.util.LinkedList;
import java.util.Queue;


public class ProdCons1 {
        public static int BUFFSIZE = 20;
        public static int NUMITEMS = 100;
        public static void main(String args[]) throws InterruptedException
        {
                Queue<Integer> sharedQueue = new LinkedList<Integer>();
                Thread prodThread = new Thread(new Producer(sharedQueue, BUFFSIZE,NUMITEMS), "Producer");
        Thread consThread = new Thread(new Consumer(sharedQueue, BUFFSIZE, NUMITEMS), "Consumer");
        prodThread.start();
        consThread.start();
        prodThread.join();
        consThread.join();
        }
}

class Producer implements Runnable {

    private final Queue<Integer> sharedQueue;
    private final int SIZE;
    private final int NUMITEMS;

    public Producer(Queue<Integer> sharedQueue, int buffsize, int numitems) {
        this.sharedQueue = sharedQueue;
        this.SIZE = buffsize;
        this.NUMITEMS = numitems;
    }

    @Override
    public void run() {
        for (int i = 0; i <= NUMITEMS; i++) {
            System.out.println("Produced: " + i);
            try {
                produce(i);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        System.out.println("Producer ends");
    }

    private void produce(int i) throws InterruptedException {

        //wait if the queue is full
        while (sharedQueue.size() == SIZE) {
            synchronized (sharedQueue) {
                System.out.println("The queue is full " + Thread.currentThread().getName()
                                    + " is waiting , size: " + sharedQueue.size());
                sharedQueue.wait();
            }
        }

        //producing element and notify consumers
        synchronized (sharedQueue) {
            sharedQueue.add(i);
            sharedQueue.notifyAll();
        }

    }
}

class Consumer implements Runnable {

    private final Queue<Integer> sharedQueue;
    @SuppressWarnings("unused")
        private final int SIZE;
    private final int NUMITEMS;

    public Consumer(Queue<Integer> sharedQueue, int size, int numitems) {
        this.sharedQueue = sharedQueue;
        this.SIZE = size;
        this.NUMITEMS = numitems;
    }

    public void run() {
        int returnValue = 0;
        while (returnValue != NUMITEMS) {
            try
            {
                 returnValue = consume();
                 System.out.println("Consumed: " + returnValue);
                if(returnValue == 100)
                        {
                        System.out.println("Consumer ends");
                        }
            }
            catch (InterruptedException ex)
            {
                ex.printStackTrace();
            }
        }
    }

    private int consume() throws InterruptedException {
        //wait if the queue is empty
        while (sharedQueue.isEmpty())
        {
            synchronized (sharedQueue)
            {
                System.out.println("The queue is empty " + Thread.currentThread().getName()
                                    + " is waiting , size: " + sharedQueue.size());
                        sharedQueue.wait();
            }
        }

        //Otherwise consume element and notify the waiting producer
        synchronized (sharedQueue)
        {
            sharedQueue.notifyAll();
            return (Integer) sharedQueue.remove().intValue();
        }
    }
}