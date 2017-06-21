package at.fhooe.mcm.hba;

import java.awt.Dimension;
import java.awt.Label;
import java.awt.Panel;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.BoxLayout;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import at.fhooe.mcm.hba.datastructure.Brightness;
import at.fhooe.mcm.hba.datastructure.IElement;
import at.fhooe.mcm.hba.mqtt.MqttHandler;

public class ContextManagement implements ChangeListener, MqttCallback {

	// GUI
	private Panel mContextPanel;
	private JSlider mSliderTransmissionFrequency;
	private JSlider mSliderBrightness;

	// Context Elements initial values
	private static final int INIT_TRANSMISSION_FREQUENCY = 1;
	private static final int INIT_BRIGHTNESS = 0;

	// Update ContextSituation
	private long transmissionFrequencySec = 1;
	private Timer timer;
	
	// MQTT
	private MqttHandler mMqttHandler;
	
	private static final String DISPLAY_TOPIC = "door/dp1/set";
	private static final String DISPLAY_CONTENT_CLEAR = "&&clear";

	public ContextManagement() {
		createView();
		startUpdateTimer();
		
		mMqttHandler = new MqttHandler(this);
		mMqttHandler.connect();
	}

	public Panel getView() {
		return mContextPanel;
	}

	private void createView() {
		mContextPanel = new Panel();
		mContextPanel.setLayout(new BoxLayout(mContextPanel, BoxLayout.Y_AXIS));

		// Transmission Frequency
		mContextPanel.add(createLabel("Transmission Frequency [sec]: "));
		mSliderTransmissionFrequency = createSlider(1, 20, INIT_TRANSMISSION_FREQUENCY, 1, 1);
		mContextPanel.add(mSliderTransmissionFrequency);
		mSliderTransmissionFrequency.addChangeListener(this);
		
		// Humidity
		mContextPanel.add(createLabel("Brightness [lx]: "));
		mSliderBrightness = createSlider(0, 500, INIT_BRIGHTNESS, 50, 10);
		mContextPanel.add(mSliderBrightness);

		// Hack for layout issues (hate java layouting :-/)
		mContextPanel.add(new Label());
	}

	private Label createLabel(String text) {
		Label label = new Label(text);
		label.setMaximumSize(new Dimension(MainWindow.WINDOW_WIDTH, 300));
		return label;
	}

	private JSlider createSlider(int min, int max, int value, int majorTickSpacing, int minorTickSpacing) {
		JSlider slider = new JSlider(JSlider.HORIZONTAL, min, max, value);
		slider.setMajorTickSpacing(majorTickSpacing);
		slider.setMinorTickSpacing(minorTickSpacing);
		slider.setPaintTicks(true);
		slider.setPaintLabels(true);
		return slider;
	}

	public void send(IElement element) {
		if (element != null) {
			if(mMqttHandler != null && mMqttHandler.isConnected()) {
				
				mMqttHandler.publish(DISPLAY_TOPIC, DISPLAY_CONTENT_CLEAR);
				mMqttHandler.publish(DISPLAY_TOPIC, element.toString());
			} else {
				System.out.println("Sending mqtt message failed!!");
			}
		}
	}

	@Override
	public void connectionLost(Throwable cause) {
		mMqttHandler.setConnected(false);
		
		cause.printStackTrace();
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken token) {
		// Nothing to do
	}
	
	@Override
	public void messageArrived(String arg0, MqttMessage arg1) throws Exception {
		// Nothing to do - Only sensors in this project
	}
	
	private void startUpdateTimer() {
		if (timer != null) {
			timer.cancel();
		}

		timer = new Timer();
		timer.schedule(new TimerTask() {

			@Override
			public void run() {
				Brightness brightness = new Brightness(mSliderBrightness.getValue());
				
				send(brightness);
			}
		}, 0, transmissionFrequencySec * 1000);
	}

	@Override
	public void stateChanged(ChangeEvent e) {
		JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
        	transmissionFrequencySec = (int) source.getValue();
            startUpdateTimer();
        }
	}
}
