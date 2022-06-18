led(green, GREENLED).inverted().report_change(false);
output(relais1, RELAIS1).off();
input(button1, BUTTON1, "released", "pressed")
    .debounce(3)
    .on_change([] (Device& dev) {
         if(dev.is("pressed")) {
             IN(relais1).toggle();
             IN(green).toggle();
         }
         return true;
     });
