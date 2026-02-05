do_later
========

``do_later`` schedules a callback for later execution without blocking the main loop.
The callback is executed in the next loop cycle after the delay has elapsed.

With id (replaceable)
--------------------

**syntax:** ``do_later(delay_ms, id, callback)``

When you provide an ``id``, a new call replaces any already scheduled callback with
the same id. This is ideal for debouncing or ensuring only one pending action exists.

Example (from ``examples/water-sensor``):

..  code-block:: cpp

    void blink(int id) {
        IN(blue).toggle();
        blinks_left--;
        if(blinks_left > 0) {
            do_later(500, id, blink);  // reschedule same id
        } else {
            IN(blue).set("off");
        }
    }

    if(IN(a0).value().equals("wet")) {
        blinks_left = 10;
        do_later(100, 1234, blink);
    }

Without id (one-shot)
---------------------

**syntax:** ``do_later(delay_ms, callback)``

Without an id, each call creates a new scheduled callback.

Example (from ``examples/m5stickc/basic-with-screen``):

..  code-block:: cpp

    void start() {
        do_later(100, [] () {
            IN(console).print("This is: stick-").print(id);
        });
    }

Canceling scheduled callbacks
-----------------------------

**cancel one:** ``do_later_cancel(id)``

Cancels the pending callback for the given ``id`` so it will not be executed.
Returns ``true`` when a scheduled callback was found and removed.

**cancel all:** ``do_later_cancel_all()``

Clears all pending callbacks, including those scheduled without an id.

Example:

..  code-block:: cpp

    const int blink_id = 42;
    do_later(5000, blink_id, blink);
    do_later_cancel(blink_id);
    do_later_cancel_all();
