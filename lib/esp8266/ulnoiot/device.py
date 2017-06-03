# Device
# author: ulno
# created: 2017-04-08
#
# The device class

class Device(object):
    def __init__(self, name, pin, value_map=None,
                setters={}, getters={},
                ignore_case=True, on_change=None,
                report_change=True):
        global _topic
        self.on_change = on_change
        self.name = name
        self.pin = pin
        self.ignore_case = ignore_case
        self.setters = setters
        self.getters = getters
        self.report_change=report_change
        if len(getters) == 0:
            self.getters[''] = self.mapped_value # add default getter for main topic
        self.value_map = value_map

    def set_on_change(self,on_change):
        self.on_change=on_change

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
        return None # else ignore

    def value(self):
        return None

    def mapped_value(self,v=None):
        if v is None:
            v=self.value()
        if v is None:
            return None
        else:
            if isinstance( self.value_map,dict):
                return self.value_map[v]
            else:
                return None

    def _update(self):
        pass  # usually does nothing, can be overwritten
        # to actualy update values, called by update

    def update(self):
        # returns True if the update caused a change in value
        oldval = self.value()
        self._update()
        newval = self.value()
        changed = oldval != newval
        if changed and self.on_change is not None:
            self.on_change(self)
        if self.report_change:
            return changed
        return False
