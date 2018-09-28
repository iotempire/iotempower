const char* p="pressed";
const char* r="released";
button_(lower, D0, r, p).with_pull_up(false);
button_(left, D3, r, p);
button_(right, D6, r, p);

led_(blue, ONBOARDLED,"off","on").set("off");
led_(red, D7);
led_(yellow, D8);

dht_(th, D1);

display_(dp1, font_medium).i2c(D2, D5);
