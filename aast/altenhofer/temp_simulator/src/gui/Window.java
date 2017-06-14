package gui;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * Created by christian on 14.06.2017.
 */
public class Window extends JFrame implements ChangeListener, ActionListener {

    static final int TEMP_MIN = 10;
    static final int TEMP_MAX = 35;
    static final int TEMP_INIT = 20;    //initial frames per second

    private String title;

    public Window(String title) throws HeadlessException {
        this.title = title;
    }

    public void init() {

        this.setTitle(title);

        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.getContentPane().add(new JLabel(title), BorderLayout.CENTER);

        JLabel tempLable = new JLabel("Temperature");
        JButton startBut = new JButton("Start");
        startBut.addActionListener(this);
        JSlider tempSlider = new JSlider(JSlider.HORIZONTAL, TEMP_MIN, TEMP_MAX, TEMP_INIT);
        tempSlider.addChangeListener(this);

        //Turn on labels at major tick marks.
        tempSlider.setMajorTickSpacing(10);
        tempSlider.setMinorTickSpacing(1);
        tempSlider.setPaintTicks(true);
        tempSlider.setPaintLabels(true);

        this.add(tempLable, BorderLayout.PAGE_START);
        this.add(tempSlider, BorderLayout.CENTER);
        this.add(startBut, BorderLayout.PAGE_END);

        this.setSize(300, 200);
        this.setVisible(true);
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            int temp = (int)source.getValue();
            System.out.println("Temperature: " + temp);
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        initMqttConnection();
    }

    private void initMqttConnection() {
        System.out.println("Button pressed");
    }
}
