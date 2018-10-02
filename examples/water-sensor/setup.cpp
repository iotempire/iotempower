int blinks_left=0;

output(blue, ONBOARDLED, "off", "on").set("off").with_report_change(false);

void blink(int id) {
    IN(blue).toggle();
    blinks_left --;
    if(blinks_left>0)
        do_later(500, id, blink);
    else
        IN(blue).set("off");
}

analog(a0)
    .with_precision(5)
    .with_threshold(100, "wet", "dry")
    .with_on_change_callback([&] {
        if(IN(a0).value().equals("wet")) {
            blinks_left = 10;
            do_later(100, 1234, blink);
        }
    });
