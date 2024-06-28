import com.fazecast.jSerialComm.SerialPort;
import java.nio.charset.StandardCharsets;

public class SerialData {
    private SerialPort port;
    private byte[] buffer;
    public SerialData(int baudRate, int portNum) {
        initialize(baudRate, portNum);
    }

    private void initialize(int baudRate, int portNum) {
        port = SerialPort.getCommPort("COM" + portNum);

        port.setBaudRate(baudRate);
        port.setNumDataBits(8);
        port.setNumStopBits(1);
        port.setParity(SerialPort.NO_PARITY);

        port.openPort();

        // line that blocks data is necessary (if delay == timeout, not good)
        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 100, 100);
        buffer = new byte[128];
    }

    public boolean checkForData() {
        return port.bytesAvailable() > 0;
    }

    public String getData() {
        // Read the data into the buffer
        int bytesRead = port.readBytes(buffer, buffer.length);

        // Convert the byte array to a string
        return new String(buffer, 0, bytesRead);
    }

    public void sendData(String data) {
        // System.out.println(Arrays.toString(buffer));
        port.writeBytes(data.getBytes(StandardCharsets.UTF_8), data.length());
    }
}