# Device
# author: ulno
# created: 2017-04-08
#
# The device class

class Device(object):
    def __init__(self, name, pin, value_map=None,
                 settable=False, ignore_command_case=True,
                 on_change=None):
        global _topic
        self.on_change=on_change
        self.topic = name
        self.name = name
        self.pin = pin
        self.command_topic = None
        self.ignore_case = ignore_command_case
        if settable:
            self.command_topic = self.topic + "/set"
            self.commands = {}
        self.value_map = value_map

    def set_on_change(self,on_change):
        self.on_change=on_change

    def is_settable(self):
        return self.command_topic is not None

    def set_command(self, callback):
        # this adds one generic command for all incoming commands
        self.commands = callback

    def add_command(self, command_name, callback):
        if self.ignore_case: command_name = command_name.lower()
        self.commands[command_name] = callback

    def delete_command(self, command_name):
        if self.ignore_case: command_name = command_name.lower()
        self.commands.pop(command_name)

    def command(self, command_str):
        if self.ignore_case:
            command_str = command_str.lower()
        if isinstance( self.commands, dict ):
            command_run = self.commands.get(command_str)
            if command_run is not None:
                command_run()
            else:
                print("Device %s cannot run command %s." % (self.name, command_str))
        else: # processes message itself
            self.commands(command_str)

    def value(self):
        return None

    def mapped_value(self,v=None):
        if v is None:
            v = self.value()
        if v is None:
            return None
        else:
            if isinstance( self.value_map,dict):
                return self.value_map[v]
            else:
                return None

    def _update(self):
        pass  # usually does nothing, can be overrriden to actualy update values, called by update

    def update(self):
        # returns True if the update caused a change in value
        oldval = self.value()
        self._update()
        newval = self.value()
        changed = oldval != newval
        if changed and self.on_change is not None:
            self.on_change(self)
        return changed
