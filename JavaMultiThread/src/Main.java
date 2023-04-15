



public class Main {
    public static void main(String[] args) {

        int arr0 [] = new int[10];
        int arr1 [] = new int[10];
        int arr2 [] = new int[10];
        int arr3 [] = new int[10];
        int arr4 [] = new int[10];
        int arr5 [] = new int[10];


        for (int i = 0; i < 10; i++) {
            arr0[i] = 0;
            arr1[i] = 1;
            arr2[i] = 2;
            arr3[i] = 3;
            arr4[i] = 4;
            arr5[i] = 5;
        }

        SampleData input0 = new SampleData(0,10,arr0);
        SampleData input1 = new SampleData(1,10,arr1);
        SampleData input2 = new SampleData(2,10,arr2);
        SampleData input3 = new SampleData(3,10,arr3);
        SampleData input4 = new SampleData(4,10,arr4);
        SampleData input5 = new SampleData(5,10,arr5);

        Receiver rec = new Receiver();

        ParallelCalculator calc = new ParallelCalculator();
        calc.setDeltaReceiver(rec);
        calc.setThreadsNumber(4);
        long time =System.currentTimeMillis();

        calc.addData(input0);
        System.out.println(System.currentTimeMillis()-time);
        time =System.currentTimeMillis();

        calc.addData(input4);
        System.out.println(System.currentTimeMillis()-time);
        time =System.currentTimeMillis();

        calc.addData(input2);
        System.out.println(System.currentTimeMillis()-time);
        time =System.currentTimeMillis();

        calc.addData(input3);
        System.out.println(System.currentTimeMillis()-time);
        time =System.currentTimeMillis();

        calc.addData(input1);
        System.out.println(System.currentTimeMillis()-time);
        time =System.currentTimeMillis();

        calc.addData(input5);
        System.out.println(System.currentTimeMillis()-time);


    }
}
