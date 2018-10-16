/* Sonoff Double Touch */

led(blue, ONBOARDLED, "off", "on").set("off").with_report_change(false);
output(relais1, RELAIS1).off();
output(relais2, RELAIS2).off();

input(button1, BUTTON1, "released", "pressed")
    .with_threshold(7)
    .with_on_change_callback([&] {
         if(IN(button1).is("pressed")) {
             IN(relais1).toggle();
             IN(blue).toggle();
         }
     });

input(button2, BUTTON2, "released", "pressed")
    .with_threshold(7)
    .with_on_change_callback([&] {
         if(IN(button2).is("pressed")) {
             IN(relais2).toggle();
         }
     });
