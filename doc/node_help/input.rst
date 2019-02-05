input
=====

..  code-block:: cpp

    input(name, pin, "high_word", "low_word")
      [.with_threshold(threshold_value, "high_word", "low_word")]
      [.with_pull_up(true/false)]
      [.with_filter(filter_function)]
      ;

**other names**: button, contact

Create a new input port for a contact or a button.
name will be appended to the mqtt topic and corresponding
value will be published there.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via ``IN(name)``.

- ``pin``: the gpio pin

- ``high_word``: what to send when respective port high (default on)

- ``low_word``: what to send when respective port low (default off)

- ``pull_up``: enable (true) or disable (false) internal pullup (default enabled)

- ``threshold``: debouncing value (higher -> average more measurements)

- ``filter_function``: specify a preprocessor for measured values
  (see uhelp filter).

Example
-------

**node name:** ``living room/leds1``

..  code-block:: cpp
    
    input(lower, D2, "depressed", "pressed")

Now the status button is published as ``living room/leds1/lower``.