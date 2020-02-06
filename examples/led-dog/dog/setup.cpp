// vars for eyes
enum eyes_anim_type {none, wink, color_animation};
eyes_anim_type eyes_atype = none;
CRGB dest_color;
int eyes_frame_counter = 0;
int eyes_frames = 0;
int current_eye = 0;

// vars for tie
enum tie_anim_type {tie_none, scroll, fade_out};
tie_anim_type tie_atype = tie_none;
int tie_frames = 0;

// prototype for wink
void start_wink();

rgb_strip(leds, 13, WS2812, D3, GRB);
rgb_matrix(matrix, IN(leds));

animator(anim)
    .with_fps(30)
    .with_frame_builder( [&] {
        CRGB eye1=CRGB::Black, eye2 = CRGB::Black;
        CRGB new_color;
        CHSV hsv;
        switch(eyes_atype) {
            case wink:
                if(eyes_frame_counter < eyes_frames/2) { // increase brightness
                    new_color = blend( CRGB::Black, dest_color, 
						eyes_frame_counter * 255 / (eyes_frames / 2 - 1));
                } else {
                    new_color = blend( CRGB::Black, dest_color, 
						(eyes_frames - 1 - eyes_frame_counter) * 255 
						/ (eyes_frames/2-1));
                }
                if(current_eye == 0) {
                    eye1 = new_color;
                } else {
                    eye2 = new_color;
                }
                break;
            case color_animation:
                hsv.hue = eyes_frame_counter%256;
                hsv.val = (eyes_frames - eyes_frame_counter > 125) ?
						255 : (eyes_frames - eyes_frame_counter)*2;
                hsv.sat = 240;
                eye1 = hsv;
                hsv.hue += 128;
                eye2 = hsv;
                break;
            default:
                break;
        }
		if(eyes_atype != none) {
            IN(leds).set_color(0, eye1, false);
            IN(leds).set_color(1, eye2, false);
            eyes_frame_counter ++;
            if(eyes_frame_counter >= eyes_frames) {
	    	IN(leds).show(); // show last frame
                eyes_atype = none;
            }
        }
        
        // tie animation
        switch(tie_atype) {
			case scroll:
				IN(matrix).scroll_right(true, 2, 0);
				break;
			case fade_out:
				IN(matrix).fade(8, 2, 0);
				break;
		}
		if(tie_atype != tie_none) {
			if (tie_frames > 0) tie_frames --;
			else {
				tie_atype = tie_none;
				// keep content IN(matrix).gradient_row(CRGB::Black, CRGB::Black, 2, 0); // off
			}
		}
    } )
    .with_show( [&] { 
    	if(eyes_atype != none || tie_atype != tie_none) IN(matrix).show(); 
    } )
    .with_command_handler( "colors", [&] (Ustring& command) {
        eyes_atype = color_animation;
        eyes_frame_counter = 0;
        eyes_frames = 300;
    } )
    .with_command_handler( "wink", [&] (Ustring& command) {
        start_wink();
    } )
    .with_command_handler( "rainbow", [&] (Ustring& command) {
        tie_atype = tie_none;
        IN(matrix).rainbow_row(2,0);
        IN(matrix).show();
    } )
    .with_command_handler( "blue_red", [&] (Ustring& command) {
        tie_atype = tie_none;
        IN(matrix).gradient_row(CRGB::Blue, CRGB::Red, 2, 0);
        IN(matrix).show();
    } )
    .with_command_handler( "green_blue", [&] (Ustring& command) {
        tie_atype = tie_none;
        IN(matrix).gradient_row(CRGB::Green, CRGB::Blue, 2, 0);
        IN(matrix).show();
    } )
    .with_command_handler( "scroll", [&] (Ustring& command) {
		tie_frames = 300;
		tie_atype = scroll;
    } )
    .with_command_handler( "fade_out", [&] (Ustring& command) {
		tie_frames = 100;
		tie_atype = fade_out;
    } );


void start_wink() {
    if(eyes_atype==none) { // skip if other animation running
        current_eye = random(2);
        eyes_atype = wink;
        switch(random(6)) {
            case 0: dest_color = CRGB::Blue;
                break;
            case 1: dest_color = CRGB::Red;
                break;
            case 2: dest_color = CRGB::Purple;
                break;
            case 3: dest_color = CRGB::Green;
                break;
            case 4: dest_color = CRGB::Yellow;
                break;
            default: dest_color = CRGB::White;
            break;
        }
        eyes_frame_counter = 0;
        eyes_frames = 30;
    }
//    do_later( 60000+random(1,30000), start_wink );
}


void start() {
//    start_wink();
}
