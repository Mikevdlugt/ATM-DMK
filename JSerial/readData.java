package HobbyStuff.JSerial;

import com.fazecast.jSerialComm.SerialPort;
public class readData {
    private SerialPort port;
    private byte[] buffer;
    private String data;

    public readData(int baudRate, int portNum) {
        initialize(baudRate, portNum);
    }

    private void initialize(int baudRate, int portNum) {
        port = SerialPort.getCommPort("COM" + portNum);

        port.setBaudRate(baudRate);
        port.setNumDataBits(8);
        port.setNumStopBits(1);
        port.setParity(SerialPort.NO_PARITY);

        boolean isOpen = port.openPort();

        // line that blocks data is necessary
        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 100, 0);
        buffer = new byte[1024];
    }

    public boolean checkForData() {
        return port.bytesAvailable() > 0;
    }

    public String getData() {
        // Read the data into the buffer
        int bytesRead = port.readBytes(buffer, buffer.length);

        // Convert the byte array to a string
        data = new String(buffer, 0, bytesRead);
        return data;
    }
}
