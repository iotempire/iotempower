rgb_strip_bus(strip1, 90, F_BRG, NeoEsp8266Uart1800KbpsMethod, D4);
rgb_matrix_(matrix, "matrix", 25, 2)
        .with(strip1, 0, 9, Right_Up, 9);
// TODO: animations like in raspberry pi python program
