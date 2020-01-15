filter
======

Example for filter-function:

..  code-block:: cpp

    analog(a0).filter( [&] (Device& dev) {
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

There some pre-defined filters available:

-   **filter:** ``filter_average(buflen)``
    
    **example:** ``analog(a0).filter_average(100);``
    
    **description:** That's the same as above.

-   **filter:** ``filter_jmc_median(update_ms)``

    **example:** ``analog(a0).filter_jmc_median(500);``

    **description:** That's an estimated median giving a new value every 500ms.
    (The Jeff McClintock running median estimate.)

-   **filter:** ``filter_jmc_interval_median(interval, dev)``

    **description:** Same as above but resets every interval

-   **filter:** ``filter_minchange(minchange)``

    **description:** Only report (forward) value if a minimum change to last 
    value happened (also often called precision)

-   **filter:** ``filter_binarize(cutoff, high, low)``

    **description:** Turn analog values into binary with a cutoff/threshold level.

-   **filter:** ``filter_round(base)``

    **description:** Round to the next multiple of base.

-   **filter:** ``filter_limit_time(interval)``

    **description:** Return maximum one value per time interval (interval in ms).

-   **filter:** ``filter_detect_click(click_min_ms, click_max_ms,
    longclick_min_ms, longclick_max_ms=2500,
    pressed_str, released_str)``

    **description:** A filter that can detect, clicks, double clicks,
    long clicks and too long clicks. Parameter are usually not necessary as
    it has sane defaults.
