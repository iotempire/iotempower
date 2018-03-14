# Device
# author: ulno
# created: 2017-04-08
#
# The device class

class Device(object):
    # on_change: callback to call when change detected (submits current device)
    # report_change: True when chages shoudl be reported
    # filter: callback to be called with current read value, allows building averages or medians
    #         should return the new value or None, if value is not yet current
    def __init__(self, name, port, value_map=None,
                 setters=None, getters=None,
                 ignore_case=True, on_change=None,
                 report_change=True, filter=None):
        global _topic
        self._value = None # current value(s) are saved here
        self.on_change = on_change
        self.report_change = report_change
        self.name = name
        self.port = port
        self.ignore_case = ignore_case
        self.filter = filter
        # using these in defaults and with len comparison does not work
        if setters is None:
            self.setters = {}
        else:
            self.setters = setters
        if getters is None:
            self.getters = {"": self.mapped_value}  # add default getter for main topic
        else:
            self.getters = getters
        self.value_map = value_map

    def set_on_change(self, on_change):
        self.on_change = on_change

    def is_settable(self):
        return len(self.setters) > 0

    def add_setter(self, setter_name, callback):
        if self.ignore_case: setter_name = setter_name.lower()
        self.commands[setter_name] = callback

    def delete_setter(self, setter_name):
        if self.ignore_case: setter_name = setter_name.lower()
        self.commands.pop(setter_name)

    def run_setter(self, setter, command_str):
        if self.ignore_case:
            command_str = command_str.lower()
            setter = setter.lower()
        if setter in self.setters:
            self.setters[setter](command_str)
        # else ignore

    def add_getter(self, getter_name, callback):
        if self.ignore_case: getter_name = getter_name.lower()
        self.commands[getter_name] = callback

    def delete_getter(self, getter_name):
        if self.ignore_case: getter_name = getter_name.lower()
        self.commands.pop(getter_name)

    def run_getter(self, getter):
        if self.ignore_case:
            getter = getter.lower()
        if getter in self.getters:
            return self.getters[getter]()
        return None  # else ignore

    def value(self):
        # this should NOT be overloaded
        # It's just the getter for self._value
        return self._value

    def mapped_value(self, v=None):
        # try to map a (eventually given) value according
        # to the value map
        if v is None:
            v = self.value()
        if v is None:
            return None
        if isinstance(self.value_map, dict):
            return self.value_map[v]
        return v

    def measure(self):
        # does nothing by default
        # Usually this needs to be overwritten
        # It should make sure to trigger necessary steps to
        # measure from the physical hardware a sensor value (or
        # several values from a multi sensor).
        # It needs to return the current value
        # It should return None, if no value can be measured (or has not been)
        # This is called from update to trigger the actual value generation
        return None

    def update(self):
        # returns True if the update caused a change in value
        # and report_change is set to True
        # This will be called very often from event loop
        # this only reports change when the measured (and filtered)
        # value is new (has changed)
        newval = self.measure()  # measure new value or trigger physical update
        if newval is None:
            newval = self._value
        if self.filter is not None:
            newval = self.filter(newval)
        if newval is not None:  # filter could have returned None
            changed = self._value != newval
            if changed:
                if self.on_change is not None:
                    self.on_change(self)
                self._value = newval
            if self.report_change:
                return changed
        return False

    def updated_value(self):
        # This should only be used for debugging purposes
        self.update() # execute update first before returning value
        return self._value
