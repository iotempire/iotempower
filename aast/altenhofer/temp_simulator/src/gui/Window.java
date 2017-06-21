package gui;

import org.fusesource.mqtt.client.BlockingConnection;
import org.fusesource.mqtt.client.MQTT;
import org.fusesource.mqtt.client.QoS;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.URISyntaxException;

/**
 * Created by christian on 14.06.2017.
 */
public class Window extends JFrame implements ChangeListener, ActionListener {

    static final int TEMP_MIN = 10;
    static final int TEMP_MAX = 35;
    static final int TEMP_INIT = 20;    //initial frames per second

    private String title;
    private int currentTemp;
    private JButton startBut;
    private JButton closeBut;

    private JSlider tempSlider;

    private MQTT client;
    private BlockingConnection connection;

    private boolean running = false;

    public Window(String title) throws HeadlessException {
        this.title = title;
    }

    public void init() {

        this.setTitle(title);

        this.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        this.getContentPane().add(new JLabel(title), BorderLayout.CENTER);

        JLabel tempLabel = new JLabel("Temperature");
        JPanel buttonPan = new JPanel();
        buttonPan.setLayout(new GridLayout(1, 2));

        startBut = new JButton("Start");
        closeBut = new JButton("Close");

        buttonPan.add(startBut);
        buttonPan.add(closeBut);

        startBut.addActionListener(this);
        closeBut.addActionListener(this);
        tempSlider = new JSlider(JSlider.HORIZONTAL, TEMP_MIN, TEMP_MAX, TEMP_INIT);
        tempSlider.addChangeListener(this);

        //Turn on labels at major tick marks.
        tempSlider.setMajorTickSpacing(10);
        tempSlider.setMinorTickSpacing(1);
        tempSlider.setPaintTicks(true);
        tempSlider.setPaintLabels(true);

        this.add(tempLabel, BorderLayout.PAGE_START);
        this.add(tempSlider, BorderLayout.CENTER);

        this.add(buttonPan, BorderLayout.PAGE_END);

        this.setSize(300, 200);
        this.setVisible(true);
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider) e.getSource();
        if (!source.getValueIsAdjusting()) {
            int temp = (int) source.getValue();
            currentTemp = temp;
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource().equals(startBut)) {
            currentTemp = tempSlider.getValue();
            initMqttConnection();
        } else {
            closeMqttConnection();
        }
    }

    private void initMqttConnection() {
        System.out.println("Start pressed");
        running = true;
        try {
            client = new MQTT();
            client.setHost("192.168.12.1", 1883);

            connection = client.blockingConnection();
            connection.connect();

            // connection.publish("nfc-id", String.valueOf(currentTemp).getBytes(), QoS.AT_LEAST_ONCE, false);
            // connection.publish("lock/relay/set", "on".getBytes(), QoS.AT_LEAST_ONCE, false);
            new TempThread().start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void closeMqttConnection() {
        System.out.println("Close pressed");

        try {
            if (connection.isConnected()) {
                connection.disconnect();
                running = false;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    class TempThread extends Thread {
        @Override
        public void run() {
            while (running) {
                if (connection.isConnected()) {
                    System.out.println("Temp: " + currentTemp);
                    try {
                        connection.publish("tempserver/temperature", String.valueOf(currentTemp).getBytes(), QoS.AT_LEAST_ONCE, false);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }
}
