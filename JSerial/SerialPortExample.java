public class SerialPortExample {

    public static void main(String[] args) {
        readData dataReader = new readData(115200, 4);
        //implement function that reads from first ' to second ' for id
        while (true) {
            if(dataReader.checkForData()) {
                System.out.println(dataReader.getData());
            }
        }
    }
}
