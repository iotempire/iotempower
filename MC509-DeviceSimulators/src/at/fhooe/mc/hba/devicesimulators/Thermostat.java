package at.fhooe.mc.hba.devicesimulators;

import java.awt.Component;
import java.awt.Font;
import java.awt.GridLayout;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class Thermostat implements MqttCallback {

	private static Thermostat instance;
	private JLabel lblCurrentTempValue = new JLabel("");
	private JLabel lblDesiredTempValue = new JLabel("");
	private JLabel lblCooling = new JLabel("cooling: off");
	private JLabel lblHeating = new JLabel("heating: off");

	// times 100 gives us 2 decimal places without the problems double brings with it
	private int currentTempTimes100 = 1800;
	private int desiredTempTimes100 = 1800;

	public static void main(String[] args) {
		if (instance == null) {
			instance = new Thermostat();
		}
	}
	
	private MqttClient client;

	public Thermostat() {
		JFrame frame = new JFrame("Thermostat");
		frame.setSize(300, 200);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		GridLayout experimentLayout = new GridLayout(2, 2);
		frame.getContentPane().setLayout(experimentLayout);

		JPanel panelCurrentTemperature = new JPanel();
		panelCurrentTemperature.setLayout(new BoxLayout(panelCurrentTemperature, BoxLayout.Y_AXIS));
		panelCurrentTemperature.add(new JLabel(" "));
		lblCurrentTempValue.setFont(new Font(lblCurrentTempValue.getFont().getName(), Font.PLAIN, 30));
		lblCurrentTempValue.setAlignmentX(Component.CENTER_ALIGNMENT);
		panelCurrentTemperature.add(lblCurrentTempValue);
		JLabel lblCurrentTempLabel = new JLabel("current temperature");
		lblCurrentTempLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
		panelCurrentTemperature.add(lblCurrentTempLabel);
		frame.getContentPane().add(panelCurrentTemperature);
		
		JPanel panelDesiredTemperature = new JPanel();
		panelDesiredTemperature.setLayout(new BoxLayout(panelDesiredTemperature, BoxLayout.Y_AXIS));
		panelDesiredTemperature.add(new JLabel(" "));
		lblDesiredTempValue.setFont(new Font(lblDesiredTempValue.getFont().getName(), Font.PLAIN, 30));
		lblDesiredTempValue.setAlignmentX(Component.CENTER_ALIGNMENT);
		panelDesiredTemperature.add(lblDesiredTempValue);
		JLabel lblDesiredTempLabel = new JLabel("desired temperature");
		lblDesiredTempLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
		panelDesiredTemperature.add(lblDesiredTempLabel);
		frame.getContentPane().add(panelDesiredTemperature);

		lblCooling.setHorizontalAlignment(JLabel.CENTER);
		frame.getContentPane().add(lblCooling);

		lblHeating.setHorizontalAlignment(JLabel.CENTER);
		frame.getContentPane().add(lblHeating);

		frame.setVisible(true);

		// start thermostat-service
		new Thread(new Runnable() {
			public void run() {
				try {
					while (true) {
						doThermostatStuff();
						Thread.sleep(100);
					}
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}).start();
		
		// setup mqtt
		try {
			client = new MqttClient("tcp://192.168.12.1:1883", "JavaSample", new MemoryPersistence());

			MqttConnectOptions connOpts = new MqttConnectOptions();
			connOpts.setCleanSession(true);
			client.connect(connOpts);
			client.setCallback(this);
			client.subscribe("livingroom/temperature/set");
		} catch (MqttException e) {
			e.printStackTrace();
		}

		// run seperate thread for publishing temperature
		new Thread(new Runnable() {
			public void run() {
				try {
					while (true) {
						try {
							client.publish("livingroom/temperature", new MqttMessage(String.valueOf(getCurrentTempRounded()).getBytes()));
						} catch (MqttException e) {
							e.printStackTrace();
						}
						Thread.sleep(10000);
					}
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}).start();
	}

	private void doThermostatStuff() {
		if (currentTempTimes100 > desiredTempTimes100) { // apply cooling
			currentTempTimes100 -= Math.min(5, currentTempTimes100 - desiredTempTimes100);
		} else if (currentTempTimes100 < desiredTempTimes100) { // apply heating
			currentTempTimes100 += Math.min(5, desiredTempTimes100 - currentTempTimes100);
		}
		
		updateGUI();
	}

	private void updateGUI() {
		lblCurrentTempValue.setText(getCurrentTempRounded() + "°");
		lblDesiredTempValue.setText(getDesiredTempRounded() + "°");

		boolean isCooling = currentTempTimes100 > desiredTempTimes100;
		boolean isHeating = currentTempTimes100 < desiredTempTimes100;

		lblCooling.setText("cooling: " + (isCooling ? "on" : "off"));
		lblHeating.setText("heating: " + (isHeating ? "on" : "off"));
	}

	private int getDesiredTempRounded() {
		return desiredTempTimes100 / 100 + (desiredTempTimes100 % 100 < 50 ? 0 : 1);
	}

	private int getCurrentTempRounded() {
		return currentTempTimes100 / 100 + (currentTempTimes100 % 100 < 50 ? 0 : 1);
	}

	@Override
	public void connectionLost(Throwable arg0) {
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken arg0) {
	}

	@Override
	public void messageArrived(String topic, MqttMessage msg) throws Exception {
		desiredTempTimes100 = Integer.parseInt(new String(msg.getPayload())) * 100;
	}
}
