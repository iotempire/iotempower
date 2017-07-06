package at.fhooe.mcm.hba;

import java.awt.FlowLayout;
import java.awt.Label;
import java.awt.Panel;
import java.util.ArrayList;
import java.util.List;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import at.fhooe.mcm.hba.datastructure.IElement;
import at.fhooe.mcm.hba.datastructure.Led;
import at.fhooe.mcm.hba.listeners.ViewChangedListener;
import at.fhooe.mcm.hba.mqtt.MqttHandler;

public class ContextManagement implements MqttCallback {

	// GUI
	private Panel mContextPanel;
	private JSpinner mSpinnerLedCount;
	private JPanel mPanelLeds;
	private List<Led> mLeds;
	
	// INIT VALUES
	private static final int LED_COUNT_INIT = 5;
	
	// MQTT
	private MqttHandler mMqttHandler;
	
	// LISTENERS
	private ViewChangedListener mViewChangedListener;
	
	private int mCurrSpinnerLedCount = LED_COUNT_INIT;
	

	public ContextManagement() {
		mLeds = new ArrayList<Led>();
		
		mMqttHandler = new MqttHandler(this);
		mMqttHandler.connect();
		
		createView();
	}
	
	public void addListener(ViewChangedListener viewChangedListener) {
		mViewChangedListener = viewChangedListener;
	}

	public Panel getView() {
		return mContextPanel;
	}

	private void createView() {
		mContextPanel = new Panel();
		mContextPanel.setLayout(new BoxLayout(mContextPanel, BoxLayout.Y_AXIS));
		
		createLedCount(mContextPanel);
		createLedsPanel(mContextPanel);
	}
	
	private void createLedCount(Panel parent) {
		JPanel panelLedCount = new JPanel();
		panelLedCount.setLayout(new BoxLayout(panelLedCount, BoxLayout.Y_AXIS));
		
		panelLedCount.add(new Label("Led Count:"));
		
		mSpinnerLedCount = new JSpinner();
		mSpinnerLedCount.setValue(LED_COUNT_INIT);
		mSpinnerLedCount.addChangeListener(new ChangeListener() {
			
			@Override
			public void stateChanged(ChangeEvent e) {
				int value = (int) mSpinnerLedCount.getValue();
				
				if(mCurrSpinnerLedCount < value) { // UP
					addLed(mPanelLeds);
				} else { // DOWN
					removeLed(mPanelLeds);
					LedFactory.decreaseLedCounter();
				}
				
				mCurrSpinnerLedCount = value;
				
				// Notify the listeners that the view changed
				mViewChangedListener.viewChanged();
			}
		});
		panelLedCount.add(mSpinnerLedCount);
		
		// Hack for layout issues (hate java layouting :-/)
		panelLedCount.add(new Label());
		panelLedCount.add(new Label());
		panelLedCount.add(new Label());
		
		parent.add(panelLedCount);
	}
	
	private void createLedsPanel(Panel parent) {
		mPanelLeds = new JPanel(new FlowLayout(FlowLayout.CENTER));
		
		int ledCount = (int) mSpinnerLedCount.getValue();
		for(int i = 0; i < ledCount; i++) {
			addLed(mPanelLeds);
		}
		
		parent.add(mPanelLeds);
	}
	
	private void addLed(JPanel parent) {
		Led led = LedFactory.createLed();
		
		mLeds.add(led);
		parent.add(led);
		
		mMqttHandler.subscribe(led.getTopic());
	}
	
	private void removeLed(JPanel parent) {
		Led led = mLeds.get(mLeds.size()-1);
		
		mLeds.remove(led);
		parent.remove(led);
		
		mMqttHandler.unsubscribe(led.getTopic());
	}

	public void send(IElement element) {
		// Nothing to do - Only actuators in this project
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
	public void messageArrived(String topic, MqttMessage message) throws Exception {
		
		int ledCount = (int) mSpinnerLedCount.getValue();
		
		for(int i = 0; i < ledCount; i++) {
			Led led = mLeds.get(i);
			
			if(led.getTopic().equals(topic)) {
				led.setSelected(convertOnOffToBoolean(message.toString()));
				
				// topics are unique
				break;
			}
		}
	}
	
	private boolean convertOnOffToBoolean(String msg) {
		switch(msg) {
			case "on":
				return true;
			case "off":
				return false;
			default:
				System.out.println("ON/OFF can not be converted!!!!");
				return false;
		}
	}
}
