/* Sonoff T1 3 with gestures, long click, and double click*/

//// Testing on Wemos D1 Mini
//#define BTN1 D1
//#define BTN2 D2
//#define BTN3 D3
//#define RL1 D5
//#define RL2 D6
//#define RL3 D7

// Real Sonoff T1 3 Channel
#define BTN1 BUTTON1
#define BTN2 BUTTON2
#define BTN3 BUTTON3
#define RL1 RELAIS1
#define RL2 RELAIS2
#define RL3 RELAIS3

#define GESTURE 1500
#define DEBOUNCE 10

led(blue, ONBOARDLED).invert().off();
output(relais1, RL1).light().off();
output(relais2, RL2).light().off();
output(relais3, RL3).light().off();

unsigned long lastb1 = millis();
unsigned long lastb2 = millis();
unsigned long lastb3 = millis();

uint16_t touch_state = 0;

input(button1, BTN1).invert()
    .debounce(DEBOUNCE)
    .filter([&] (Device &dev) {
        if(dev.value().equals("on")) {
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
         if(dev.value().equals("click")) {
             unsigned long current = millis();
             if(current - lastb1 > 500) {
                 if(current - lastb2 < GESTURE 
                       && current - lastb3 < GESTURE) { // gesture
                     // undo previous toggles
                     IN(relais2).toggle();
                     IN(relais3).toggle();
                     dev.value().from("up_gesture");
                 } else {
                     IN(relais1).toggle();
                 }
             }
             lastb1 = current;
         }
         return true;
     });

input(button2, BTN2).invert()
    .debounce(DEBOUNCE)
    .filter([&] (Device &dev) {
        if(dev.value().equals("on")) {
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
         if(dev.value().equals("click")) {
             lastb2 = millis();
             IN(relais2).toggle();
         }
         return true;
     });

input(button3, BTN3).invert()
    .debounce(DEBOUNCE)
    .filter([&] (Device &dev) {
        if(dev.value().equals("on")) {
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
         if(dev.value().equals("click")) {
             unsigned long current = millis();
             if(current - lastb3 > 500) {
                 if(current-lastb1 < GESTURE 
                       && current-lastb2 < GESTURE) { // gesture
                     // undo previous toggles
                     IN(relais1).toggle();
                     IN(relais2).toggle();
                     dev.value().from("down_gesture");
                 } else {
                     IN(relais3).toggle();
                 }
             }
             lastb3 = current;
         }
         return true;
     });

void all_off() {
    IN(relais1).off();
    IN(relais2).off();
    IN(relais3).off();
}

void start() {
    do_later(500, all_off);
}
