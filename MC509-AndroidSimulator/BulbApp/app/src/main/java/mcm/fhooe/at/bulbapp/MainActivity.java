package mcm.fhooe.at.bulbapp;

import android.graphics.Color;
import android.graphics.PorterDuff;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.DisconnectedBufferOptions;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import static android.webkit.ConsoleMessage.MessageLevel.LOG;

public class MainActivity extends AppCompatActivity {

    ImageView bulb1, bulb2;
    MqttCallback callback;
    MqttAndroidClient androidClient;
    final String subscriptionTopic = "app";

    final int standardOn = Color.rgb(255,247,40);
    final int standardOff = Color.rgb(175,175,175);

    int bulb1alpha = 255;
    int bulb1red = -1;
    int bulb1green = -1;
    int bulb1blue = -1;

    int bulb2alpha = 255;
    int bulb2red = -1;
    int bulb2green = -1;
    int bulb2blue = -1;

    boolean bulb1on = false;
    boolean bulb2on = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupUI();
        setupMQTT();
    }

    private void setupUI() {
        bulb1 = (ImageView) findViewById(R.id.imageView);
        bulb2 = (ImageView) findViewById(R.id.imageView2);
        bulb1.setImageDrawable(getDrawable(R.drawable.bulb_on_new));
        bulb2.setImageDrawable(getDrawable(R.drawable.bulb_on_new));
        bulb1.setColorFilter(standardOff, PorterDuff.Mode.MULTIPLY);
        bulb2.setColorFilter(standardOff, PorterDuff.Mode.MULTIPLY);
    }

    private void setupMQTT() {
        String broker = "tcp://192.168.12.1:1883";
        String clientId = "app";
        androidClient = new MqttAndroidClient(getApplicationContext(), broker, clientId);
        androidClient.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean reconnect, String serverURI) {

                if (reconnect) {
                    Log.i("TAG", "Reconnected to : " + serverURI);
                    // Because Clean Session is true, we need to re-subscribe
                    subscribeToTopic();
                } else {
                    Log.i("TAG", "Connected to: " + serverURI);
                    MqttMessage msg = new MqttMessage("hello".getBytes());
                    try {
                        androidClient.publish("app/publish",msg);
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }
            }
            @Override
            public void connectionLost(Throwable cause) {
                Log.i("TAG", "The Connection was lost.");
            }
            /*
            sensors/rgb/brightness/status 255
            sensors/rgb/rgb/status (255, 255, 255)
            sensors/rgb on/off
             */

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                Log.i("TAG","Topic: "+topic+" | message: "+message.toString());

                switch (topic){
                    case "sensors/rgb1":
                        if(message.toString().equalsIgnoreCase("on")){
                            //bulb1.setImageDrawable(getDrawable(R.drawable.bulb_on_new));
                            if(bulb1red != -1){
                                bulb1.setColorFilter(getBulb1Color(), PorterDuff.Mode.MULTIPLY);
                            }else{
                                bulb1.setColorFilter(standardOn, PorterDuff.Mode.MULTIPLY);
                            }
                            bulb1on = true;
                        }else if(message.toString().equalsIgnoreCase("off")){
                            bulb1.setColorFilter(standardOff, PorterDuff.Mode.MULTIPLY);
                            bulb1on = false;
                            //bulb2alpha= 255;
                        }
                        break;
                    case "sensors/rgb2":
                        if(message.toString().equalsIgnoreCase("on")){
                           // bulb2.setImageDrawable(getDrawable(R.drawable.bulb_on_new));
                            bulb2on = true;
                            if(bulb2red != -1){
                                bulb2.setColorFilter(getBulb2Color(), PorterDuff.Mode.MULTIPLY);
                            }else{
                                bulb2.setColorFilter(standardOn, PorterDuff.Mode.MULTIPLY);
                            }
                        }else if(message.toString().equalsIgnoreCase("off")){
                            bulb2.setColorFilter(standardOff, PorterDuff.Mode.MULTIPLY);
                            //bulb2alpha = 255;
                            bulb2on = false;
                        }
                        break;
                    case "sensors/rgb1/rgb/status":
                        setColorFromMessage(message.toString(),true);
                        if(bulb1on){
                           bulb1.setColorFilter(getBulb1Color(), PorterDuff.Mode.MULTIPLY);
                        }
                        break;
                    case "sensors/rgb2/rgb/status":
                        setColorFromMessage(message.toString(),false);
                        if(bulb2on){
                            bulb2.setColorFilter(getBulb2Color(), PorterDuff.Mode.MULTIPLY);
                        }
                        break;
                    case "sensors/rgb1/brightness/status":
                        setAlphaFromMessage(message.toString(),true);
                        if(bulb1on){
                            bulb1.setColorFilter(getBulb1Color(), PorterDuff.Mode.MULTIPLY);
                        }
                        break;
                    case "sensors/rgb2/brightness/status":
                        setAlphaFromMessage(message.toString(),false);
                        if(bulb2on){
                            bulb2.setColorFilter(getBulb2Color(), PorterDuff.Mode.MULTIPLY);
                        }
                        break;
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
            }
            });

        MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
        mqttConnectOptions.setAutomaticReconnect(true);
        mqttConnectOptions.setCleanSession(false);

        try {
            androidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    DisconnectedBufferOptions disconnectedBufferOptions = new DisconnectedBufferOptions();
                    disconnectedBufferOptions.setBufferEnabled(true);
                    disconnectedBufferOptions.setBufferSize(100);
                    disconnectedBufferOptions.setPersistBuffer(false);
                    disconnectedBufferOptions.setDeleteOldestMessages(false);
                    androidClient.setBufferOpts(disconnectedBufferOptions);
                    subscribeToTopic();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to connect to: ");
                    exception.printStackTrace();
                }
            });


        } catch (MqttException ex){
            ex.printStackTrace();
        }

    }

    private void setAlphaFromMessage(String s,boolean firstBulb){
        if(firstBulb){
            bulb1alpha = Integer.parseInt(s);
        }else{
            bulb2alpha = Integer.parseInt(s);
        }
    }


    private void setColorFromMessage(String s,boolean firstBulb){
        //String local = s.substring(1,s.length()-1);
        String[] rgb = s.split(",");
        int r = Integer.parseInt(rgb[0]);
        int g = Integer.parseInt(rgb[1]);
        int b = Integer.parseInt(rgb[2]);
        if(firstBulb){
            bulb1red = r;
            bulb1green = g;
            bulb1blue = b;
        }else{
            bulb2red = r;
            bulb2green = g;
            bulb2blue = b;
        }
    }

    private int getBulb1Color() {
        return Color.argb(bulb1alpha,bulb1red,bulb1green,bulb1blue);
    }

    private int getBulb2Color() {
        return Color.argb(bulb2alpha,bulb2red,bulb2green,bulb2blue);
    }

    public void subscribeToTopic() {
        try {

            androidClient.subscribe("sensors/rgb1", 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i("TAG","bulb1 Subscribed!");
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to subscribe");
                }
            });
            androidClient.subscribe("sensors/rgb2", 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i("TAG","bulb 2 Subscribed!");
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to subscribe");
                }
            });
            androidClient.subscribe("sensors/rgb1/rgb/status", 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i("TAG","bulb1 Subscribed!");
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to subscribe");
                }
            });
            androidClient.subscribe("sensors/rgb2/rgb/status", 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i("TAG","bulb 2 Subscribed!");
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to subscribe");
                }
            });
            androidClient.subscribe("sensors/rgb1/brightness/status", 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i("TAG","bulb1 Subscribed!");
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to subscribe");
                }
            });
            androidClient.subscribe("sensors/rgb2/brightness/status", 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i("TAG","bulb 2 Subscribed!");
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i("TAG","Failed to subscribe");
                }
            });


            androidClient.subscribe(subscriptionTopic, 0, new IMqttMessageListener() {
                @Override
                public void messageArrived(String topic, MqttMessage message) throws Exception {
                    System.out.println("Message: " + topic + " : " + new String(message.getPayload()));
                }
            });

        } catch (MqttException ex) {
            System.err.println("Exception whilst subscribing");
            ex.printStackTrace();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        try {
            androidClient.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        setupMQTT();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            androidClient.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
