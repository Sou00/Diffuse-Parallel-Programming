public class SampleData implements Data{
    int [] values;
    int id;
    int size;
    SampleData(int id, int size, int []arr){
        this.id = id;
        this.size=size;
        values = new int[size];
        for (int i = 0; i < size; i++) {
            values[i] = arr[i];
        }
    }

    @Override
    public int getDataId() {
        return id;
    }

    @Override
    public int getSize() {
        return size;
    }

    @Override
    public int getValue(int idx) {
        return values[idx];
    }
}
