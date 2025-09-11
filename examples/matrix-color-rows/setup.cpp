/* setup.cpp */

const int rows=8;
const int cols=32;
const int panels=3;

// Global animation variables
unsigned long frames[rows] = {0, 0, 0, 0, 0, 0, 0};
ICRGB dest_colors[rows];
enum anim_type {none, fade_to, scroll};
anim_type anim_types[rows] = {none, none, none, none, none, none, none, none};

// RGB strips using NeoPixelBus (interrupt-resistant)
rgb_strip_bus(strip, cols*rows*panels, F_GRB, NeoEsp32I2s0Ws2812xMethod, 26);

rgb_matrix(matrix, cols*panels, rows)
    .with(IN(strip), 0, 0, Down_Right, rows);

// // Helper function to draw patterns
// void draw_pattern(int pattern, int row) {
//     switch(pattern) {
//         case 1:
//             IN(matrix).rainbow_row(0, row, 25, 1);
//             break;
//         case 2:
//             IN(matrix).gradient_row(ICRGB::Green, ICRGB::Blue, 0, row, 25, 1);
//             break;
//         case 3:
//             IN(matrix).gradient_row(ICRGB::Blue, ICRGB::Red, 0, row, 25, 1);
//             break;
//     }
// }

animator(anim)
    .with_fps(10)
    .with_frame_builder([] {
        for(int i = 0; i < rows; i++) {
            if(frames[i] > 0 && anim_types[i] != none) {
                switch(anim_types[i]) {
                    case fade_to:
                        IN(matrix).fade_to(dest_colors[i], 16, 0, i, -1, 1);
                        break;
                    case scroll:
                        IN(matrix).scroll_right(false, 0, i, -1, 1);
                        break;
                }
                frames[i]--;
            }
        }
    })
    .with_command_handler("fade_to", [] (Ustring& command) {
        int row = command.as_int() - 1;
        if(row >= 0 && row < rows) {
            anim_types[row] = fade_to;
            frames[row] = 50;
        }
        command.strip_param(); // this is now the color
        IN(matrix).read_color(command, dest_colors[row]);
    })
    .with_command_handler("scroll", [] (Ustring& command) {
        int row = command.as_int() - 1;
        if(row >= 0 && row < rows) {
            anim_types[row] = scroll;
            frames[row] = 150;
        }
    });
