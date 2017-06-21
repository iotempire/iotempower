package at.fhooe.mcm.hba.mqtt;

import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;


public class MqttHandler {

	private MqttClient mClient;
	private MqttCallback mCallback;
	
	private static int qos             = 2;
	private static String broker       = "tcp://192.168.12.1:1883";
	private static String clientId     = "HBA_Simluator_Actuators";
	private static MemoryPersistence persistence = new MemoryPersistence();
	
	private boolean isConnected = false;
	
	public MqttHandler(MqttCallback callback) {
		mCallback = callback;
	}
	
	public void connect() {
		try {
			mClient = new MqttClient(broker, clientId, persistence);
	        
	        MqttConnectOptions connOpts = new MqttConnectOptions();
	        connOpts.setCleanSession(true);
	        
	        mClient.connect(connOpts);
	        mClient.setCallback(mCallback);
	        
	        isConnected = true;
	        
		} catch(MqttException me) {
			 writeExceptionToConsole(me);
        }
	}
	
	public void disconnect() {
		try {
			mClient.disconnect();

			isConnected = false;
        
		} catch(MqttException me) {
			 writeExceptionToConsole(me);
        }
	}
	
	public void publish(String topic, String content) {
		try {
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            
            mClient.publish(topic, message);
            
            System.out.println("Message published: " + topic);
            
        } catch(MqttException me) {
            writeExceptionToConsole(me);
        }
	}

	private void writeExceptionToConsole(MqttException me) {
		isConnected = false;
		
		System.out.println("reason " + me.getReasonCode());
        System.out.println("msg " + me.getMessage());
        System.out.println("loc " + me.getLocalizedMessage());
        System.out.println("cause " + me.getCause());
        System.out.println("excep " + me);
        me.printStackTrace();
	}
	
	public boolean isConnected() {
		return isConnected;
	}
	
	public void setConnected(boolean isConnected) {
		this.isConnected = isConnected;
	}
	
	public void subscribe(String topic) {
		try {
			mClient.subscribe(topic);
		} catch (MqttException me) {
			writeExceptionToConsole(me);
		}
	}
	
	public void unsubscribe(String topic) {
		try {
			mClient.unsubscribe(topic);
		} catch (MqttException me) {
			writeExceptionToConsole(me);
		}
	}
}
