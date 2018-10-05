led(green, GREENLED, "off", "on").set("off").with_report_change(false);
output(relais1, RELAIS1).off();
input(button1, BUTTON1, "released", "pressed")
    .with_threshold(3)
    .with_on_change_callback([&] {
         if(IN(button1).is("pressed")) {
             IN(relais1).toggle();
             IN(green).toggle();
         }
     });
