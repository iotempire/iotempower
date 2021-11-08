input
=====

..  code-block:: cpp

    input(name, pin, "high_word", "low_word")
      [.with_debounce(debounce)]
      [.with_pull_up(true/false)]
      [.inverted()]
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

- ``debounce``: debouncing value (higher -> average more measurements)

- ``inverted``: if called makes the button inverted (use high as low and low as high).

- ``filter_function``: specify a preprocessor for measured values
  (`filter <filter.rst>`_).

Example
-------

**node name:** ``living room/leds1``

..  code-block:: cpp
    
    input(lower, D2, "depressed", "pressed").with_debounce(5);

Now the status button is published as ``living room/leds1/lower``.
