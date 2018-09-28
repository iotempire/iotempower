/* setup.cpp
 * This is the configuration code for a ulnoiot node.
 * Here, you define all the devices (and eventually their interactions)
 * connected to the node specified with this directory.
 * If you want to see more device configuration examples,
 * check $ULNOIOT_ROOT/examples/running-node-test-setup.cpp
 * */

// The following is mostly example code, adjust to your needs accordingly.
// wifi and network and mqtt setup are done automatically
// based on global system definitions or values set in system.conf
//
// For some of the following example explanations, we assume that the current 
// node-name is test1
// and that this configuration is located in the folder myroom

//////// Global Variables ////////
// You might need in some cases to define global variable here
// Example:
// bool my_global_state = false;

//////// Device setup  ////////
// All device setup/configuration needs to be done here,
// look for the following commented examples for reference
// and uncomment and modify as needed
// check the output of uhelp at the ulnoiot command prompt for other
// supported devices and options.
// You might need to define some callback functions here (for filters
// or interactions).

// Led/Output example:
// The onboard-led is always available on development boards.
// With this configuration it will report under myroom/test1/blue
// and can be set via sending off or on to myroom/test1/blue/test.
// output(blue, ONBOARDLED, "off", "on").set("off");

// Input/button example:
// Add a button with a slightly higher debounce rate, which will report
// in the topic myroom/test1/button1.
// input(button1, D3, "released", "pressed")
//     .with_threshold(3)
//     .with_on_change_callback([&] {
//         if(IN(button1).is("pressed")) {
//             IN(blue).toggle();
//         }
//     });

// Examples for analog device:
// analog(a0).with_precision(5); //.with_threshold(100, "high", "low");

// Examples for displays
//display(d1, font_medium);
//display44780(d2, 16, 2);

// Example for pulse width modulation
// pwm(blue, ONBOARDLED).with_frequency(1000);

// Optional init function, which is called right before device initialization
// void init() {
// } // end init

// Optional start function, which is called directly after all devices are
// started. Here you can define everything to get things started, often nothing 
// void start() { // begin start, uncomment, if you need to start things 
//
//     // Trigger first blink
//     do_later(2000, blink);
//
//     // Send updates of current status every 10s (default 5)
//     transmission_interval(10);
//
// } // end start
