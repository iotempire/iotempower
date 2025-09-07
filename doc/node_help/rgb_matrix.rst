rgb_matrix
==========

The ``rgb_matrix`` device allows you to control a matrix or array of RGB LEDs by combining multiple RGB strips. It provides convenient methods for manipulating LEDs in a 2D grid pattern and supports various animation effects.

Overview
--------

An RGB matrix can be created from one or more RGB strips arranged in a grid pattern. The matrix provides high-level functions for setting colors, creating gradients, animations, and effects across the entire matrix or specific regions.

Syntax
------

.. code-block:: cpp

    rgb_matrix(matrix_name, width, height)
        .with(strip_reference, start_x, start_y, direction, length);

    // Alternative: single strip matrix
    rgb_matrix(matrix_name, strip_reference);

Parameters
----------

- ``name``: Unique identifier for the matrix
- ``width``: Number of LEDs horizontally  
- ``height``: Number of LEDs vertically
- ``strip_reference``: Reference to an RGB strip (use ``IN(strip_name)``)
- ``start_x``, ``start_y``: Starting position in the matrix
- ``direction``: LED arrangement direction (``Right_Down``, ``Left_Down``, ``Right_Up``, ``Left_Up``)
- ``length``: Number of LEDs from the strip to use

Common Methods
--------------

Color Setting
~~~~~~~~~~~~~

.. code-block:: cpp

    IN(matrix_name).set_color(x, y, color, show_immediately);  // Set single LED
    IN(matrix_name).fill(color, show_immediately);             // Fill entire matrix
    IN(matrix_name).show();                                    // Update display

Gradients
~~~~~~~~~

.. code-block:: cpp

    IN(matrix_name).gradient_row(color1, color2, start_x, start_y, width, height);
    IN(matrix_name).rainbow_row(start_x, start_y, width, height);

Animation Effects
~~~~~~~~~~~~~~~~~

.. code-block:: cpp

    IN(matrix_name).fade(amount, start_x, start_y, width, height);
    IN(matrix_name).fade_to(target_color, amount, start_x, start_y, width, height);
    IN(matrix_name).scroll_right(wrap, start_x, start_y, width, height);
    IN(matrix_name).scroll_left(wrap, start_x, start_y, width, height);

Examples
--------

Multi-Strip Matrix
~~~~~~~~~~~~~~~~~~

This example creates a 25×4 matrix from four separate RGB strips:

.. code-block:: cpp

    /* setup.cpp */

    // RGB strips
    rgb_strip(strip1, 50, WS2811, D3, BRG);
    rgb_strip(strip2, 50, WS2811, D5, BRG);
    rgb_strip(strip3, 50, WS2811, D4, BRG);
    rgb_strip(strip4, 50, WS2811, D1, BRG);

    // A matrix consisting of several strips
    rgb_matrix(matrix_name, 25, 4)
        .with(IN(strip1), 0, 0, Right_Down, 25)
        .with(IN(strip2), 0, 1, Right_Down, 25)
        .with(IN(strip3), 0, 2, Right_Down, 25)
        .with(IN(strip4), 0, 3, Right_Down, 25);

Single Strip Matrix
~~~~~~~~~~~~~~~~~~~

For a linear arrangement or single strip used as a virtual matrix:

.. code-block:: cpp

    /* setup.cpp */

    const int num_leds = 64;

    // Single RGB strip 
    rgb_strip_bus(leds, num_leds, F_GRB, NeoEsp8266Uart1800KbpsMethod, D4);
    
    // Create matrix from single strip
    rgb_matrix(matrix_name, IN(leds));

    void start() {
        // Set gradient across the strip
        IN(matrix_name).gradient_row(ICRGB::Blue, ICRGB::Red, 0, 0);
        IN(matrix_name).show();
    }

Matrix with Color Effects
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

    /* setup.cpp */

    rgb_strip_bus(leds, 90, F_GRB, NeoEsp8266Uart1800KbpsMethod, D2);
    rgb_matrix(matrix_name, 25, 2)
        .with(IN(leds), 0, 0, Right_Up, 25);

    void start() {
        // Create rainbow pattern
        IN(matrix_name).rainbow_row(0, 0);
        
        // Create blue to red gradient
        IN(matrix_name).gradient_row(ICRGB::Blue, ICRGB::Red, 0, 1);
        
        IN(matrix_name).show();
    }

Color Types
-----------

The matrix works with the ICRGB color system. Common colors include:

.. code-block:: cpp

    ICRGB::Black, ICRGB::White, ICRGB::Red, ICRGB::Green, ICRGB::Blue
    ICRGB::Yellow, ICRGB::Cyan, ICRGB::Magenta, ICRGB::Purple
    ICRGB::Orange, ICRGB::Pink, ICRGB::Brown, ICRGB::Gold

You can also create custom colors:

.. code-block:: cpp

    ICRGB custom_color(255, 128, 64);  // RGB values
    ICRGB from_hex(0xFF8040);          // Hex color code

MQTT Commands
-------------

RGB matrices automatically respond to MQTT commands:

- ``<node>/<matrix_name>/rgb/set`` - Set specific colors
- Works with animator command handlers for dynamic control

See Also
--------

- :doc:`rgb_strip` - Individual RGB strip control
- :doc:`animator` - Animation framework for matrices  
- :doc:`rgb_strip_bus` - High-performance RGB strips

