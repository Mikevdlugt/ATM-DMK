import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

public class ApiDataHandler {
    private final static String serverUrl = "http://145.24.223.81";
    private final static String noobServerUrl = "https://noob.datalabrotterdam.nl";
    private final static String noobToken = "8a87b139-5068-4382-bd7d-4f12899389fc";
    private final static int port = 8500;
    private final static String loginEndpoint = "/login";
    private final static String withdrawEndpoint = "/withdraw";
    private final static String withdrawValidity = "/withdrawValidity";
    private final static String billAmountEndpoint = "/billAmount";
    private final static String transactionBillsEndpoint = "/transactionBills";
    private final static String accountInfoNoobEndpoint = "/api/noob/accountinfo";
    private final static String withdrawNoobEndpoint = "/api/noob/withdraw";


    public static String status = "";
    public static String balanceInCents = "";
    public static String fullName = "";
    public static String attemptsRemaining = "";
    public static String transactionId = "";


    private static String setUrl(String urlServer, String endpoint) {
        if(endpoint.startsWith("/api/noob")) {
            return urlServer + endpoint;
        } else {
            return urlServer + ':' + port + endpoint;
        }
    }

    private static String writeDataToOutput(String endpoint, String data) {
        return writeDataToOutput(serverUrl, endpoint, data, "");
    }

    private static String writeDataToOutput(String urlServer, String endpoint, String data, String target){
        try {
            // URL of the server where you want to send the data
            URL url = new URL(setUrl(urlServer, endpoint + target));

            // Open a connection to the URL
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();

            // Set request method to POST
            connection.setRequestMethod("POST");
            connection.setRequestProperty("NOOB-TOKEN", noobToken);

            if(urlServer.equals(noobServerUrl)) {
                connection.setRequestProperty("Content-Type", "application/json");
                connection.setRequestProperty("Origin", noobServerUrl);
            }

            // Enable output and input streams
            connection.setDoOutput(true);
            connection.setDoInput(true);

            // Write data to the connection output stream
            DataOutputStream outputStream = new DataOutputStream(connection.getOutputStream());
            outputStream.writeBytes(data);
            outputStream.flush();
            outputStream.close();

            // Get response from the server
            int responseCode = connection.getResponseCode();
            BufferedReader in;

            if (responseCode == 200) {
                // Use input stream for successful response
                in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
            } else {
                // Use error stream for error response
                in = new BufferedReader(new InputStreamReader(connection.getErrorStream()));
            }

            String inputLine;
            StringBuilder response = new StringBuilder();

            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
            in.close();

            // Close connection
            connection.disconnect();
            return parseData(response.toString());
        } catch (Exception e) {
            return "Error: " + e.getMessage();
        }
    }

    private static String parseData(String data) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        JsonNode jsonNode = mapper.readTree(data.getBytes());

        status = jsonNode.get("status").textValue();
        String data1 = jsonNode.get("data").textValue();

        if (status.equals("200")) {
            jsonNode = mapper.readTree(data1.getBytes());
            balanceInCents = jsonNode.get("balanceInCents").numberValue().toString();
            fullName = jsonNode.get("fullName").textValue();
            transactionId = jsonNode.get("maxTransactionId").numberValue().toString();
        } else if (status.equals("401")) {
            attemptsRemaining = String.valueOf(jsonNode.get("attempts_remaining").asInt());
//            System.out.println(attemptsRemaining);
        }

        return status;
    }

    public static String sendLoginData(String pinCode, String iban, String uid) {
        // Create data to send to the server
        String data = "pinCode=" + pinCode + "&iban=" + iban + "&uid=" + uid;
        return writeDataToOutput(loginEndpoint, data);
    }

    public static String withdrawValidity(String amount) {
        String data = "amount=" + amount;
        return writeDataToOutput(withdrawValidity, data);
    }

    public static String sendWithdrawData(String amount) {
        // Create data to send to the server
        String data = "amount=" + amount;
        return writeDataToOutput(withdrawEndpoint, data);
    }

    public static String sendBillAmountData(String fivePoundBills, String tenPoundBills, String twentyPoundBills, String fiftyPoundBills, boolean isAdmin) {
        // Create data to send to the server
        String data = "fivePoundBills=" + fivePoundBills + "&tenPoundBills=" + tenPoundBills + "&twentyPoundBills=" + twentyPoundBills + "&fiftyPoundBills=" + fiftyPoundBills + "&isAdmin=" + isAdmin;
        return writeDataToOutput(billAmountEndpoint, data);
    }

    public static String sendTransactionBills(int fivePoundBills, int tenPoundBills, int twentyPoundBills, int fiftyPoundBills) {
        // Create data to send to the server
        String data = "fivePoundBills=" + fivePoundBills + "&tenPoundBills=" + tenPoundBills + "&twentyPoundBills=" + twentyPoundBills + "&fiftyPoundBills=" + fiftyPoundBills;
        return writeDataToOutput(transactionBillsEndpoint, data);
    }

    public static String sendAccountInfoNoob(String pinCode, String iban, String uid) {
        // Create data to send to the server
//        String data = "pinCode=" + pinCode + "&uid=" + uid;
        String data = "pincode=" + pinCode + "&uid=" + uid;
//        System.out.println("iban: " + iban);
        System.out.println(noobServerUrl + "?target=" + iban);
        return writeDataToOutput(noobServerUrl, accountInfoNoobEndpoint, data, "?target=" + iban);
    }

    public static String sendWithdrawDataNoob(String pinCode, String iban, String uid, String amount) {
        // Create data to send to the server
//        String data = "pinCode=" + pinCode + "&uid=" + uid + "amount=" + amount;
        String data = "pincode=" + pinCode + "&uid=" + uid + "&amount=" + amount;
        return writeDataToOutput(noobServerUrl, withdrawNoobEndpoint, data, "?target=" + iban);
    }
}