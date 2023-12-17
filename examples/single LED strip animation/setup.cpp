// Animation test for single LED strip
// You can send to leds/rgb/set colors
// and send to anim rainbow (set rainbow colors on the strip), blue_red (LEDs go from blue to red), green_blue,
// scroll (makes a scroll animation), and fade_out (fades all LEDs to off) 

const int num_leds = 10; // Here you define the number of LEDs in your single strip. Careful some strips are triplets (so divide by three)

enum strip_anim_type {strip_none, scroll, fade_out};
strip_anim_type strip_atype = strip_none;
int strip_frames = 0;



//rgb_strip(leds, 13, WS2812, D3, GRB);
rgb_strip_bus(leds, num_leds, F_BRG, NeoEsp8266Uart1800KbpsMethod, D4).report_change(false);
rgb_matrix(matrix, IN(leds));

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
