filter
======

Any device in IoTempower can take a filter. A filter can change or even compress
measured values and even prevent them from being sent.

Filters are applied directly on devices using method chaining in your setup.cpp.

Example for custom filter-function:

..  code-block:: cpp

    analog(a0).filter( [] (Device& dev) {
        const int buflen = 100;
        static long sum = 0;
        static long values_count = 0;
        int v = dev.read_int();
        sum += v;
        values_count ++;
        if(values_count >= buflen) {
            dev.value().from(sum / values_count);
            values_count = 0;
            sum = 0;
            return true;
        }
        return false;
    });

Several filters can be chained and are executed in order.


Filtering Full Values with mark_field
--------------------------------------
If a device has several subdevices (like temperature and humidity in the dht sensors)
you can use `mark_field` and `commit_mark` to select on which value you want to filter
marking fields for filtering. This approach works on the complete value (not individual CSV fields)
and is easier than CSV field marking. If you build your own filter the marked values can be accessed
via the MARKED(name_of_your_mark) macro.



Basic workflow:

1. **mark_field(name, subdevice)** - Mark the full value for a subdevice
2. **filter_*(name, ...)** - Apply filter(s) to the marked value
3. **commit_mark(name, threshold, absolute)** - Finalize with change detection

Example with temperature sensor:

..  code-block:: cpp

    // Apply median filter to humidity readings (three subdevices: 0=status, 1=temp, 2=hum)
    dht22(climate, D4)
        .mark_field(hum, 2)              // Mark value from subdevice 2
        .filter_jmc_median(hum)          // Apply median filter
        .commit_mark(hum, 2);   // commit value back to value(2), do not clear others

Example with analog sensor:

..  code-block:: cpp

    // Apply averaging to analog readings
    analog(light, A0)
        .mark_field(brightness, 0)        // Mark the analog value
        .filter_average(brightness, 20)   // Average over 20 samples
        .commit_mark(brightness, 0); // commit value back to value(0) (no other values to clear)

The `commit_mark` parameters:

- **name**: Marked value name
- **subdevice value number**: Subdevice index to commit the value to
- **clear others**: If true, clear all other subdevice values; if false or skipped, keep them

Filtering CSV Fields
---------------------

For devices that output multiple values as CSV (comma-separated values), you can mark
specific fields and apply filters to them individually.

Example with IMU sensor returning CSV data:

..  code-block:: cpp

    // IMU outputs: "gx,gy,gz" (gyroscope x, y, z values)
    m5stickc_imu(imu, true, true, false, false)
        .mark_csv_field(zacc, 2, 1)         // Mark field 2 (z-axis), subdevice 1
        .filter_average(zacc, 10)           // Apply averaging to z-axis
        .commit_mark(zacc, 2, true);        // Write back to value(2) and clear others

The workflow is:

1. **mark_csv_field(name, field_index, subdevice)** - Mark a CSV field for filtering
2. **filter_*(name, ...)** - Apply filter(s) to the marked field  
3. **commit_mark(name, threshold, absolute)** - Finalize the mark with change detection


Committing Multiple Marked Fields as CSV
-----------------------------------------

**filter:** ``commit_csv_marks(value_index, marker1, marker2, ...)``

**description:** Combines multiple marked values into a single CSV string and writes 
it to the specified value index. This is useful when you want to process several 
fields independently with filters and then recombine them into a CSV output.

The macro accepts:

- **value_index**: The subdevice value index to write the CSV result to
- **marker1, marker2, ...**: Names of previously marked fields (up to 10 markers)

**example:**

..  code-block:: cpp

    const int gyro_value_index = 0;
    m5stickc_imu(imu, true, true, false, false)
        .mark_csv_field(xgyro, 0, gyro_value_index)  // up and down is X
        .mark_csv_field(zgyro, 2, gyro_value_index)  // left and right is Z
        .mark_csv_field(yacc, 1, 1)                  // push is Y
        
        .filter_moving_median(xgyro, 20)
        .filter_interval_map_m(xgyro, "d", -180, "x", 180, "u")  // up down
        .filter_moving_median(yacc, 20)
        .filter_interval_map_m(yacc, "b", -1.2, "x", 1.2, "f")   // forward back
        .filter_moving_median(zgyro, 20)
        .filter_interval_map_m(zgyro, "r", -180, "x", 180, "l")  // left right
        
        .commit_csv_marks(gyro_value_index, xgyro, yacc, zgyro)
        .clear_value(1);  // clear reported accelerations

This example:

1. Marks three fields: x and z gyroscope values, and y acceleration
2. Applies median filtering and interval mapping to each
3. Combines the three filtered values into a CSV string (e.g., "u,f,l")
4. Writes the result to value index 0
5. Clears value index 1 (raw acceleration data)


Clearing Values
---------------

**filter:** ``clear_value(value_index)``

**description:** Clears the value at the specified subdevice index. This is useful 
when you want to suppress certain subdevice outputs while keeping others, typically 
after processing data with `commit_csv_marks` or other filters.

This is similar to the boolean flag in `commit_mark`. When that flag is omitted or 
set to false (keeping other values), `clear_value` can be used to explicitly clear 
specific other values that should not be published.

**parameters:**

- **value_index**: The subdevice value index to clear (0-based)

**example:**

..  code-block:: cpp

    m5stickc_imu(imu, true, true, false, false)
        .mark_csv_field(xgyro, 0, 0)
        .filter_average(xgyro, 10)
        .commit_mark(xgyro, 0)    // Commit without clearing others
        .clear_value(1);          // Explicitly clear acceleration data

This clears the value at index 1 (acceleration subdevice), leaving only the 
filtered gyroscope data at index 0.

**note:** The `clear_value` filter is often used in combination with 
`commit_csv_marks` to clean up intermediate values that were used during 
processing but shouldn't be published.


Pre-defined Filters
-------------------

There are some pre-defined filters available:

-   **filter:** ``filter_average(buflen)`` or ``filter_average(marker, buflen)``
    
    **example:** ``analog(a0).filter_average(100);``
    
    **example with marker:** ``device(name).mark_csv_field(field1, 0, 0).filter_average(field1, 50);``
    
    **description:** Average over buflen values. When used with a marker, filters only that specific CSV field.

-   **filter:** ``filter_sliding_average(buflen)`` or ``filter_sliding_average(marker, buflen)``

    **description:** Sliding window average over buflen values.

-   **filter:** ``filter_moving_median(buflen)`` or ``filter_moving_median(marker, buflen)``

    **description:** Moving median over buflen values.

-   **filter:** ``filter_time_average(time_ms)`` or ``filter_time_average(marker, time_ms)``

    **description:** Time-based averaging over specified milliseconds.

-   **filter:** ``filter_jmc_median()`` or ``filter_jmc_median(marker)``

    **example:** ``analog(a0).filter_jmc_median();``

    **description:** Estimated running median (Jeff McClintock algorithm).

-   **filter:** ``filter_jmc_interval_median(update_ms)``

    **example:** ``analog(a0).filter_jmc_interval_median(500);``

    **description:** Estimated median giving a new value every 500ms.

-   **filter:** ``filter_jmc_interval_median(interval, dev)``

    **description:** Same as above but resets every interval.

-   **filter:** ``filter_minchange(minchange)`` or ``filter_minchange(marker, minchange)``

    **description:** Only report (forward) value if a minimum change to last 
    value happened (also often called precision).

-   **filter:** ``filter_restrict(min, max)`` or ``filter_restrict(marker, min, max)``

    **description:** Restrict values to a specific range (min to max).

-   **filter:** ``filter_binarize(cutoff, high, low)`` or ``filter_binarize(marker, cutoff, high, low)``

    **example:** ``hcsr04(distance, D5, D6).filter_binarize(200, "off", "on");``

    **description:** Turn analog values into binary with a cutoff/threshold level.

-   **filter:** ``filter_round(base)``

    **description:** Round to the next multiple of base.

-   **filter:** ``filter_limit_time(interval)``

    **description:** Return maximum one value per time interval (interval in ms).

-   **filter:** ``filter_detect_click(click_min_ms, click_max_ms,
    longclick_min_ms, longclick_max_ms,
    pressed_str, released_str)``

    **description:** A filter that can detect, clicks, double clicks,
    long clicks and too long clicks. Parameters are usually not necessary as
    it has sane defaults.

-   **filter:** ``filter_interval_map(v0, b0, v1, b1, ..., vn)`` or ``filter_interval_map(marker, v0, b0, v1, b1, ..., vn)``

    **description:** A filter that maps a set of intervals to single
    descrete values. It is always assumed to start at minus infinity and
    is automatically terminated by infinity
 
    **example**:  ``.filter_interval_map("low", -0.5, NULL, 0.5, "high")``
    returns ``low`` for values <=-0.5, nothing between -0.5 and 0.5,
    and ``high`` for values > 0.5


Complete Examples
-----------------

Example 1: Filtering Full Values
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Simple filtering on complete device values:

..  code-block:: cpp

    // Temperature sensor with median filter for humidity
    // dht22 has two subdevices - the first for general status,
    // the second for temperature and the third for humidity
    dht22(climate, D4)
        .mark_field(hum, 2)
        .filter_jmc_median(temp)
        .commit_mark(hum, 2, false); // keep temperature value untouched (true would delete it)
    

Example 2: Filtering CSV Fields
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Filtering specific fields from CSV output:

..  code-block:: cpp

    // Simple devices
    out(led, ONBOARDLED).off();
    button(left, BUTTON_LEFT, "pressed", "released").inverted().debounce(10);
    
    // IMU with CSV output - mark and filter z-acceleration
    m5stickc_imu(imu, true, true, false, false)
        .mark_csv_field(zacc, 2, 1)      // Mark z-axis (field 2) in subdevice 1
        .filter_average(zacc, 10)        // Average over 10 readings
        .commit_mark(zacc, 2, true);     // Commit to value  and clear all others
    
    void start() {
        do_later(100, [] () {
            IN(led).on();  // Turn on LED after 100ms
        });
    }
