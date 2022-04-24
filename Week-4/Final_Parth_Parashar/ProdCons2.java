//package prodcons2;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Queue;


public class ProdCons2 {
        public static int BUFFSIZE = 20;
        public static int NUMITEMS = 100;
        public static HashMap<Integer,String> result= new HashMap<Integer,String>();
        public static int printresult = 0;

        public static void main(String args[])
        {
                int numberOfConsumers = 0;
                Queue<Integer> sharedQueue = new LinkedList<Integer>();
                if (args.length > 0) {
                        numberOfConsumers = Integer.parseInt(args[0]);

                        Thread prodThread = new Thread(new Producer(sharedQueue, BUFFSIZE,NUMITEMS,numberOfConsumers), "Producer");
                        Thread[] consThreads = new Thread[numberOfConsumers];
                        prodThread.start();
                        for(int i=0;i<numberOfConsumers; i++)
                        {
                                consThreads[i] = new Thread(new Consumer(sharedQueue, BUFFSIZE, NUMITEMS,i,result), "Consumer");
                                consThreads[i].start();
                                System.out.println("The consumer thread started is:- "+i);
                        }

                        //joining the threads
                        try {
                                prodThread.join();
                        } catch (InterruptedException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                        }
                        for(int i = 0; i< numberOfConsumers; i++)
                        {
                                try {
                                        consThreads[i].join();
                                } catch (InterruptedException e) {
                                        // TODO Auto-generated catch block
                                        e.printStackTrace();
                                }
                        }
                }

                else
                {
                        Thread prodThread = new Thread(new Producer(sharedQueue, BUFFSIZE,NUMITEMS,numberOfConsumers), "Producer");
                        Thread consThread = new Thread(new Consumer(sharedQueue, BUFFSIZE, NUMITEMS,0,result), "Consumer");
                        prodThread.start();
                        consThread.start();
                        try {
                                prodThread.join();
                        } catch (InterruptedException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                        }
                        try {
                                consThread.join();
                        } catch (InterruptedException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                        }
                }

                //printing the stats
                System.out.println("The statistics are: -");
                System.out.println("The statistics include the -1 consumed by the consumer");
                System.out.println("So the number of Items consumed by the consumer will be displayed including the -1");
                for(int k=0;k<result.size();k++)
                {
                        System.out.println(result.get(k));
                }
        }
}

class Producer implements Runnable {

    private final Queue<Integer> sharedQueue;
    private final int SIZE;
    private final int NUMITEMS;
    private final int NUMBEROFCONSUMERS;

    public Producer(Queue<Integer> sharedQueue, int buffsize, int numitems, int numberOfConsumers) {
        this.sharedQueue = sharedQueue;
        this.SIZE = buffsize;
        this.NUMITEMS = numitems;
        this.NUMBEROFCONSUMERS = numberOfConsumers;
    }

    @Override
    public void run() {
        for (int i = 1; i <=NUMITEMS; i++) {
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
                sharedQueue.wait();
            }
        }

        //producing element and notify consumers
        synchronized (sharedQueue) {
            if(i == NUMITEMS)
            {
                System.out.println("Producing 100th element");
                sharedQueue.add(i);
                for(int j=0;j<NUMBEROFCONSUMERS; j++)
                {
                        System.out.println("Adding the -1's");
                        sharedQueue.add((-1));
                }
                if(NUMBEROFCONSUMERS == 0)
                {
                        System.out.println("Adding the -1's");
                    sharedQueue.add((-1));
                }
                sharedQueue.notifyAll();
            }
            else
            {
                sharedQueue.add(i);
                sharedQueue.notifyAll();
            }
        }
    }
}

class Consumer implements Runnable {

    private final Queue<Integer> sharedQueue;
    @SuppressWarnings("unused")
        private final int SIZE;
    private final int NUMITEMS;
    private final int CONSUMER_NUMBER;
    private final HashMap<Integer,String> RESULT;
    public Consumer(Queue<Integer> sharedQueue, int size, int numitems,int consumer_number,HashMap<Integer,String> result) {
        this.sharedQueue = sharedQueue;
        this.SIZE = size;
        this.NUMITEMS = numitems;
        this.CONSUMER_NUMBER = consumer_number;
        this.RESULT =result;
    }

    @SuppressWarnings("deprecation")
        public void run() {
        int returnValue = 0;
        int count = 0;
        while (true) {
            try
            {
                synchronized(sharedQueue)
                {
                        returnValue = consume();
                        System.out.println("Consumed item: " + returnValue + " consumed by:- "+CONSUMER_NUMBER);
                        count = count+1;

                if(returnValue == (-1))
                        {
                        //System.out.println("Consumer number:- "+CONSUMER_NUMBER+ " ends as it encountered -1");
                        String finalresult = "Number of Consumed items by Consumer Number "+CONSUMER_NUMBER+" are:- "+count;
                        RESULT.put(CONSUMER_NUMBER, finalresult);
                        Thread.currentThread().stop();
                        }
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