import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.concurrent.TimeUnit;

public class ActionHandler {
    public static String amountInCents = "0";; // Amount the user wants to pin (in cents) for api
    public static String amountInPoundsText = "0"; // field for gui to display pinAmt
    public static int amountInPounds; // Amount the user wants to pin in Pounds

    private static String pinCodeNoob = "";; // field to store pincode for withdraw endpoint NOOB
    private static String pinCode = "";; // field to store pincode to be sent to server-database
    private static String iban = "";; // field to store iban to be sent to server-database
    private static String uid = "";; // field to store uid to be sent to server-database

    private static boolean loggedIn = false; // boolean value to keep track of user login

    private static boolean secondConfirm = false; // boolean that checks if user agrees with the selected billtypes

    private static String currentPage = "indexPage"; // field that keeps track of the current GUI-page

    private static int fivePoundBills = 0; // variable for five pound bills
    private static int tenPoundBills = 0; // variable for ten pound bills
    private static int twentyPoundBills = 0; // variable for twenty pound bills
    private static int fiftyPoundBills = 0; // variable for fifty pound bills

    private static int sumBills = 0; // variable to keep track of sum bill values

    private static int[] atmBillData = {0, 0, 0, 0}; // array to keep track of bills currently stored in the atm

    // function that returns true if char is a number
    private static boolean isNumChar(char num) {
        return num >= '0' && num <= '9';
    }

    // function that handles keypad presses
    private static void keypadHandler(char action, GUI gui, SerialData port) {
        if (!loggedIn) {
            if (isNumChar(action)) {
                if (pinCode.length() < 4) {
                    pinCode += action;
                    gui.editPincodePage(pinCode.length(), false, false);
                }
            } else {
                switch (action) {
                    case 'A': // Abort
                        pinCode = "";
                        // swingLoginScreen
                        break;
                    case 'B': // Clear
                        pinCode = "";
                        gui.editPincodePage(0, false, false);
                        break;
                    case 'D':
                        if (pinCode.length() == 4) {
                            String errorCode;

                            if (iban.substring(4, 8).equals("DMKB")) {
                                errorCode = ApiDataHandler.sendLoginData(pinCode, iban, uid);
                            } else {
                                errorCode = ApiDataHandler.sendAccountInfoNoob(pinCode, iban, uid);
                            }

                            gui.pincodePage(ApiDataHandler.attemptsRemaining);

                            if (errorCode.equals("200")) {
                                loggedIn = true;
                                // success
                            } else if (errorCode.equals("403")) {
                                gui.editPincodePage(0, false, true);
                                pinCode = "";

                                try {
                                    TimeUnit.SECONDS.sleep(1);
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }

                                gui.endPage();

                                try {
                                    TimeUnit.SECONDS.sleep(3);
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }

                                gui.indexPage();

                                port.sendData("C");
                                currentPage = "indexPage";


                                //blocked
                            } else if (errorCode.equals("401")) {
                                gui.editPincodePage(0, true, false);
                                pinCode = "";
                                //incorrect pincode
                            } else {
                                pinCode = "";
                            }

                            if (loggedIn) {
                                gui.homePage(ApiDataHandler.fullName, ApiDataHandler.balanceInCents);
                                currentPage = "homePage";
                                pinCodeNoob = pinCode;
                                pinCode = "";
                            }
                        }
                        break;
                }
            }
        } else {
            if (currentPage.equals("pinnenPage")) {
                if (isNumChar(action)) {
                    if (amountInPoundsText.length() < 4) {
                        amountInPoundsText += action;
                    }
                } else {
                    if (action == 'B') {
                        amountReset("amountToPin", gui);
                    }
                }
                gui.pinnenPage(Integer.parseInt(amountInPoundsText));
            }

            if (currentPage.equals("billChoicePage1") || currentPage.equals("billChoicePage2")) {
                int restAmount;
                if (action == 'D') {
                    if (sumBills == amountInPounds || secondConfirm) {
                        gui.bonPage();
                        currentPage = "bonPage";
                        secondConfirm = false;
                    } else {
                        int remaining = amountInPounds - sumBills;

                        amountReset("guiBills", gui);

                        gui.calculateBills(true, remaining);

                        fivePoundBills += gui.fiveBills;
                        tenPoundBills += gui.tenBills;
                        twentyPoundBills += gui.twentyBills;
                        fiftyPoundBills += gui.fiftyBills;

                        gui.fiveBills = fivePoundBills;
                        gui.tenBills = tenPoundBills;
                        gui.twentyBills = twentyPoundBills;
                        gui.fiftyBills = fiftyPoundBills;

                        restAmount = 0;
                        gui.remainingAmt = restAmount;

                        if (currentPage.equals("billChoicePage1")) {
                            gui.billChoicePage1(true);
                        } else {
                            gui.billChoicePage2(true);
                        }
                        secondConfirm = true;
                    }
                } else if (action == 'B') {
                    amountReset("poundBills", gui);

                    gui.fiveBills = fivePoundBills;
                    gui.tenBills = tenPoundBills;
                    gui.twentyBills = twentyPoundBills;
                    gui.fiftyBills = fiftyPoundBills;

                    sumBills = 0;
                    restAmount = amountInPounds;
                    gui.remainingAmt = restAmount;

                    if (currentPage.equals("billChoicePage1")) {
                        gui.billChoicePage1(false);
                    } else {
                        gui.billChoicePage2(false);
                    }

                    secondConfirm = false;
                }
            }
            //keypadinvoer
        }
    }

    // function that handles button presses / navigates pages
    public static void menuNavigator(char action, GUI gui, SerialData port) {
        if (action == '3') {
            currentPage = "endPage";
            gui.endPage();
            try {
                TimeUnit.SECONDS.sleep(2);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            currentPage = "indexPage";
            gui.indexPage();
            loggedIn = false;
            amountReset("amountToPin", gui);
            amountReset("poundBills", gui);
            gui.fiveBills = fivePoundBills;
            gui.tenBills = tenPoundBills;
            gui.twentyBills = twentyPoundBills;
            gui.fiftyBills = fiftyPoundBills;
            port.sendData("C");
            // afbreken
            return;
        }

        switch (currentPage) {
            case "homePage":
                switch (action) {
                    case '2': // quick pin
                        amountInCents = "7000";
                        fiftyPoundBills = 1;
                        twentyPoundBills = 1;
                        gui.bonPage();
                        currentPage = "bonPage";
                        break;
                    case '4':
                        gui.pinnenPage(0);
                        currentPage = "pinnenPage";
                        break;
                    default:
                        // button 1
                        break;
                }
                break;
            case "pinnenPage":
                if (action == '4') {
                    amountInPounds = Integer.parseInt(amountInPoundsText);
                    int pinAmt = amountInPounds;

                    amountInPounds -= pinAmt % 5; // naar beneden afronden

                    if (pinAmt - amountInPounds > 2) { // naar boven afronden
                        amountInPounds += 5;
                    }

                    if (amountInPounds != 0) {
                        gui.roundedPinAmt = amountInPounds;

                        amountInCents = Integer.toString(amountInPounds * 100);

                        String statusCode = ApiDataHandler.withdrawValidity(amountInCents);
                        if (statusCode.equals("412") || statusCode.equals("429")) {
                            gui.sendStatusCode(statusCode);
                        } else {
                            currentPage = "billChoiceOptionPage";
                            amountReset("guiBills", gui);
                            gui.billChoiceOptionPage();
                        }
                    }
                }
                // else button 1 & 2
                break;
            case "billChoiceOptionPage":
                switch (action) {
                    case '2':
                        currentPage = "billChoicePage1";

                        amountReset("poundBills", gui);

                        gui.fiveBills = fivePoundBills;
                        gui.tenBills = tenPoundBills;
                        gui.twentyBills = twentyPoundBills;
                        gui.fiftyBills = fiftyPoundBills;

                        gui.remainingAmt = amountInPounds;

                        gui.billChoicePage1(false);
                        break;
                    case '4':
                        currentPage = "bonPage";
                        fivePoundBills = gui.fiveBills;
                        tenPoundBills = gui.tenBills;
                        twentyPoundBills = gui.twentyBills;
                        fiftyPoundBills = gui.fiftyBills;
                        gui.bonPage();
                        break;
                    default:
                        // button 1
                        break;
                }
                break;
            case "bonPage":
                String statusCode;
                if (iban.substring(4, 8).equals("DMKB")) {
                    statusCode = ApiDataHandler.sendWithdrawData(amountInCents);
                    ApiDataHandler.sendTransactionBills(fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills);
                } else {
                    statusCode = ApiDataHandler.sendWithdrawDataNoob(pinCodeNoob, iban, uid, amountInCents);
                    ApiDataHandler.sendTransactionBills(fivePoundBills, tenPoundBills, twentyPoundBills, fiftyPoundBills);
                    pinCodeNoob = "";
                }

                gui.sendStatusCode(statusCode);
                currentPage = "endPage";
                gui.endPage();

                try {
                    TimeUnit.SECONDS.sleep(2);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                currentPage = "indexPage";
                gui.indexPage();

                loggedIn = false;
                amountReset("amountToPin", gui);

                switch (action) {
                    case '4':
                        String bonDateData;
                        LocalDateTime myDateObj = LocalDateTime.now();
                        DateTimeFormatter myFormatObj = DateTimeFormatter.ofPattern("dd/MM/yyyy HH:mm:ss");
                        bonDateData = "D" + myDateObj.format(myFormatObj) + iban.substring(iban.length() - 4) + ApiDataHandler.transactionId;

                        port.sendData(bonDateData);

                        // Adding a wait time
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            Thread.currentThread().interrupt();
                        }

                        String bonGeldData = "G";

                        bonGeldData += (char) (65 + fivePoundBills);
                        bonGeldData += (char) (65 + tenPoundBills);
                        bonGeldData += (char) (65 + twentyPoundBills);
                        bonGeldData += (char) (65 + fiftyPoundBills);

                        // writeData
                        port.sendData(bonGeldData);
                        System.out.println(bonGeldData);

                        break;
                    case '2':
                        String bonGeldData2 = "G";

                        bonGeldData2 += (char) (65 + fivePoundBills);
                        bonGeldData2 += (char) (65 + tenPoundBills);
                        bonGeldData2 += (char) (65 + twentyPoundBills);
                        bonGeldData2 += (char) (65 + fiftyPoundBills);

                        // writeData
                        port.sendData(bonGeldData2);
                        System.out.println(bonGeldData2);
                        break;
                    default:
                        // button 1
                        break;
                }

                amountReset("poundBills", gui);

                // Adding a wait time
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }

                port.sendData("C");
                break;
            case "billChoicePage1":
                switch (action) {
                    case '1':
                        if (sumBills + 5 <= amountInPounds) {
                            fivePoundBills++;
                            gui.fiveBills = fivePoundBills;
                        }

                        sumBills = fivePoundBills * 5 + tenPoundBills * 10 + twentyPoundBills * 20 + fiftyPoundBills * 50;
                        gui.remainingAmt = amountInPounds - sumBills;
                        gui.billChoicePage1(false);
                        break;
                    case '2':
                        if (sumBills + 10 <= amountInPounds) {
                            tenPoundBills++;
                            gui.tenBills = tenPoundBills;
                        }

                        sumBills = fivePoundBills * 5 + tenPoundBills * 10 + twentyPoundBills * 20 + fiftyPoundBills * 50;
                        gui.remainingAmt = amountInPounds - sumBills;
                        gui.billChoicePage1(false);
                        break;
                    case '4':
                        currentPage = "billChoicePage2";
                        gui.billChoicePage2(false);
                        break;
                }
                break;
            case "billChoicePage2":
                switch (action) {
                    case '1':
                        if (sumBills + 20 <= amountInPounds) {
                            twentyPoundBills++;
                            gui.twentyBills = twentyPoundBills;
                        }

                        sumBills = fivePoundBills * 5 + tenPoundBills * 10 + twentyPoundBills * 20 + fiftyPoundBills * 50;
                        gui.remainingAmt = amountInPounds - sumBills;
                        gui.billChoicePage2(false);
                        break;
                    case '2':
                        if (sumBills + 50 <= amountInPounds) {
                            fiftyPoundBills++;
                            gui.fiftyBills = fiftyPoundBills;
                        }

                        sumBills = fivePoundBills * 5 + tenPoundBills * 10 + twentyPoundBills * 20 + fiftyPoundBills * 50;
                        gui.remainingAmt = amountInPounds - sumBills;
                        gui.billChoicePage2(false);
                        break;
                    case '4':
                        currentPage = "billChoicePage1";
                        gui.billChoicePage1(false);
                        break;
                }
                sumBills = fivePoundBills * 5 + tenPoundBills * 10 + twentyPoundBills * 20 + fiftyPoundBills * 50;

                if (sumBills < amountInPounds) {
                    gui.remainingAmt = amountInPounds - sumBills;
                } else {
                    // incorrect amount
                }
                break;
            default:
                break;
        }
        // updateScreen(String currentPage);
    }

    public static void actionMenu(String data, GUI gui, SerialData port) {
        char action = data.charAt(0);
        switch (action) {
            case '1':
                // button1
            case '2':
                // button2
            case '3':
                // button3
            case '4':
                // button4
                if (loggedIn) {
                    menuNavigator(action, gui, port);
                }
                break;
            case '5':
                // keypad switch
                keypadHandler(data.charAt(1), gui, port);
                break;
            case '7':
                // iban + uid
                if (!loggedIn) {
                    iban = data.substring(1, 19);
                    uid = data.substring(19);
                    gui.pincodePage("");
                    currentPage = "pincodePage";
                }
                break;
            case '8':
                if (loggedIn) {
                    gui.endPage();
                    currentPage = "endPage";
                    try {
                        Thread.sleep(2000);
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                    gui.indexPage();
                    currentPage = "indexPage";
                    loggedIn = false;
                    amountReset("amountToPin", gui);
                    amountReset("poundBills", gui);
                    gui.fiveBills = fivePoundBills;
                    gui.tenBills = tenPoundBills;
                    gui.twentyBills = twentyPoundBills;
                    gui.fiftyBills = fiftyPoundBills;
                    port.sendData("C");
                }
                break;
            case '9':
                // billUpdate
                updateBills(data.substring(1));
                break;
            default:
                System.out.println("Invalid action = " + action);
        }
    }
    // 912

    private static void updateBills(String data) {
        int[] amount = {5, 10, 20, 50};
        int index = data.charAt(0) - '0';

        if (index != 9) {
            billIncrementAmount(data.charAt(1) - '0', amount[index]);
        } else {
            ApiDataHandler.sendBillAmountData(Integer.toString(atmBillData[0]), Integer.toString(atmBillData[1]), Integer.toString(atmBillData[2]), Integer.toString(atmBillData[3]), true);
            for (int i = 0; i < 4; i++) {
                atmBillData[i] = 0;
            }
        }
    }

    private static void billIncrementAmount(int option, int amount) {
        int[] incrementAmount = {1, 10, -1};

        switch (amount) {
            case 5:
                atmBillData[0] += incrementAmount[option];
                break;
            case 10:
                atmBillData[1] += incrementAmount[option];
                break;
            case 20:
                atmBillData[2] += incrementAmount[option];
                break;
            case 50:
                atmBillData[3] += incrementAmount[option];
                break;
        }
    }

    public static void amountReset(String type, GUI gui) {
        switch (type) {
            case "amountToPin":
                amountInCents = "0";
                amountInPoundsText = "0";
                amountInPounds = 0;
                break;
            case "poundBills":
                fivePoundBills = 0;
                tenPoundBills = 0;
                twentyPoundBills = 0;
                fiftyPoundBills = 0;
                break;
            case "guiBills":
                gui.fiveBills = 0;
                gui.tenBills = 0;
                gui.twentyBills = 0;
                gui.fiftyBills = 0;
                break;
        }
    }
}