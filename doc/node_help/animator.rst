animator
========

The ``animator`` device provides a powerful framework for creating frame-based animations with RGB matrices and strips. It manages timing, frame updates, and MQTT command handling for complex LED animations and effects.

Overview
--------

The animator works by calling a frame builder function at regular intervals (defined by FPS), allowing you to create smooth animations by updating LED states frame by frame. It also supports custom MQTT command handlers for interactive control.

Key Features:

- Frame-based animation with configurable FPS
- Custom frame builder functions
- MQTT command handlers for interactive control  
- Integration with RGB matrices and strips
- Support for complex animation states and transitions

Syntax
------

.. code-block:: cpp

    animator(name)
        .with_fps(frames_per_second)
        .with_frame_builder(frame_function)
        .with_show(show_function)
        .with_command_handler("command_name", handler_function);

Components
----------

Frame Builder
~~~~~~~~~~~~~

The frame builder function is called every frame and contains the animation logic:

.. code-block:: cpp

    .with_frame_builder([] {
        // Animation logic here
        // Update LED colors, positions, effects
        // Manage animation state variables
    })

Show Function
~~~~~~~~~~~~~

Optional function to control when LEDs are actually updated:

.. code-block:: cpp

    .with_show([] { 
        IN(matrix_name).show(); // Update display
    })

Command Handlers
~~~~~~~~~~~~~~~~

Add MQTT command handlers for interactive control:

.. code-block:: cpp

    .with_command_handler("command_name", [] (Ustring& command) {
        // Handle MQTT command
        // Parse parameters from command string
    })

Animation Patterns
------------------

Simple LED Animation
~~~~~~~~~~~~~~~~~~~~

This example shows a basic winking eye animation:

.. code-block:: cpp

    /* setup.cpp */

    enum anim_type {none, wink, color_animation};
    ICRGB dest_color;
    int frame_counter = 0;
    int frames = 0;
    int current_eye = 0;
    anim_type atype = none;

    rgb_strip_bus(eyes, 2, F_GRB, NeoEsp8266Uart1800KbpsMethod, D4);

    animator(anim)
        .with_fps(30)
        .with_frame_builder([] {
            ICRGB eye1 = ICRGB::Black, eye2 = ICRGB::Black;
            ICRGB new_color;
            
            switch(atype) {
                case wink:
                    if(frame_counter < frames/2) {
                        // Fade in
                        new_color = blend(ICRGB::Black, dest_color, 
                                        frame_counter * 255 / (frames/2-1));
                    } else {
                        // Fade out  
                        new_color = blend(ICRGB::Black, dest_color, 
                                        (frames-1-frame_counter) * 255 / (frames/2-1));
                    }
                    
                    if(current_eye == 0) {
                        eye1 = new_color;
                    } else {
                        eye2 = new_color;
                    }
                    break;
                    
                case color_animation:
                    ICHSV hsv;
                    hsv.hue = frame_counter % 256;
                    hsv.val = (frames-frame_counter > 125) ? 255 : (frames-frame_counter)*2;
                    hsv.sat = 240;
                    eye1 = hsv;
                    hsv.hue += 128;
                    eye2 = hsv;
                    break;
            }
            
            IN(eyes).set_color(0, eye1, false);
            IN(eyes).set_color(1, eye2, false);
            
            frame_counter++;
            if(frame_counter >= frames) {
                atype = none;
            }
        })
        .with_show([] { IN(eyes).show(); })
        .with_command_handler("colors", [] (Ustring& command) {
            atype = color_animation;
            frame_counter = 0;
            frames = 300;
        });

    void start_wink() {
        if(atype == none) {
            current_eye = random(2);
            atype = wink;
            dest_color = ICRGB::Blue; // or any color
            frame_counter = 0;
            frames = 30;
        }
    }

Strip Animation with Matrix
~~~~~~~~~~~~~~~~~~~~~~~~~~~

More complex animation using matrix effects:

.. code-block:: cpp

    /* setup.cpp */

    const int num_leds = 64;
    enum strip_anim_type {strip_none, scroll, fade_out};
    strip_anim_type strip_atype = strip_none;
    int strip_frames = 0;

    rgb_strip_bus(leds, num_leds, F_GRB, NeoEsp8266Uart1800KbpsMethod, D4);
    rgb_matrix(matrix, IN(leds));

    animator(anim)
        .with_fps(30)
        .with_frame_builder([] {
            switch(strip_atype) {
                case scroll:
                    IN(matrix).scroll_right(true, 0, 0);
                    break;
                case fade_out:
                    IN(matrix).fade(8, 0, 0);
                    break;
            }
            
            if(strip_atype != strip_none) {
                if (strip_frames > 0) {
                    strip_frames--;
                } else {
                    strip_atype = strip_none;
                }
            }
        })
        .with_show([] { 
            if(strip_atype != strip_none) IN(matrix).show(); 
        })
        .with_command_handler("rainbow", [] (Ustring& command) {
            strip_atype = strip_none;
            IN(matrix).rainbow_row(0, 0);
            IN(matrix).show();
        })
        .with_command_handler("blue_red", [] (Ustring& command) {
            strip_atype = strip_none;
            IN(matrix).gradient_row(ICRGB::Blue, ICRGB::Red, 0, 0);
            IN(matrix).show();
        })
        .with_command_handler("scroll", [] (Ustring& command) {
            strip_frames = 300;
            strip_atype = scroll;
        })
        .with_command_handler("fade_out", [] (Ustring& command) {
            strip_frames = 100;
            strip_atype = fade_out;
        });

Multi-Strip Matrix Animation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Advanced example with multiple animation states:

.. code-block:: cpp

    /* setup.cpp */

    // Global animation variables
    unsigned long frames[4] = {0, 0, 0, 0};
    enum anim_type {none, fade, fade_to, scroll};
    anim_type anim_types[4] = {none, none, none, none};

    // RGB strips using NeoPixelBus (interrupt-resistant)
    rgb_strip_bus(strip1, 50, F_BRG, NeoEsp32I2s0Ws2812xMethod, 16);
    rgb_strip_bus(strip2, 50, F_BRG, NeoEsp32I2s0Ws2812xMethod, 17);
    rgb_strip_bus(strip3, 50, F_BRG, NeoEsp32I2s0Ws2812xMethod, 21);
    rgb_strip_bus(strip4, 50, F_BRG, NeoEsp32I2s0Ws2812xMethod, 22);

    rgb_matrix(matrix, 25, 4)
        .with(IN(strip1), 0, 0, Right_Down, 25)
        .with(IN(strip2), 0, 1, Right_Down, 25)
        .with(IN(strip3), 0, 2, Right_Down, 25)
        .with(IN(strip4), 0, 3, Right_Down, 25);

    // Helper function to draw patterns
    void draw_pattern(int pattern, int row) {
        switch(pattern) {
            case 1:
                IN(matrix).rainbow_row(0, row, 25, 1);
                break;
            case 2:
                IN(matrix).gradient_row(ICRGB::Green, ICRGB::Blue, 0, row, 25, 1);
                break;
            case 3:
                IN(matrix).gradient_row(ICRGB::Blue, ICRGB::Red, 0, row, 25, 1);
                break;
        }
    }

    animator(anim)
        .with_fps(10)
        .with_frame_builder([] {
            for(int i = 0; i < 4; i++) {
                if(frames[i] > 0 && anim_types[i] != none) {
                    switch(anim_types[i]) {
                        case fade:
                            IN(matrix).fade(8, 0, i, 25, 1);
                            break;
                        case scroll:
                            IN(matrix).scroll_right(false, 0, i, 25, 1);
                            break;
                        case fade_to:
                            IN(matrix).fade_to(ICRGB::Red, 16, 0, i, 25, 1);
                            break;
                    }
                    frames[i]--;
                }
            }
        })
        .with_command_handler("fade", [] (Ustring& command) {
            int row = command.as_int() - 1;
            if(row >= 0 && row < 4) {
                anim_types[row] = fade;
                frames[row] = 100;
            }
        })
        .with_command_handler("scroll", [] (Ustring& command) {
            int row = command.as_int() - 1;
            if(row >= 0 && row < 4) {
                anim_types[row] = scroll;
                frames[row] = 150;
            }
        });

Animation Concepts
------------------

Frame-Based Animation
~~~~~~~~~~~~~~~~~~~~~

Animations work by updating LED states every frame:

1. **Frame Rate**: Set with ``.with_fps(30)`` - determines smoothness
2. **Frame Counter**: Track animation progress with frame counters
3. **Animation State**: Use enums to manage different animation types
4. **Timing**: Use frame counts to control animation duration

Color Transitions
~~~~~~~~~~~~~~~~~

Create smooth color transitions using:

.. code-block:: cpp

    // Linear blend between two colors
    ICRGB result = blend(color1, color2, progress_0_to_255);

    // HSV color cycling
    ICHSV hsv;
    hsv.hue = frame_counter % 256;  // Cycle through hues
    hsv.sat = 240;                  // High saturation
    hsv.val = 255;                  // Full brightness
    ICRGB rgb_color = hsv;          // Convert to RGB

Command Processing
~~~~~~~~~~~~~~~~~~

Command handlers receive MQTT messages:

.. code-block:: cpp

    .with_command_handler("command", [] (Ustring& command) {
        // Parse parameters
        int param1 = command.as_int();     // Get integer
        command.strip_param();              // Move to next parameter
        int param2 = command.as_int();     // Get next integer
        
        // Set animation state
        animation_active = true;
        frame_counter = 0;
    })

MQTT Commands
-------------

Send commands to control animations:

.. code-block:: bash

    # Start color animation
    mqtt_send <node>/<animator>/colors

    # Start scroll animation  
    mqtt_send <node>/<animator>/scroll

    # Set specific patterns
    mqtt_send <node>/<animator>/blue_red
    mqtt_send <node>/<animator>/rainbow

Performance Tips
----------------

1. **Optimize Frame Rate**: Higher FPS = smoother but more CPU intensive
2. **Conditional Updates**: Only call ``show()`` when needed
3. **State Management**: Use enums and flags to avoid unnecessary calculations
4. **Batch Operations**: Group LED updates before calling ``show()``

See Also
--------

- :doc:`rgb_matrix` - Matrix control functions
- :doc:`rgb_strip_bus` - High-performance strips (recommended)
