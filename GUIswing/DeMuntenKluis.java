import javax.swing.*;
import java.awt.*;

public class DeMuntenKluis extends JFrame {

    public DeMuntenKluis() {
        setTitle("De Munten Kluis");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1680, 1050);
        setLayout(null); // For absolute positioning, not recommended

        int panelWidth = 1200;
        int panelHeight = 600;
        int panelX = (getWidth() - panelWidth) / 2;
        int panelY = (getHeight() - panelHeight) / 2;

        // logo
        // ImageIcon logo = new ImageIcon("bank1.png");
        // setIcon(logo);

        // Block panel
        JPanel blockPanel = new JPanel();
        blockPanel.setBackground(new Color(77, 169, 134)); // Set background color
        blockPanel.setBounds(panelX, panelY, panelWidth, panelHeight); // Adjust position and size as needed
        add(blockPanel);

        // for (int i = 1; i <= 4; i++) {
        //     JPanel test = new JPanel();
        //     blockPanel.add(test);
        // }

        // Buttons
        JButton button1 = new JButton("Afbreken");
        button1.setBackground(new Color(13, 14, 14)); // Achtergrondkleur aanpassen
        button1.setForeground(new Color(77, 169, 134));
        button1.setPreferredSize(new Dimension(200, 100));
        button1.setFont(new Font("Arial", Font.BOLD, 16));
        button1.setBorderPainted(false); // Schakel de knoprand uit
        button1.setFocusPainted(false);
        button1.setContentAreaFilled(false);
        button1.setOpaque(true); // Zorg ervoor dat de achtergrondkleur wordt weergegeven
        //button1.setVerticalAlignment(JButton.BOTTOM);
        //button1.setHorizontalAlignment(JPanel.LEFT);
        blockPanel.add(button1);

        JButton button2 = new JButton("...");
        button2.setBackground(Color.RED); // Achtergrondkleur aanpassen
        button2.setForeground(new Color(77, 169, 134));
        button2.setPreferredSize(new Dimension(200, 100));
        button2.setFont(new Font("Arial", Font.BOLD, 16));
        button2.setBorderPainted(false); // Schakel de knoprand uit
        button2.setFocusPainted(false);
        button2.setContentAreaFilled(false);
        button2.setOpaque(true); // Zorg ervoor dat de achtergrondkleur wordt weergegeven
        blockPanel.add(button2);

        JButton button3 = new JButton("Snel pinnen (£70)");
        button3.setBackground(new Color(13, 14, 14)); // Achtergrondkleur aanpassen
        button3.setForeground(new Color(77, 169, 134));
        button3.setPreferredSize(new Dimension(200, 100));
        button3.setFont(new Font("Arial", Font.BOLD, 16));
        button3.setBorderPainted(false); // Schakel de knoprand uit
        button3.setFocusPainted(false);
        button3.setContentAreaFilled(false);
        button3.setOpaque(true); // Zorg ervoor dat de achtergrondkleur wordt weergegeven
        blockPanel.add(button3);

        JButton button4 = new JButton("Ander bedrag");
        button4.setBackground(new Color(13, 14, 14)); // Achtergrondkleur aanpassen
        button4.setForeground(new Color(77, 169, 134));
        button4.setPreferredSize(new Dimension(200, 100));
        button4.setFont(new Font("Arial", Font.BOLD, 16));
        button4.setBorderPainted(false); // Schakel de knoprand uit
        button4.setFocusPainted(false);
        button4.setContentAreaFilled(false);
        button4.setOpaque(true); // Zorg ervoor dat de achtergrondkleur wordt weergegeven
        blockPanel.add(button4);

        // // Create panels for each button
        // JPanel panel1 = new JPanel(new FlowLayout(FlowLayout.LEFT));
        // JPanel panel2 = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        // JPanel panel3 = new JPanel(new FlowLayout(FlowLayout.LEFT));
        // JPanel panel4 = new JPanel(new FlowLayout(FlowLayout.RIGHT));

        // // Add buttons to panels
        // panel1.add(button1);
        // panel2.add(button2);
        // panel3.add(button3);
        // panel4.add(button4);

        // // Add panels to main panel
        // blockPanel.add(panel1);
        // blockPanel.add(panel2);
        // blockPanel.add(panel3);
        // blockPanel.add(panel4);


        // Text labels
        JLabel paragraphLabel = new JLabel("hoi");
        paragraphLabel.setForeground(new Color(77, 169, 134));
        paragraphLabel.setHorizontalAlignment(SwingConstants.CENTER);
        paragraphLabel.setFont(new Font("Arial", Font.PLAIN, 50));
        paragraphLabel.setBounds(0, 300, 600, 50); // Adjust position and size as needed
        blockPanel.add(paragraphLabel);

        getContentPane().setBackground(new Color(13, 14, 14)); // Set background color

        setVisible(true);
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(DeMuntenKluis::new);
    }
}

