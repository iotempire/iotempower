M5StickC
========

The M5StickC is a portable, easy-to-use, tiny but mighty IoT development device.

Here you will find an example of how to use the M5StickC node in IoTempower.

Example
-------

Configure the ``node.conf`` file:

..  code-block:: bash
  
  board="m5stickc"

Configure the ``setup.cpp`` file:

..  code-block:: cpp

    const char* id="01";
    
    out(led, ONBOARDLED).inverted().off(); // initialize the onboard LED
    
    button(home, BUTTON_HOME, "pressed", "released").inverted().debounce(10);
    
    button(button, BUTTON_RIGHT, "pressed", "released").inverted().debounce(10);
    
    m5stickc_display(console, 1, 270); // initialize LCD display as 'console'

    void start() { 
        do_later(100, [] () { 
                IN(console).print("This is: stick-")
                .print(id);});} // print device id on LCD display (console)


**Note**:

- The ``inverted()`` method inverts the button logic.
- The ``debounce()`` method debounces the button.
- The ``m5stickc_display()`` method initializes the LCD display.
- The ``do_later()`` method prints the device id on the LCD display.
 
  - Helps keep track of multiple m5StickC devices.

- Change the **LCD display** color by sending an **MQTT** message to ``console``:
 
  - To adjust **background** color, use ``&&bg`` followed by a ``HEX color code``.
  - To adjust **foreground** color, use ``&&fg`` followed by a ``HEX color code``.
  
  - For example:  
   
    - Send: ``&&bg 000000`` to set the **background** to black: 
    
    ``mqtt_send your_m5_node/console &&bg 000000``
    
    - Send: ``&&fg ffffff`` to set the **foreground** to white: 
    
    ``mqtt_send your_m5_node/console &&fg ffffff``


Power switch operation:
-----------------------

- Turn it on/off
  
  - Power ``ON`` :Press power button for 2 seconds
  - Power ``OFF`` :Press power button for 6 seconds

- Charging:
 
  - The ``VBUS_VIN`` and ``VBUS_USB`` have a limited input range between ``4.8-5.5V``.
  - The ``AXP192`` power manager charges the battery from a powered ``VBUS``.

**Note:**

Supported baud rates: ``1200 ~115200, 250K, 500K, 750K, 1500K``


Node-RED Flow
-------------

This section includes an example Node-RED flow for controlling the M5StickC. 

The flow toggles the internal LED on and off using the side buttons and MQTT messages.

.. figure:: /doc/images/m5stickc_toggleLED_example_flow.png
   :width: 100%
   :figwidth: 100%
   :align: center
   :alt: M5StickC Toggle LED Example Flow
   :name: m5stickc_toggleLED_example_flow


This flow includes the following nodes:

- MQTT input nodes for the M5StickC buttons and LED.
- Debug nodes for debugging purposes.
- Switch nodes for toggling the LED on and off.
- Change nodes for setting the LED state.
- MQTT output node for sending the LED state to the M5StickC.

.. code-block:: json

    [
      {"id":"6d565a3c.17c584","type":"subflow","name":"Toggle","info":"","category":"","in":[{"x":60,"y":100,"wires":[{"id":"a6449155.3cdd8"}]}],"out":[{"x":440,"y":60,"wires":[{"id":"519f75ef.d3d8cc","port":0}]},{"x":440,"y":140,"wires":[{"id":"d35764b.91d1e98","port":0}]}],"env":[{"name":"option1","type":"bool","value":"true"},{"name":"option2","type":"bool","value":"false"}],"meta":{},"color":"#DDAA99","outputLabels":["Option 1","Option 2",""],"icon":"node-red/switch.svg"},{"id":"519f75ef.d3d8cc","type":"change","z":"6d565a3c.17c584","name":"Option 1","rules":[{"t":"set","p":"payload","pt":"msg","to":"option1","tot":"env"},{"t":"set","p":"next","pt":"flow","to":"0","tot":"str"}],"action":"","property":"","from":"","to":"","reg":false,"x":300,"y":80,"wires":[[]]},{"id":"d35764b.91d1e98","type":"change","z":"6d565a3c.17c584","name":"Option 2","rules":[{"t":"set","p":"payload","pt":"msg","to":"option2","tot":"env"},{"t":"set","p":"next","pt":"flow","to":"1","tot":"str"}],"action":"","property":"","from":"","to":"","reg":false,"x":300,"y":120,"wires":[[]]},{"id":"a6449155.3cdd8","type":"switch","z":"6d565a3c.17c584","name":"","property":"next","propertyType":"flow","rules":[{"t":"eq","v":"1","vt":"str"},{"t":"else"}],"checkall":"true","repair":false,"outputs":2,"x":150,"y":100,"wires":[["519f75ef.d3d8cc"],["d35764b.91d1e98"]]},{"id":"0219001e4cf6fd99","type":"tab","label":"Flow 1","disabled":false,"info":"","env":[]},{"id":"afef96c16940b2dd","type":"debug","z":"0219001e4cf6fd99","name":"debug 1","active":true,"tosidebar":true,"console":false,"tostatus":false,"complete":"false","statusVal":"","statusType":"auto","x":360,"y":200,"wires":[]},{"id":"a43e9023d176c29b","type":"mqtt in","z":"0219001e4cf6fd99","name":"m5stick/buttom","topic":"m5stick/buttom","qos":"2","datatype":"auto-detect","broker":"0edb8bf3e9c2706a","nl":false,"rap":true,"rh":0,"inputs":0,"x":140,"y":200,"wires":[["afef96c16940b2dd","0d941c4712540c41"]]},{"id":"9cba322878ed2bba","type":"mqtt in","z":"0219001e4cf6fd99","name":"m5stick/led","topic":"m5stick/led","qos":"2","datatype":"auto-detect","broker":"0edb8bf3e9c2706a","nl":false,"rap":true,"rh":0,"inputs":0,"x":140,"y":260,"wires":[["afef96c16940b2dd"]]},{"id":"8ff63993707fa0a0","type":"mqtt in","z":"0219001e4cf6fd99","name":"m5stick/home","topic":"m5stick/home","qos":"2","datatype":"auto-detect","broker":"0edb8bf3e9c2706a","nl":false,"rap":true,"rh":0,"inputs":0,"x":140,"y":320,"wires":[["afef96c16940b2dd","0d941c4712540c41"]]},{"id":"c2218b78ba588270","type":"mqtt out","z":"0219001e4cf6fd99","name":"","topic":"m5stick/led/set","qos":"","retain":"false","respTopic":"","contentType":"","userProps":"","correl":"","expiry":"","broker":"0edb8bf3e9c2706a","x":750,"y":380,"wires":[]},{"id":"2ade6942db5dcec8","type":"switch","z":"0219001e4cf6fd99","name":"","property":"payload","propertyType":"msg","rules":[{"t":"eq","v":"pressed","vt":"str"}],"checkall":"true","repair":false,"outputs":1,"x":440,"y":320,"wires":[["0f095ef88d239930"]]},{"id":"0d941c4712540c41","type":"rbe","z":"0219001e4cf6fd99","name":"","func":"rbe","gap":"","start":"","inout":"out","septopics":true,"property":"payload","topi":"topic","x":370,"y":260,"wires":[["2ade6942db5dcec8","e8c2a8f9dd2a3ac1"]]},{"id":"ee498e28af20f459","type":"change","z":"0219001e4cf6fd99","name":"set off","rules":[{"t":"set","p":"payload","pt":"msg","to":"off","tot":"str"}],"action":"","property":"","from":"","to":"","reg":false,"x":630,"y":440,"wires":[["c2218b78ba588270"]]},{"id":"3e8b53b6b8a5acdf","type":"change","z":"0219001e4cf6fd99","name":"set on","rules":[{"t":"set","p":"payload","pt":"msg","to":"on","tot":"str"}],"action":"","property":"","from":"","to":"","reg":false,"x":640,"y":320,"wires":[["c2218b78ba588270"]]},{"id":"e8c2a8f9dd2a3ac1","type":"debug","z":"0219001e4cf6fd99","name":"debug 2","active":false,"tosidebar":true,"console":false,"tostatus":false,"complete":"payload","targetType":"msg","statusVal":"","statusType":"auto","x":600,"y":260,"wires":[]},{"id":"0f095ef88d239930","type":"subflow:6d565a3c.17c584","z":"0219001e4cf6fd99","name":"","x":480,"y":380,"wires":[["3e8b53b6b8a5acdf"],["ee498e28af20f459"]]},{"id":"0edb8bf3e9c2706a","type":"mqtt-broker","name":"local gw","broker":"192.168.91.29","port":"1883","clientid":"","autoConnect":true,"usetls":false,"protocolVersion":"4","keepalive":"60","cleansession":true,"autoUnsubscribe":true,"birthTopic":"","birthQos":"0","birthRetain":"false","birthPayload":"","birthMsg":{},"closeTopic":"","closeQos":"0","closeRetain":"false","closePayload":"","closeMsg":{},"willTopic":"","willQos":"0","willRetain":"false","willPayload":"","willMsg":{},"userProps":"","sessionExpiry":""}
    ]

To import this flow into your Node-RED setup:

1. Copy the above JSON array.
2. Go to your Node-RED instance.
3. Click on the menu at the top right corner (three horizontal lines).
4. Select *Import* from the drop-down menu.
5. Paste the copied JSON in the text field that appears, and then click *Import*.



Physical Features
-----------------


.. table::
   :widths: auto

   +----------------------+--------------------------------------------------+
   | Resources            | Parameter                                        |
   +======================+==================================================+
   | ESP32                | 240MHz dual core, 600 DMIPS, 520KB SRAM, Wi-Fi   |
   +----------------------+--------------------------------------------------+
   | Flash Memory         | 4MB                                              |
   +----------------------+--------------------------------------------------+
   | Power Input          | 5V @ 500mA                                       |
   +----------------------+--------------------------------------------------+
   | Port                 | TypeC x 1, GROVE (I2C+I/0+UART) x 1              |
   +----------------------+--------------------------------------------------+
   | LCD screen           | 0.96 inch, 80*160 Colorful TFT LCD, ST7735S      |
   +----------------------+--------------------------------------------------+
   | Button               | Custom button x 2                                |
   +----------------------+--------------------------------------------------+
   | LED                  | RED LED                                          |
   +----------------------+--------------------------------------------------+
   | MEMS                 | MPU6886                                          |
   +----------------------+--------------------------------------------------+
   | IR                   | Infrared transmission                            |
   +----------------------+--------------------------------------------------+
   | MIC                  | SPM1423                                          |
   +----------------------+--------------------------------------------------+
   | RTC                  | BM8563                                           |
   +----------------------+--------------------------------------------------+
   | PMU                  | AXP192                                           |
   +----------------------+--------------------------------------------------+
   | Battery              | 95 mAh @ 3.7V                                    |
   +----------------------+--------------------------------------------------+
   | Antenna              | 2.4G 3D Antenna                                  |
   +----------------------+--------------------------------------------------+
   | PIN port             | G0, G26, G36                                     |
   +----------------------+--------------------------------------------------+
   | Operating Temperature| 0°C to 60°C                                      |
   +----------------------+--------------------------------------------------+
   | Net weight           | 15.1g                                            |
   +----------------------+--------------------------------------------------+
   | Gross weight         | 33g                                              |
   +----------------------+--------------------------------------------------+
   | Product Size         | 48.2*25.5*13.7mm                                 |
   +----------------------+--------------------------------------------------+
   | Package Size         | 55*55*20mm                                       |
   +----------------------+--------------------------------------------------+
   | Case Material        | Plastic (PC)                                     |
   +----------------------+--------------------------------------------------+


Resources
---------

Product page:
    https://shop.m5stack.com/products/stick-c?variant=43982750843137