import javax.swing.*;
import java.awt.*;

public class GUI {
    JFrame frame = new JFrame();

    private JPanel indexPanel;
    private JPanel pincodePanel;
    private JPanel homePanel;
    private JPanel pinPanel;
    private JPanel billChoiceOptionPanel;
    private JPanel billChoicePanel1;
    private JPanel billChoicePanel2;
    private JPanel bonPanel;
    private JPanel endPanel;

    private JLabel pincodeLabel;
    private JLabel pressDLabel;
    private JLabel pincodeIncorrectLabel;
    private JLabel pincodeBlockedLabel;
    private JLabel bedragLabel;
    private JLabel roundedAmtLabel;
    private JLabel restAmtLabel;
    private JLabel fullName;
    private JLabel balanceInCents;

    private JLabel notEnoughBalanceLabel;
    private JLabel exceedingDailyLimitLabel;

    private final int width = 1375;
    private final int height = 920;

    public int fiftyBills = 0;
    public int twentyBills = 0;
    public int tenBills = 0;
    public int fiveBills = 0;

    public int roundedPinAmt;
    public int remainingAmt;


    private JPanel createPanel(String imgPath) {
        Image indexImage = new ImageIcon(imgPath).getImage();
        // Maak een aangepast JPanel voor de achtergrondafbeelding
        JPanel imagePanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                // Schilder de afbeelding over het hele paneel
                g.drawImage(indexImage, 0, 0, getWidth(), getHeight(), this);
            }
        };

        // Stel de layout van het imagePanel in
        imagePanel.setLayout(new BorderLayout());

        // Maak een JLayeredPane om lagen te beheren
        JLayeredPane layeredPane = new JLayeredPane();
        layeredPane.setPreferredSize(new Dimension(width, height));

        // Voeg de imagePanel toe aan de onderste laag
        imagePanel.setBounds(0, 0, width, height);
        layeredPane.add(imagePanel, JLayeredPane.DEFAULT_LAYER);

        // Maak een overlayPanel en stel de layout in als null layout voor absolute positioning
        JPanel overlayPanel = new JPanel();
        overlayPanel.setOpaque(false);  // Zorg ervoor dat het paneel doorzichtig is
        overlayPanel.setBounds(0, 0, width, height);
        overlayPanel.setLayout(null);

        // Voeg de overlayPanel toe aan een hogere laag
        layeredPane.add(overlayPanel, JLayeredPane.PALETTE_LAYER);

        // Voeg de layeredPane toe aan het frame
        frame.setContentPane(layeredPane);

        return overlayPanel;
    }

    private JLabel createLabel(String text, int fontSize, Color color, int x, int y, int width, int height) {
        JLabel label = new JLabel(text);
        label.setFont(new Font("Vertica", Font.BOLD, fontSize));
        label.setForeground(color);
        label.setBounds(x, y, width, height);
        return label;
    }

    public GUI() {
        endPanel = new JPanel();
        bonPanel = new JPanel();
        billChoiceOptionPanel = new JPanel();
        billChoicePanel1 = new JPanel();
        billChoicePanel2 = new JPanel();
        pincodePanel = new JPanel();
        pinPanel = new JPanel();

        // Stel de frame eigenschappen in
        frame.setTitle("De Munten Kluis");
        frame.setSize(width, height);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLocationRelativeTo(null);
        frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
        frame.setUndecorated(true);
        indexPage();
    }

    public void indexPage() {
        endPanel.setVisible(false);
        indexPanel = createPanel("img/index.png");
        indexPanel.setVisible(true);
        frame.setVisible(true);
    }

    public void pincodePage(String attemptsRemaining) {
        indexPanel.setVisible(false);
        pincodePanel = createPanel("img/pincodepage.png");

        pincodeLabel = createLabel("", 65, Color.BLACK, (width - 100) / 2, 500, 200, 100);
//        pincodeLabel = new JLabel();
//        pincodeLabel.setFont(new Font("Vertica", Font.BOLD, 65));
//        pincodeLabel.setBounds((width - 100) / 2, 500, 200, 100); // Zet het label in het midden bovenaan

        pressDLabel = createLabel("Druk op D voor doorgaan", 25, new Color(0, 0, 128), (width - 295) / 2, 400, 500, 100);
//        pressDLabel = new JLabel("Press D to confirm");
//        pressDLabel.setFont(new Font("Vertica", Font.BOLD, 25));
//        pressDLabel.setForeground(new Color(0,0,128));
//        pressDLabel.setBounds((width - 245) / 2, 400, 500, 100);

        pincodeIncorrectLabel = createLabel("Incorrecte pincoode, pogingen over: " + attemptsRemaining, 20, Color.RED, (width - 333) / 2, 532, 500, 100);
//        pincodeIncorrectLabel = new JLabel("Incorrect pincode, attempts left: " + attemptsRemaining); //ApiDataHandler.attemptsRemaining
//        pincodeIncorrectLabel.setFont(new Font("Vertica", Font.BOLD, 20));
//        pincodeIncorrectLabel.setForeground(Color.RED);
//        pincodeIncorrectLabel.setBounds((width - 333) / 2, 532, 500, 100); // Zet het label in het midden bovenaan

        pincodeBlockedLabel = createLabel("Kaart is geblokkeerd", 20, Color.RED, (width - 170) / 2, 532, 200, 100);
//        pincodeBlockedLabel = new JLabel("Card is blocked");
//        pincodeBlockedLabel.setFont(new Font("Vertica", Font.BOLD, 20));
//        pincodeBlockedLabel.setForeground(Color.RED);
//        pincodeBlockedLabel.setBounds((width - 162) / 2, 532, 200, 100); // Zet het label in het midden bovenaan


        pincodePanel.setVisible(true);
        pincodeLabel.setVisible(true);
        pressDLabel.setVisible(true);

        pincodeIncorrectLabel.setVisible(false);
        pincodeBlockedLabel.setVisible(false);
        pincodePanel.add(pincodeLabel);
        pincodePanel.add(pressDLabel);
        pincodePanel.add(pincodeIncorrectLabel);
        pincodePanel.add(pincodeBlockedLabel);
        frame.setVisible(true);
    }

    public void editPincodePage(int i, boolean incorrect, boolean blocked) {
        pincodeLabel.setText("*".repeat(Math.max(0, i)));
        if (i != 0) {
            pincodeLabel.setVisible(true);
        } else {
            pincodeLabel.setVisible(false);
        }
        if (incorrect) {
            pincodeIncorrectLabel.setVisible(true);
            pincodeBlockedLabel.setVisible(false);
        } else if (blocked) {
            pincodeIncorrectLabel.setVisible(false);
            pincodeBlockedLabel.setVisible(true);
        }
    }

    public void homePage(String fullNameText, String balanceInCentsText) {

        pincodePanel.setVisible(false);
        pincodeLabel.setVisible(false);
        pincodeIncorrectLabel.setVisible(false);
        homePanel = createPanel("img/keuzemenu.png");

        // Voeg het label toe aan de overlayPanel
        fullName = createLabel("", 25, new Color(77, 169, 134), (width - 125) / 2, 50, 350, 100);
//        fullName = new JLabel();
//        fullName.setFont(new Font("Vertica", Font.BOLD, 25));
//        fullName.setBounds((width - 125) / 2, 50, 350, 100); // Zet het label in het midden bovenaan
//        fullName.setForeground(new Color(77, 169, 134));
        homePanel.add(fullName);

        balanceInCents = createLabel("", 25, new Color(77, 169, 134), (width - 125) / 2, 100, 350, 100); // Zet het label in het midden bovenaan
        homePanel.add(balanceInCents);

        fullName.setText("Name: " + fullNameText);

        if (balanceInCentsText.length() > 2) {
            balanceInCents.setText("Balance: £" + balanceInCentsText.substring(0, balanceInCentsText.length() - 2) + "," + balanceInCentsText.substring(balanceInCentsText.length() - 2));
        } else if (balanceInCentsText.length() == 1) {
            balanceInCents.setText("Balance: £" + "0,0" + balanceInCentsText);
        } else {
            balanceInCents.setText("Balance: £" + "0," + balanceInCentsText);
        }

        fullName.setVisible(true);
        balanceInCents.setVisible(true);

//        dateTime = new JLabel();
//        dateTime.setFont(new Font("Vertica", Font.BOLD, 65));
//        dateTime.setBounds((width - 100) / 2, 500, 200, 100); // Zet het label in het midden bovenaan
//        homePanel.add(dateTime);
//        dateTime.setVisible(true);

        homePanel.setVisible(true);
        frame.setVisible(true);
    }

    public void bonPage() {
        homePanel.setVisible(false);
        billChoiceOptionPanel.setVisible(false);
        pinPanel.setVisible(false);

        bonPanel = createPanel("img/bon.png");
        bonPanel.setVisible(true);
        frame.setVisible(true);
    }

    public void pinnenPage(int amount) {
        roundedPinAmt = amount;

        pincodePanel.setVisible(false);
        pinPanel = createPanel("img/bedrag.png");
        pinPanel.add(balanceInCents);
        pinPanel.add(fullName);

        bedragLabel = createLabel("", 28, Color.BLACK, (width - 120) / 2, 524, 200, 100);
//        bedragLabel = new JLabel();
//        bedragLabel.setFont(new Font("Vertica", Font.BOLD, 28));
//        bedragLabel.setBounds((width - 120) / 2, 524, 200, 100); // Zet het label in het midden bovenaan

        notEnoughBalanceLabel = createLabel("Te weinig saldo", 20, Color.RED, (width - 220) / 2, 580, 200, 100);

        exceedingDailyLimitLabel = createLabel("Daglimiet overschreden", 20, Color.RED, (width - 280) / 2, 580, 500, 100);

        bedragLabel.setText("£" + amount);
        pinPanel.add(bedragLabel);
        bedragLabel.setVisible(true);
        notEnoughBalanceLabel.setVisible(false);
        exceedingDailyLimitLabel.setVisible(false);
        pinPanel.add(notEnoughBalanceLabel);
        pinPanel.add(exceedingDailyLimitLabel);

        pinPanel.setVisible(true);
        frame.setVisible(true);
    }

    public void calculateBills(boolean isRemaining, int amount) {
        int pinAmt;

        if (isRemaining) {
            pinAmt = amount;
        } else {
            pinAmt = roundedPinAmt;
        }
        int rest;

        if(pinAmt % 50 != pinAmt) {
            if(pinAmt % 50 == 0){
                fiftyBills = pinAmt / 50;
                pinAmt = 0;
            } else {
                rest = pinAmt % 50;
                pinAmt -= rest;
                fiftyBills = pinAmt / 50;
                pinAmt = rest;
            }
        }

        if(pinAmt % 20 != pinAmt) {
            if(pinAmt % 20 == 0){
                twentyBills = pinAmt / 20;
                pinAmt = 0;
            }
            else{
                rest = pinAmt % 20;
                pinAmt -= rest;
                twentyBills = pinAmt / 20;
                pinAmt = rest;
            }
        }


        if(pinAmt % 10 != pinAmt) {
            if(pinAmt % 10 == 0){
                tenBills = pinAmt / 10;
                pinAmt = 0;
            }
            else{
                rest = pinAmt % 10;
                pinAmt -= rest;
                tenBills = pinAmt / 10;
                pinAmt = rest;
            }
        }

        if(pinAmt % 5 != pinAmt) {
            fiveBills = pinAmt / 5;
        }

    }

    public void billChoiceOptionPage() {
        calculateBills(false, 0);
        homePanel.setVisible(false);
        billChoiceOptionPanel = createPanel("img/bedragkeuze.png");

        roundedAmtLabel = createLabel("", 28, Color.BLACK, (width - 200) / 2, 390, 300, 100); // Zet het label in het midden bovenaan
        roundedAmtLabel.setText("Totaal bedrag: £" + roundedPinAmt);

        billChoiceOptionPanel.add(roundedAmtLabel);

        setBillsLabels("billChoiceOptionPage");

        billChoiceOptionPanel.add(balanceInCents);
        billChoiceOptionPanel.add(fullName);

        billChoiceOptionPanel.setVisible(true);
        frame.setVisible(true);
    }

    public void billChoicePage1(boolean askCheck) {

        billChoiceOptionPanel.setVisible(false);
        billChoicePanel1 = createPanel("img/biljetkeuze1.png");

        restAmtLabel = createLabel("", 28, Color.BLACK, (width - 200) / 2, 420, 500, 100); // Zet het label in het midden bovenaan
        pressDLabel = createLabel("Druk op D voor doorgaan", 25, new Color(0, 0, 128), (width - 285) / 2, 350, 500, 100);

        if (!askCheck) {
            restAmtLabel.setText("overig bedrag: £" + remainingAmt);
        } else {
            restAmtLabel.setText("Druk op D voor doorgaan");
        }

        setBillsLabels("billChoicePage1");

        billChoicePanel1.add(roundedAmtLabel);
        billChoicePanel1.add(restAmtLabel);
        billChoicePanel1.add(pressDLabel);

        billChoicePanel1.add(balanceInCents);
        billChoicePanel1.add(fullName);

        billChoicePanel1.setVisible(true);
        frame.setVisible(true);
    }

    public void billChoicePage2(boolean askCheck) {
        billChoicePanel1.setVisible(false);
        billChoicePanel2 = createPanel("img/biljetkeuze2.png");

        if(!askCheck) {
            restAmtLabel.setText("overig bedrag: £" + remainingAmt);
        } else {
            restAmtLabel.setText("Druk op D voor doorgaan");
        }

        setBillsLabels("billChoicePage2");

        billChoicePanel2.add(roundedAmtLabel);
        billChoicePanel2.add(restAmtLabel);
        billChoicePanel2.add(pressDLabel);

        billChoicePanel2.add(balanceInCents);
        billChoicePanel2.add(fullName);

        billChoicePanel2.setVisible(true);
        frame.setVisible(true);
    }

    public void endPage() {
        bonPanel.setVisible(false);
        endPanel = createPanel("img/einde.png");
        endPanel.setVisible(true);
        frame.setVisible(true);
    }

    public void setBillsLabels(String pageName) {
        String[] billAmounts = {"£50", "£20", "£10", "£5"};
        int[] billCounts = {fiftyBills, twentyBills, tenBills, fiveBills};

        JLabel[] billLabels = new JLabel[billAmounts.length];
        for (int i = 0; i < billAmounts.length; i++) {
            billLabels[i] = createLabel("", 28, Color.BLACK, (width - 200) / 2, 450 + (30 * i), 300, 100);
            billLabels[i].setText(billCounts[i] + "x " + billAmounts[i]);
            switch (pageName) {
                case "billChoiceOptionPage":
                    billChoiceOptionPanel.add(billLabels[i]);
                    break;
                case "billChoicePage1":
                    billChoicePanel1.add(billLabels[i]);
                    break;
                case "billChoicePage2":
                    billChoicePanel2.add(billLabels[i]);
                    break;
            }
        }
    }

    public void sendStatusCode(String statusCode) {
        GUI gui = new GUI();
        if (statusCode.equals("412")) {
            ActionHandler.amountReset("amountToPin", gui);
            pinnenPage(0);
            notEnoughBalanceLabel.setVisible(true);
            // insufficient funds
        } else if (statusCode.equals("418")) {

            // Not enough bills in the atm
        } else if (statusCode.equals("429")) {
            ActionHandler.amountReset("amountToPin", gui);
            pinnenPage(0);
            exceedingDailyLimitLabel.setVisible(true);
            // Exceeding daily limit
//        } else if (statusCode.equals("500")) {
//
//            // Internal server error
        }
    }
}