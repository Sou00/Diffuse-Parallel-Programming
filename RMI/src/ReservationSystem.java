import java.net.MalformedURLException;
import java.rmi.AlreadyBoundException;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.*;

public class ReservationSystem {
    public ReservationSystem() {
        try {
            Cinema service = new Service();
            Cinema cinemaService = (Cinema) UnicastRemoteObject.exportObject(service, 0);
            java.rmi.Naming.rebind(Cinema.SERVICE_NAME,cinemaService);
        } catch ( MalformedURLException | RemoteException e) {
            //e.printStackTrace();
        }
    }
}

class Service implements Cinema{

    Set<Integer> freeSeats = new HashSet<>();
    Hashtable<Integer,String> takenSeats = new Hashtable<>();
    Hashtable<Integer,String> reservedSeats = new Hashtable<>();
    Hashtable<String,Set<Integer>> reservations = new Hashtable<>();
    Hashtable<String, Long> reservationTime = new Hashtable<>();
    long time;

    @Override
    public void configuration(int seats, long timeForConfirmation) throws RemoteException {

        synchronized (freeSeats) {

            for (int i = 0; i < seats; i++) {
                freeSeats.add(i);
            }

            time = timeForConfirmation;
        }
    }

    @Override
    public Set<Integer> notReservedSeats() throws RemoteException {
       synchronized (freeSeats){
           List<Integer> list = new ArrayList<>();
           for (int seat : reservedSeats.keySet()){
                   if (System.currentTimeMillis() - reservationTime.get(reservedSeats.get(seat)) > time) {
                       list.add(seat);
                       freeSeats.add(seat);
                   }
           }
           for (int seat : list){
               reservedSeats.remove(seat);
           }

       }
        return freeSeats;
    }

    @Override
    public boolean reservation(String user, Set<Integer> seats) throws RemoteException {
        synchronized (freeSeats) {
            if (freeSeats.containsAll(seats)) {
                freeSeats.removeAll(seats);

                for (int seat : seats) {
                    reservedSeats.put(seat, user);
                }
                reservations.put(user, seats);
                reservationTime.put(user, System.currentTimeMillis());
                return true;
            } else {
                for (int seat : seats) {
                    if (takenSeats.containsKey(seat))
                        return false;
                    if (reservedSeats.containsKey(seat)) {
                        if (System.currentTimeMillis() - reservationTime.get(reservedSeats.get(seat)) < time) {
                            return false;
                        }
                    }
                }
                for (int seat : seats) {
                    if (freeSeats.contains(seat)) {
                        freeSeats.remove(seat);
                        reservedSeats.put(seat, user);
                    }
                    if (reservedSeats.containsKey(seat)) {
                        reservedSeats.put(seat, user);
                    }
                }
                reservations.put(user, seats);
                reservationTime.put(user, System.currentTimeMillis());

                return true;
            }
        }
    }

    @Override
    public boolean confirmation(String user) throws RemoteException {

        synchronized (freeSeats){
            Set<Integer> tmp = reservations.get(user);
        for (int seat : tmp ) {
            if(takenSeats.containsKey(seat)){
                for (int seat1 : tmp){
                    if(reservedSeats.get(seat1)!= null)
                        if(reservedSeats.get(seat1).equals(user)) {
                            reservedSeats.remove(seat1);
                            freeSeats.add(seat1);
                    }
                }
                return false;
            }

            if(reservedSeats.containsKey(seat))
                if(!reservedSeats.get(seat).equals(user)){
                    if(System.currentTimeMillis() - reservationTime.get(reservedSeats.get(seat)) < time) {
                        for (int seat1 : tmp){
                            if(reservedSeats.get(seat1)!= null)
                                if(reservedSeats.get(seat1).equals(user)) {
                                    reservedSeats.remove(seat1);
                                    freeSeats.add(seat1);
                                }
                        }
                        return false;
                    }
                }
        }
            for (int seat : tmp){
                if (freeSeats.contains(seat)){
                    freeSeats.remove(seat);
                    takenSeats.put(seat,user);
                }
                else{
                    reservedSeats.remove(seat);
                    takenSeats.put(seat, user);
                }

            }
        }

        return true;
    }

    @Override
    public String whoHasReservation(int seat) throws RemoteException {
       synchronized (freeSeats) {
           if (takenSeats.containsKey(seat))
               return takenSeats.get(seat);
           return null;
       }
   }
}
