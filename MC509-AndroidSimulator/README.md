Android Lightbulb simulator that behaves close to real BULBs
based on Mqtt

There are two bulbs that are accessed independently.

Each have the functions:

- ON / OFF
- Change color (by setting RGB Values) -> "255,255,255"
- Change brighntess (by setting alpha value) -> 255

Also a status broadcast is implemented. 



Before deploying, please adjust TOPICs for your needs by changing this static values

    public static final String TOPIC_BULB2_SET_ON = "sensors/rgb2/set";
    public static final String TOPIC_BULB2_SET_COLOR = "sensors/rgb2/rgb/set";
    public static final String TOPIC_BULB2_SET_BRIGHTNESS =  "sensors/rgb2/brightness/set";

    public static final String TOPIC_BULB1_ON_STATUS = "sensors/rgb1/status";
    public static final String TOPIC_BULB1_COLOR_STATUS = "sensors/rgb1/rgb/status";
    public static final String TOPIC_BULB1_BRIGHTNESS_STATUS = "sensors/rgb1/brightness/status";

    public static final int PUBLISH_INTERVAL = 5000; //ms

    public static final String TOPIC_BULB2_ON_STATUS = "sensors/rgb2/status";
    public static final String TOPIC_BULB2_COLOR_STATUS = "sensors/rgb2/rgb/status";
    public static final String TOPIC_BULB2_BRIGHTNESS_STATUS = "sensors/rgb2/brightness/status";

