package at.fhooe.mc.hba.devicesimulators;

import java.awt.BorderLayout;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class DoorLock implements MqttCallback {

	private static DoorLock instance;
	private static final String TARGET_NFC_CODE = "198,174,119,65";
	private static final int WAIT_SECONDS = 5;

	public static void main(String[] args) {

		if (instance == null) {
			instance = new DoorLock();
		}
	}

	private MqttClient client;
	private ImageIcon iconClosed, iconOpen;
	private JLabel imageLabel;

	public DoorLock() {
		JFrame frame = new JFrame("Lock");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		try {
			iconClosed = new ImageIcon(ImageIO.read(DoorLock.class.getResource("/lock_closed.png")));
			iconOpen = new ImageIcon(ImageIO.read(DoorLock.class.getResource("/lock_open.png")));
			imageLabel = new JLabel(iconClosed);
			frame.getContentPane().add(imageLabel, BorderLayout.CENTER);
			frame.pack();
			frame.setVisible(true);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			client = new MqttClient("tcp://192.168.12.1:1883", "JavaSample", new MemoryPersistence());

			MqttConnectOptions connOpts = new MqttConnectOptions();
			connOpts.setCleanSession(true);
			client.connect(connOpts);
			client.setCallback(this);
			client.subscribe("door/tagid");
		} catch (MqttException e) {
			e.printStackTrace();
		}

		// close mqtt-client on shutdown
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override
			public void run() {
				try {
					client.disconnect();
				} catch (MqttException e) {
					e.printStackTrace();
				}
			}
		});
	}

	@Override
	public void connectionLost(Throwable arg0) {
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken arg0) {
	}

	@Override
	public void messageArrived(String topic, MqttMessage msg) throws Exception {
		String code = new String(msg.getPayload());
		if(DoorLock.TARGET_NFC_CODE.equals(code)) {
			imageLabel.setIcon(iconOpen);
			
			new Thread(new Runnable() {
				@Override
				public void run() {
					try {
						Thread.sleep(DoorLock.WAIT_SECONDS * 1000);
						imageLabel.setIcon(iconClosed);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}).start();
		}
	}

}
