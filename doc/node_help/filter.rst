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
