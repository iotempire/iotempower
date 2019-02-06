filter
======

Example for filter-function:

..  code-block:: cpp

    analog(a0).with_filter_callback( [&] {
        const int buflen = 100;
        static long sum = 0;
        static long values_count = 0;
        int v=IN(a0).read_int();
        sum += v;
        values_count ++;
        if(values_count >= buflen) {
            dev.measured_value().from(sum/values_count);
            values_count = 0;
            sum = 0;
            return true;
        }
        return false;
    });

There some pre-defined filters available:

-   **filter:** ``filter_average(TYPE, buflen, dev)``
    
    **example:** ``analog(a0).with_filter_callback(filter_average(int, 100, IN(a0)));``
    
    **description:** That's the same as above.

-   **filter:** ``filter_jmc_median(update_ms, dev)``

    **example:** ``analog(a0).with_filter_callback(filter_jmc_median(500, IN(a0)));``

    **description:** That's an estimated median giving a new value every 500ms.
    (The Jeff McClintock running median estimate.)

-   **filter:** ``filter_jmc_interval_median(interval, dev)``

    **description:** Derivation of jmc median over small time intervals

-   **filter:** ``filter_binarize(cutoff, high, low, dev)``

    **description:** Turn analog values into binary with a threshold level.

-   **filter:** ``filter_round(base, dev)``

    **description:** Round to the next multiple of base.

-   **filter:** ``filter_limit_time(interval, dev)``

    **description:** Return maximum one value per time interval (interval in ms).
