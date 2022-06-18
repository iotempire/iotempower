/* Sonoff Double Touch
 * with gestures, long click, and double click
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

uint16_t touch_state = 0; // bit pattern for the touched buttons

input(button1, BUTTON1).invert()
    .debounce(DEBOUNCE)
    .filter([] (Device &dev) {
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
    .on_change([] (Device& dev) {
         if(dev.is("click")) {
            IN(relais1).toggle();
         }
         return true;
     });

input(button2, BUTTON2).invert()
    .debounce(DEBOUNCE)
    .filter([] (Device &dev) {
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
    .on_change([] (Device& dev) {
         if(dev.is("click")) {
             IN(relais2).toggle();
         }
         return true;
     });
