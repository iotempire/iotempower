led(blue, ONBOARDLED, "off", "on");

input(up, D1, "0", "1").debounce(5);
input(left, D2, "0", "1").debounce(5);
input(down, D3, "0", "1").debounce(5);
input(right, D6, "0", "1").debounce(5);
input(fire, D7, "0", "1").debounce(5);
