import java.util.ArrayList;
import java.util.List;

public class Receiver implements DeltaReceiver{
    List<Delta> list = new ArrayList<>();
    @Override
    public void accept(List<Delta> deltas) {
        for (Delta d:deltas ) {
            list.add(d);
            //System.out.println("Pair: " + d.getDataID() + "Position: " + d.getIdx() + "Difference: " + d.getDelta());

        }
    }
}
