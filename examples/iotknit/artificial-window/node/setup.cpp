rgb_strip(strip1, 90, WS2811, D2, BRG);
rgb_matrix_(matrix, "matrix", 25, 2)
        .with(strip1, 0, 9, Right_Up, 9);
// TODO: animations like in raspberry pi python program
