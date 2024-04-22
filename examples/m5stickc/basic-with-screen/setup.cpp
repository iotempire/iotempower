const char* id="01";

out(led, ONBOARDLED).inverted().off();
button(home, BUTTON_HOME, "pressed", "released").inverted().debounce(10);
button(buttom, BUTTON_RIGHT, "pressed", "released").inverted().debounce(10);
m5stickc_display(console, 1, 270); // why does lcd as name turn into Lcd?

void start() {
        do_later(100, [] () {
                IN(console).print("This is: stick-")
                        .print(id);
        });
}