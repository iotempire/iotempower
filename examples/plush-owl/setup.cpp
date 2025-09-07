enum anim_type {none, wink, color_animation};
ICRGB dest_color;
int frame_counter=0;
int frames=0;
int current_eye=0;

anim_type atype;

rgb_strip(eyes, 2, WS2812, D2, GRB);
animator(anim)
    .with_fps(30)
    .with_frame_builder( [] {
        ICRGB eye1=ICRGB::Black, eye2 = ICRGB::Black;
        ICRGB new_color;
        ICHSV hsv;
        switch(atype) {
            case wink:
                if(frame_counter<frames/2) { // increase brightness
                    new_color = blend( ICRGB::Black, dest_color, frame_counter * 255 / (frames/2-1));
                } else {
                    new_color = blend( ICRGB::Black, dest_color, (frames-1-frame_counter) * 255 / (frames/2-1));
                }
                if(current_eye == 0) {
                    eye1 = new_color;
                } else {
                    eye2 = new_color;
                }
                break;
            case color_animation:
                hsv.hue = frame_counter%256;
                hsv.val = (frames-frame_counter>125)?255:(frames-frame_counter)*2;
                hsv.sat = 240;
                eye1 = hsv;
                hsv.hue += 128;
                eye2 = hsv;
                break;
            default:
                break;
        }
        IN(eyes).set_color(0, eye1, false);
        IN(eyes).set_color(1, eye2, false);
        frame_counter ++;
        if(frame_counter >= frames) {
            atype = none;
        }
    } )
    .with_show( [] { IN(eyes).show(); })
    .with_command_handler( "colors", [] (Ustring& command) {
        atype = color_animation;
        frame_counter = 0;
        frames = 300;
    } );

void start_wink() {
    if(atype==none) { // skip if other animation running
        current_eye = random(0,2);
        atype = wink;
        switch(random(0,6)) {
            case 0: dest_color = ICRGB::Blue;
                break;
            case 1: dest_color = ICRGB::Red;
                break;
            case 2: dest_color = ICRGB::Purple;
                break;
            case 3: dest_color = ICRGB::Green;
                break;
            default: dest_color = ICRGB::White;
            break;
        }
        frame_counter = 0;
        frames = 30;
    }
    do_later( 60000+random(1,30000), start_wink );
}


void start() {
    start_wink();
}
