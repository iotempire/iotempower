/* setup.cpp
 * This is the configuration code for a IoTempower node.
 * This file is a full base example for all devices possible in 
 * IoTempower. There is a configuration for each device possible in here.
 * It serves and includes current configuration tests.
 * 
 * TODO: split into single files/example node-folders.
 * */


//////// Device setup  ////////
// output example
// output(blue, ONBOARDLED, "off", "on").invert().set("on");
// void blink() {
//     IN(blue).toggle();
//     do_later(2000, blink);
// };

// input example
// Add a button with a slightly higher debounce rate, which will report
// in the topic myroom/test1/button1.
input(button1, D3, "released", "pressed");
//      .with_threshold(3)
//      .on_change([] (Device& dev) {
//           if(dev.is("pressed")) {
//               IN(blue).toggle();
//           }
//           return true;
//       });

// Examples for analog device:
// analog(a0).with_precision(5); //.with_threshold(100, "high", "low");
// Using _ in end of device allows giving internal name and "external name"
// analog(a0).filter( [] (Device& dev) {
//         const int buflen = 100;
//         static long sum = 0;
//         static long values_count = 0;
//         int v = dev.read_int();
//         sum += v;
//         values_count ++;
//         if(values_count >= buflen) {
//             dev.value().from(sum / values_count);
//             values_count = 0;
//             sum = 0;
//             return true;
//         }
//         return false;
//     });
// The same as above, but shorter with generic filter
// analog(a0).filter_average(100);

// gesture/color/proximity
//gesture_apds9960(gesture);

// edge_counter example
// Count rising and falling signals on D2=Pin(4) and
// report number counted at myroom/test1/shock1.
// edge_counter(shock1, D2);

// Examples for temperature (and humidity) measurement
// dht(th1 D7);
// ds18b20(temp1, D2);

// Servo motor
// servo(m1, D6, 800);

//// Distance sensors

// Acoustic distance sensor
//hcsr04(distance, D5, D6).with_precision(10);
//hcsr04(distance, D5, D6)
//    .filter_binarize(200, "off", "on");

// Laser distance sensor
//vl53l0x distance sensor
//vl53l0x(dist);


// HX711 weight sensor (sensor for scales)
//hx711(weight, D6, D7, 419.0, true)
//    .filter_round(2, IN(weight));

// Barometers
//bmp180(bmp1);
//bmp280(bmp2);

// i2c connector socket - talk to an Arduino talking on i2c on address 9
// (called plug there)
// i2c_socket(arduino1, 9, 8).i2c(100000).pollrate(100);
//i2c_socket(arduino1).pollrate(2000);

// MFRC522 RFID/NFC reader example (SPI sensor)
//mfrc522(rfid1, 32);

//// light sensitivity sensors
//bh1750(lux1);
//tsl2561(lux2);

// Example for single RGB-led
// rgb(r0, D6, D5, D0, true);


//// RGB strips

// small wemos 7-leds shield (careful, on D4=ONBOARDLED)
// rgb_strip(strip1, 7, WS2812B, D4, GRB);

//rgb_strip(strip1, 90, WS2811, D2, BRG);
// // rgb_strip_(strip2, "strip2", 50, WS2811, D5, BRG);
// // rgb_strip_(strip3, "strip3", 50, WS2811, D4, BRG);
// // rgb_strip_(strip4, "strip4", 50, WS2811, D1, BRG);

// A matrix consiting of several strips
// rgb_matrix(matrix, 25, 2)
//        .with(strip1, 0, 0, Right_Down, 25);
// //       .with(strip2, 0, 1)
// //       .with(strip3, 0, 2)
// //       .with(strip4, 0, 3);

// // following is a long example for the animator device with
// // its global variables and  support functions 
// // (this one animates the matrix defined beforehand)
//
// frames, anim_type, and anim_types are needed for testing the animation
// unsigned long frames[4]={0,0,0,0};
// enum anim_type {none, fade, fade_to, scroll};
// anim_type anim_types[4]={none, none, none, none};
//
// void draw_pattern(int p, int line, int len) {
//     switch(p) {
//         case 1:
//             IN(matrix).rainbow(0, line, len, 1);
//             break;
//         case 2:
//             IN(matrix).gradient_row(CRGB::Green, CRGB::Blue, 0, line, len, 1);
//             break;
//         case 3:
//             IN(matrix).gradient_row(CRGB::Blue, CRGB::Red, 0, line, len, 1);
//             break;
//         default:
//             break;
//     }
// }
//
// void set_animation(Ustring& command, anim_type at, int leds, int frame_count) {
//     int stripnr = limit(command.as_int() - 1, 0, 3);
//     command.strip_param();
//     int pattern = limit(command.as_int(), 1, 3);
//     draw_pattern(IN(matrix), pattern, stripnr, -1);
//     anim_types[stripnr] = at;
//     frames[stripnr] = frame_count;
// }
//
// // this defines the actual animator object
// animator(anim, IN(matrix))
//     .with_fps(10)
//     .with_frame_builder( [] () {
//         for(int i=0; i<4; i++) {
//             if(frames[i]>0 && anim_types[i] != none) {
//                 switch(anim_types[i]) {
//                     case fade:
//                         IN(matrix).fade(8, 0, i, -1, 1);
//                         break;
//                     case scroll:
//                         IN(matrix).scroll_right(false,i);
//                         break;
//                     case fade_to:
//                         IN(matrix).fade_to(CRGB::Red, 16, 0, i, -1, 1);
//                     default:
//                         break; 
//                 }
//                 frames[i] --;
//             }
//         }
//     } ).with_command_handler( "fade", [] (Ustring& command) {
//         set_animation(command, fade, -1, 100);
//     } ).with_command_handler( "fade_to", [] (Ustring& command) {
//         set_animation(command, fade_to, -1, 50);
//     } ).with_command_handler( "scroll", [] (Ustring& command) {
//         set_animation(command, scroll, 5, 150);
//     } );

//// Examples for displays

// standard ssd1306 display
//display(d1, font_medium).i2c(D6, D5);

// small ssd1306 as wemos shield
//U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2d(U8G2_R0);
//display(d1, u8g2d);

// 2 lines 44780 i2c lcd display
display44780(d2, 16, 2);

// Example for pulse width modulation
// pwm(blue, ONBOARDLED).with_frequency(1000);

//// Examples for gyros

// MPU-6050
// gyro6050(g0).filter( [] (Device& dev) { // fuse accel value into one
//    // ignore angles
//    dev.value(0).clear();
//    int a,b,c;
//    if(dev.value(1).scanf("%d,%d,%d", &a, &b, &c)!=3)
//        return false;
//    unsigned long d = sqrt((unsigned long)(a*a) + (b*b) + (c*c)) + 0.5;
//    dev.value(1).from(d>27?1:0);
//    return true;
// });

// MPU-9250
// gyro9250(g1);

// MPR121 capacitive touch
//mpr121(cap);

// Optional init function, which is called right before device initialization
// void init() {
// } // end init

// Optional start function, which is called directly after all devices are
// started 
// Here you can define everything to get things started, often nothing 
// is necessary here. This is called after device initialization.
// Things like initial do_later calls should go here
// as well as a call to transmission_interval if something else than
// the initial 5s is preferred.
void start() { // begin start, uncomment, if you need to start things 
//
//     // Trigger first blink
//     do_later(2000, blink);
//
//     // fall into deep sleep in 15s for 60s
//     deep_sleep(15000, 60000);
// 
//     // Send updates of current status every 10s (default 5)
//     transmission_interval(10);
//
} // end start
