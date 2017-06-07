# Simple servo switch control device
# author: ulno
# created: 2017-06-06
#

from ulnoiot._servo import Servo

class Servo_Switch(Servo):
    # Handle output devices
    def __init__(self, name, pin,
                 on_angle, off_angle,
                 on_command='on', off_command='off',
                 back_to_angle=None,
                 turn_time_ms=700,
                 freq=50, min_us=600, max_us=2400, angle=180,
                 ignore_case=True, on_change=None,
                 report_change=False):
        Servo.__init__(self, name, pin,
                        turn_time_ms=turn_time_ms,
                        freq=freq, min_us=min_us,max_us=max_us,angle=angle,
                        ignore_case=ignore_case,
                        on_change = on_change,report_change=report_change)
        self.on_angle=on_angle
        self.off_angle=off_angle
        self.back_to_angle=back_to_angle
        self.status=None
        self.value_map={on_command:self.on,off_command:self.off}
        self.getters={"":self.mapped_value}
        self.setters={"set":self.evaluate}

    def evaluate(self,msg):
        if msg==self.on_command:
            self.on()
        elif msg == self.low_command:
            self.off()

    def on(self):
        self.turn(self.on_angle)

    def off(self):
        self.turn(self.off_angle)

    def value(self):
        return self.status
