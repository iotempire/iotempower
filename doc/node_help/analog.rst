analog
======


..  code-block:: cpp

    analog(name)
      [.with_precision(precision)]
      [.with_threshold(threshold_value, "high_word", "low_word")]
      [.with_filter(filter_function)]
      ;

Create a new analog sensor. By default it reports all value changes.
An analog value is between 0 an 1023. If ``precision`` is ``> 1``,
report only changes
with a delta/difference to the last reported value with at least precision.
Setting threshold to a value from 0 to 1023 triggers sending ``high_word``,
when the sensor reads this threshold value or a higher value, and ``low_word``
when reading a lower value.

A ``filter_function`` can be specified
to modify and pre-process values right after
they have been read from the analog port to for example buffer some values
and build an average, median, or even implement
a (fast) fourier transformation.

Parameters
----------

- ``name``: the name it can be addressed via MQTT in the network. Inside the code
  it can be addressed via IN(name).

- ``precision``: report only changes with at least this delta.

- ``threshold``: turn into digital sensor, sending high_word when threshold is 
  reached and low_word if not.

- ``filter_function``: specify a preprocessor for measured values
  (see `filter <filter.rst>`_).

Example
-------

Assuming ``nodename`` is ``basement/water-sensor01``.

..  code-block:: cpp

    analog(water).with_precision(10).with_threshold(100, "wet", "dry");

Now, a value of ``wet`` is sent to ``basement/water-sensor01/water``,
when the analog
port reads a value of more than 90 (threshold - precision). It will start
sending dry again, when it reads a value<=90, this could though be 80 due to
only updating values, when changed by amount of precision.
