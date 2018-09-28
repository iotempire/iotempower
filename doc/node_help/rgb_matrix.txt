TODO
So far just an example:

/* setup.cpp */

// RGB strips
rgb_strip(strip1, 50, WS2811, D3, BRG);
rgb_strip(strip2, 50, WS2811, D5, BRG);
rgb_strip(strip3, 50, WS2811, D4, BRG);
rgb_strip(strip4, 50, WS2811, D1, BRG);

// A matrix consisting of several strips
rgb_matrix(matrix, 25, 4)
        .with(IN(strip1), 0, 0, Right_Down, 25)
        .with(IN(strip2), 0, 1, Right_Down, 25)
        .with(IN(strip3), 0, 2, Right_Down, 25)
        .with(IN(strip4), 0, 3, Right_Down, 25);

