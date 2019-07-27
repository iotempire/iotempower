//output(blue, ONBOARDLED).invert();
//input(b1, D3, "pressed", "released").invert();

/* Sonoff T1 3 */

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
#define DEBOUNCE 80

led(blue, ONBOARDLED).invert().on();
output(relais1, RL1).light().off();
output(relais2, RL2).light().off();
output(relais3, RL3).light().off();

const char* pressed = "pressed";
const char* released = "released";

unsigned long lastb1 = millis();
unsigned long lastb2 = millis();
unsigned long lastb3 = millis();

input(button1, BTN1, pressed, released).invert()
    .with_threshold(DEBOUNCE)
    .with_on_change_callback([&] {
         if(IN(button1).is(pressed)) {
             unsigned long current = millis();
             if(current - lastb1 > 500) {
                 if(current-lastb2 < GESTURE 
                       && current-lastb3 < GESTURE) { // gesture
                     // undo previous toggles
                     IN(relais2).toggle();
                     IN(relais3).toggle();
                     IN(button1).value().from("up_gesture");
                 } else {
                     IN(relais1).toggle();
                 }
             }
             lastb1 = current;
         }
     });

input(button2, BTN2, pressed, released).invert()
    .with_threshold(DEBOUNCE)
    .with_on_change_callback([&] {
         if(IN(button2).is(pressed)) {
             lastb2 = millis();
             IN(relais2).toggle();
         }
     });

input(button3, BTN3, pressed, released).invert()
    .with_threshold(DEBOUNCE)
    .with_on_change_callback([&] {
         if(IN(button3).is(pressed)) {
             unsigned long current = millis();
             if(current - lastb3 > 500) {
                 if(current-lastb1 < GESTURE 
                       && current-lastb2 < GESTURE) { // gesture
                     // undo previous toggles
                     IN(relais1).toggle();
                     IN(relais2).toggle();
                     IN(button3).value().from("down_gesture");
                 } else {
                     IN(relais3).toggle();
                 }
             }
             lastb3 = current;
         }
     });

void all_off() {
    IN(relais1).off();
    IN(relais2).off();
    IN(relais3).off();
}

void start() {
    do_later(500, all_off);
}
