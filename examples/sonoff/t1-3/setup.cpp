/* Sonoff T1 3 with gestures, long click, and double click
 * 
 * Board has to be set to sonoff in node.conf
 */

// Time the gesture can last
#define GESTURE 1800
// Number of debounce counts
#define DEBOUNCE 10

led(blue, ONBOARDLED).invert().off().report_change(false);
output(relais1, RELAIS1).light().off();
output(relais2, RELAIS2).light().off();
output(relais3, RELAIS3).light().off();

unsigned long lastb1 = millis();
unsigned long lastb2 = millis();
unsigned long lastb3 = millis();

uint16_t touch_state = 0; // bit pattern for the touched buttons

input(button1, BUTTON1).invert()
    .debounce(DEBOUNCE)
    .filter([&] (Device &dev) {
        if(dev.is("on")) {
            touch_state |= 1;
        } else {
            touch_state &= 254; 
        }
        if(touch_state) IN(blue).on();
        else IN(blue).off();
        return true;
    })
    .filter_click_detector(20, 800, 1000, 2500)
    .on_change([&] (Device& dev) {
         if(dev.is("click")) {
             unsigned long current = millis();
             if(current - lastb1 > 500) {
                 if(current - lastb2 < GESTURE 
                       && current - lastb3 < GESTURE) { // gesture
                     // undo previous toggles
                     IN(relais2).toggle();
                     IN(relais3).toggle();
                     dev.write("up_gesture");
                 } else {
                     IN(relais1).toggle();
                 }
             }
             lastb1 = current;
         }
         return true;
     });

input(button2, BUTTON2).invert()
    .debounce(DEBOUNCE)
    .filter([&] (Device &dev) {
        if(dev.is("on")) {
            touch_state |= 2;
        } else {
            touch_state &= 253; 
        }
        if(touch_state) IN(blue).on();
        else IN(blue).off();
        return true;
    })
    .filter_click_detector(20, 800, 1000, 2500)
    .on_change([&] (Device& dev) {
         if(dev.is("click")) {
             lastb2 = millis();
             IN(relais2).toggle();
         }
         return true;
     });

input(button3, BUTTON3).invert()
    .debounce(DEBOUNCE)
    .filter([&] (Device &dev) {
        if(dev.is("on")) {
            touch_state |= 4;
        } else {
            touch_state &= 251; 
        }
        if(touch_state) IN(blue).on();
        else IN(blue).off();
        return true;
    })
    .filter_click_detector(20, 800, 1000, 2500)
    .on_change([&] (Device& dev) {
         if(dev.is("click")) {
             unsigned long current = millis();
             if(current - lastb3 > 500) {
                 if(current-lastb1 < GESTURE 
                       && current-lastb2 < GESTURE) { // gesture
                     // undo previous toggles
                     IN(relais1).toggle();
                     IN(relais2).toggle();
                     dev.write("down_gesture");
                 } else {
                     IN(relais3).toggle();
                 }
             }
             lastb3 = current;
         }
         return true;
     });
