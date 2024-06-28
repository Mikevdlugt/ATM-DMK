import java.time.Clock;

public class Main {
    public static void main(String[] args) {
        long currentTime = Clock.systemDefaultZone().millis();
        SerialData serialData = new SerialData(115200, 4);
        String data;
        GUI gui = new GUI();

        while (true) {
            if(serialData.checkForData()) {
                currentTime = Clock.systemDefaultZone().millis();
                data = serialData.getData();
                System.out.println("data: " + data);
                ActionHandler.actionMenu(data, gui, serialData);
            } else if (Clock.systemDefaultZone().millis() - currentTime > 15000) {
                ActionHandler.actionMenu("8", gui, serialData);
                currentTime = Clock.systemDefaultZone().millis();
            }
        }
    }
}
