TODO
So far just an example:

..  code-block:: cpp

    /* setup.cpp */

    //////// Global Variables ////////
    unsigned long frames[4]={0,0,0,0};
    enum anim_type {none, fade, fade_to, scroll};
    anim_type anim_types[4]={none, none, none, none};


    //////// Device setup  ////////
    // RGB strips
    rgb_strip(strip1, 50, WS2811, D3, BRG);
    rgb_strip(strip2, 50, WS2811, D5, BRG);
    rgb_strip(strip3, 50, WS2811, D4, BRG);
    rgb_strip(strip4, 50, WS2811, D1, BRG);

    // A matrix consisting of several strips
    rgb_matrix(matrix, "matrix", 25, 4)
            .with(IN(strip1), 0, 0, Right_Down, 25)
            .with(IN(strip2), 0, 1, Right_Down, 25)
            .with(IN(strip3), 0, 2, Right_Down, 25)
            .with(IN(strip4), 0, 3, Right_Down, 25);

    // An animator object with support functions (this one animates the matrix)
    void draw_pattern(int p, int line, int len) {
        switch(p) {
            case 1:
                IN(matrix).rainbow(0, line, len, 1);
                break;
            case 2:
                IN(matrix).gradient_row(CRGB::Green, CRGB::Blue, 0, line, len, 1);
                break;
            case 3:
                IN(matrix).gradient_row(CRGB::Blue, CRGB::Red, 0, line, len, 1);
                break;
            default:
                break;
        }
    }

    void set_animation(Ustring& command, anim_type at, int leds, int frame_count) {
        int stripnr = limit(command.as_int() - 1, 0, 3);
        command.strip_param();
        int pattern = limit(command.as_int(), 1, 3);
        draw_pattern(matrix, pattern, stripnr, -1);
        anim_types[stripnr] = at;
        frames[stripnr] = frame_count;
    }

    animator(anim, IN(matrix))
        .with_fps(10)
        .with_frame_builder( [] () {
            for(int i=0; i<4; i++) {
                if(frames[i]>0 && anim_types[i] != none) {
                    switch(anim_types[i]) {
                        case fade:
                            IN(matrix).fade(8, 0, i, -1, 1);
                            break;
                        case scroll:
                            IN(matrix).scroll_right(false,i);
                            break;
                        case fade_to:
                            IN(matrix).fade_to(CRGB::Red, 16, 0, i, -1, 1);
                        default:
                            break; 
                    }
                    frames[i] --;
                }
            }
        } ).with_command_handler( "fade", [] (Ustring& command) {
            set_animation(command, fade, -1, 100);
        } ).with_command_handler( "fade_to", [] (Ustring& command) {
            set_animation(command, fade_to, -1, 50);
        } ).with_command_handler( "scroll", [] (Ustring& command) {
            set_animation(command, scroll, 5, 150);
        } );


// My example from ledpanel
/* setup.cpp
*/
// Animation test for single LED strip
// You can send to leds/rgb/set colors
// and send to anim rainbow (set rainbow colors on the strip), blue_red (LEDs go from blue to red), green_blue,
// scroll (makes a scroll animation), and fade_out (fades all LEDs to off) 

const int num_leds = 64; // Here you define the number of LEDs in your single strip. Careful some strips are triplets (so divide by three)

enum strip_anim_type {strip_none, scroll, fade_out};
strip_anim_type strip_atype = strip_none;
int strip_frames = 0;



//rgb_strip(leds, num_leds, WS2812, 26, GRB);
// rgb_strip_bus(leds, num_leds, F_BRG, NeoEsp32I2s1X8Ws2811Method, 2).report_change(false); // works
//rgb_strip_bus(leds, num_leds, F_BRG, NeoEsp32I2s1X8Ws2812xMethod, 2).report_change(false); // works
rgb_strip_bus(leds, num_leds, F_GRB, NeoEsp32I2s1X8Ws2812xMethod, 26).report_change(false); // works
rgb_matrix(matrix, IN(leds));
// rgb_matrix(matrix, 8, 8)
        // .with(IN(leds), 0, 0, Right_Down, 8)
        // .with(IN(leds), 8, 8, Left_Down, 8)
        // .with(IN(leds), 16, 2, Right_Down, 8)
        // .with(IN(leds), 24, 24, Left_Down, 8);

animator(anim)
    .with_fps(30)
    .with_frame_builder( [] {
        CRGB eye1=CRGB::Black, eye2 = CRGB::Black;
        CRGB new_color;
        CHSV hsv;
        
        // strip animation
        switch(strip_atype) {
                        case scroll:
                                IN(matrix).scroll_right(true, 0, 0);
                                break;
                        case fade_out:
                                IN(matrix).fade(8, 0, 0);
                                break;
                }
                if(strip_atype != strip_none) {
                        if (strip_frames > 0) strip_frames --;
                        else {
                                strip_atype = strip_none;
                                // keep content IN(matrix).gradient_row(CRGB::Black, CRGB::Black, 2, 0); // off
                        }
                }
    } )
    .with_show( [] { 
        if(strip_atype != strip_none) IN(matrix).show(); 
    } )

    .with_command_handler( "rainbow", [] (Ustring& command) {
        strip_atype = strip_none;
        IN(matrix).rainbow_row(0,0);
        IN(matrix).show();
    } )
    .with_command_handler( "blue_red", [] (Ustring& command) {
        strip_atype = strip_none;
        IN(matrix).gradient_row(CRGB::Blue, CRGB::Red, 0, 0);
        IN(matrix).show();
    } )
    .with_command_handler( "green_red", [] (Ustring& command) {
        strip_atype = strip_none;
        IN(matrix).gradient_row(CRGB::Green, CRGB::Red, 0, 0);
        IN(matrix).show();
    } )
    .with_command_handler( "green_blue", [] (Ustring& command) {
        strip_atype = strip_none;
        IN(matrix).gradient_row(CRGB::Green, CRGB::Blue, 0, 0);
        IN(matrix).show();
    } )
    .with_command_handler( "scroll", [] (Ustring& command) {
                strip_frames = 300;
                strip_atype = scroll;
    } )
    .with_command_handler( "fade_out", [] (Ustring& command) {
                strip_frames = 100;
                strip_atype = fade_out;
    } );

void start() {
//    start_wink();
}
