package gui;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.URISyntaxException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

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
    private JEditorPane ipText;

    private JSlider tempSlider;

    private MqttClient client;
    private String topic        = "tempchan/temperature";
    private int qos             = 2;
    private String broker;
    private String clientId     = "temp_java_simulator";
    private MemoryPersistence persistence = new MemoryPersistence();

    private boolean running = false;

    private Pattern pattern;
    private Matcher matcher;
    private static final String IPADDRESS_PATTERN =
            "^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
                    "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
                    "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
                    "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$";

    public Window(String title) throws HeadlessException {
        this.title = title;
    }

    public void init() {

        this.setTitle(title);

        this.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        this.getContentPane().add(new JLabel(title), BorderLayout.CENTER);

        JLabel tempLabel = new JLabel("Temperature");
        JPanel buttonPan = new JPanel();
        buttonPan.setLayout(new GridLayout(2, 2));
        ipText = new JEditorPane();

        startBut = new JButton("Start");
        closeBut = new JButton("Close");

        buttonPan.add(new Label("Enter IP: "));
        buttonPan.add(ipText);
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

        pattern = Pattern.compile(IPADDRESS_PATTERN);

    }

    public boolean validate(final String ip){
        matcher = pattern.matcher(ip);
        return matcher.matches();
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
        if(!validate(ipText.getText()))  {
            System.out.println(ipText.getText() + " is not an ip address");
            return;
        }
        broker = "tcp://" + ipText.getText() + ":1883"; // tcp: is necessary
        System.out.println("Start connection to: " + broker);
        running = true;
        try {
            client = new MqttClient(broker, clientId, persistence);
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(true);
            client.connect(connOpts);
            // connection.publish("nfc-id", String.valueOf(currentTemp).getBytes(), QoS.AT_LEAST_ONCE, false);
            // connection.publish("lock/relay/set", "on".getBytes(), QoS.AT_LEAST_ONCE, false);
            new TempThread().start();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    private void closeMqttConnection() {
        System.out.println("Close pressed");

        try {
            if (client.isConnected()) {
                client.disconnect();
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
                if (client.isConnected()) {
                    System.out.println("Temp: " + currentTemp);
                    try {
                        MqttMessage message = new MqttMessage(String.valueOf(currentTemp).getBytes());
                        message.setQos(qos);
                        client.publish(topic, message);
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
