import java.util.*;
import java.util.concurrent.PriorityBlockingQueue;

public class ParallelCalculator implements DeltaParallelCalculator  {

    DeltaReceiver receiver;
    int threads;
    int activeThreads = 0;
    int lastSend = -1;
    Hashtable<Integer,Data> dataTable = new Hashtable<>();
    Hashtable<Integer,List<Delta>> diffList = new Hashtable<>();
    PriorityBlockingQueue<Integer> queue = new PriorityBlockingQueue<>();
    static Object monitor = new Object();

    @Override
    public void setThreadsNumber(int threads) {
        this.threads = threads;
        activeThreads = 1;
        firstThread();
    }

    @Override
    public void setDeltaReceiver(DeltaReceiver receiver) {
        this.receiver = receiver;
    }

    @Override
    public void addData(Data data) {
       // System.out.println("dodaje nowa date");


            synchronized (queue){
                int n = data.getDataId();
                dataTable.put(n,data);
                if(dataTable.get(n-1) != null){
               //     System.out.println("Nowa para: "+ (n-1));

                    queue.add(n - 1);
                    queue.notifyAll();
            }
                if(dataTable.get(n+1) != null){
              //      System.out.println("Nowa para: "+ n);

                    queue.add(n);
                    queue.notifyAll();
        }
        }
    }

    public void firstThread(){
        new Thread(new Runnable (){
            public void run() {
                int index;
                synchronized (queue){
                    while (queue.peek() == null){
                        try{
                   //         System.out.println("To ja thread1  czekam na pierwsza pare");
                            queue.wait();
                        }
                        catch (Exception e){}

                    }
              //      System.out.println("To ja thread1 obudzilem sie");
                     index = queue.poll();
                }
                checkData(index);
            }
        }).start();
    }

    public void checkData (int index){
        Data a = dataTable.get(index);
        Data b = dataTable.get(index + 1);
        activeThreads = threads;
        for (int i = 0; i < threads - 1 ; i++) {
            int finalI = i;
            int finalIndex = index;
            new Thread(new Runnable (){
                public void run() {

                    for (int j = finalI; j < a.getSize(); j+=threads) {
                        int d = b.getValue(j) - a.getValue(j);
                        if( d != 0){

                            Delta delta = new Delta(finalIndex,j,d);
                      //      System.out.println("To ja thread "+finalI+" Pair: " + index + " Position: " + j + " Difference: " + d);
                            synchronized (diffList) {

                                if (diffList.get(finalIndex) == null) {
                                    diffList.put(finalIndex, new ArrayList<>());
                                }
                                diffList.get(finalIndex).add(delta);
                            }
                        }
                    }
                    activeThreads--;
                    synchronized (monitor){

                        monitor.notifyAll();
                    }
                }
            }).start();

        }
        for (int i = threads-1; i < a.getSize(); i+=threads) {
            int d = b.getValue(i) - a.getValue(i);
            if( d != 0){
                Delta delta = new Delta(index,i,d);
            //    System.out.println("To ja thread "+(threads-1)+" Pair: " + index + " Position: " + i + " Difference: " + d);
                synchronized (diffList) {

                    if (diffList.get(index) == null) {
                        diffList.put(index, new ArrayList<>());
                    }
                    diffList.get(index).add(delta);
                }
            }
        }

        synchronized (monitor){
            while(activeThreads > 1){
                try {
            //        System.out.println("to ja thread1 ide spac czekam az reszta skonczy liczyc swoje");
                    monitor.wait();
                } catch (Exception e) {

                }
            }
            trySend(index);
        }

        synchronized (queue){
            while (queue.peek() == null){
                try{
           //         System.out.println("to ja thread1 ide spac bo wszystko policzone, czekam na nowe dane");

                    queue.wait();
                }
                catch (Exception e){}

            }
      //      System.out.println("to ja thread1 wstaje bo dostalem nowe dane");
            index = queue.poll();
    }
        checkData(index);
}

public void trySend(int index) {
  //  System.out.println("Probuje wyslac dane pary: "+ index);
        if(lastSend == index - 1){
            List<Delta> data = diffList.get(index);
            while( data != null ){
         //       System.out.println("wysylam dane pary: "+index);

                receiver.accept(data);
                lastSend = index;
                index++;
                data = diffList.get(index);
            }
        }

    }
}