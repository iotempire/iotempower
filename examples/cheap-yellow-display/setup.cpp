const char* id="01";

rgb_single(led, ONBOARDLED_RED, ONBOARDLED_GREEN, ONBOARDLED_BLUE, true).off();
button(b0, FLASHBUTTON, "pressed", "released").inverted().debounce(10);
pwm(brightness,BACKLIGHT,5000).set(512);
cyd_display(console, 2, 270);
analog(cover, LDR).filter_binarize(10,"covered","open");

void start() {
    analogSetAttenuation(ADC_0db); // pretty esp32 specific, TODO: make available directly in IoTempower?
	do_later(100, [] () {
		IN(console).print("This is the cyd-")
			.println(id);
	});
}
