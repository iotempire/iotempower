# Animation class for performing a smimple animation
# of rgb lights
#
# author: ulno
# created: 2017-05-23
#
import time
from uiot import colors as _c


class Animation():
    def __init__(self, device, command_str):
        self.program = command_str.split()
        self.device = device
        self.playing = True
        self.program = command_str.split()
        self.step = 0
        self.fade_goals = {}
        self.length = 0
        self.starttime = 0
        self.delta = 0
        self.lasttime = time.ticks_ms()
        self.effect = None

    def stop(self):
        self.playing = False

    def start(self):
        if self.program is not None and len(self.program) > 0:
            if self.step >= len(self.program):
                self.step = 0
            self.playing = True

    def effect_move(self):
        count = self.device.ws_leds
        if self.effect_steps >= 0:  # forward
            # print("forward")
            top = self.device.get(led_num=count - 1)
            for i in range(count - 1):
                c = self.device.get(led_num=count - i - 2)
                self.device.set(*c, led_num=count - i - 1)
            self.device.set(*top, led_num=0)
        else:  # backward
            # print("backward")
            bottom = self.device.get(led_num=0)
            for i in range(count - 1):
                c = self.device.get(led_num=i + 1)
                self.device.set(*c, led_num=i)
            self.device.set(*bottom, led_num=count - 1)

    def next(self):
        current_time = time.ticks_ms()
        while self.length <= 0:
            # find next command
            commands = "sfpre"
            while self.step < len(self.program) \
                    and self.program[self.step] not in commands:
                self.step += 1
            if self.step >= len(self.program):
                self.stop()
                return
            cmd = self.program[self.step]
            # read arguments
            args = []
            self.step += 1
            while self.step < len(self.program) \
                    and self.program[self.step] not in commands:
                args.append(self.program[self.step])
                self.step += 1
            if cmd == "s":  # set
                self.device._command_list_rgb(args)
            elif cmd == "f":  # fade goal: f nr color
                led_num = int(args[0]) - 1
                if led_num <= 0: led_num = 0
                self.fade_goals[led_num] = \
                    (self.device.get(led_num=led_num), \
                     _c.get(args[1]))
                print("Fade goal:", led_num, self.fade_goals[led_num])
            elif cmd == "e":  # effect: e name steps [param 0 [param 1 ...]]
                e = args[0]
                self.effect_steps = int(args[1])
                if e == "move":
                    self.effect = self.effect_move
                    self.effect_args = args[2:]
                    self.effect_step = 0
                    print("Move effect:", self.effect_steps)
            elif cmd == "p":
                self.length = int(args[0])
                self.starttime = current_time
            elif cmd == "r":
                self.step = 0
                self.length = 0
                break  # even if this loses a bit of time,
                # we want to prevent endless loops

        if self.length > 0:  # play animation
            delta = time.ticks_diff(current_time, self.starttime)
            progress = min(1.0, delta / self.length)
            changed = False
            for i in self.fade_goals:
                ((fr, fg, fb), (tr, tg, tb)) = self.fade_goals[i]
                (cr, cg, cb) = self.device.get(led_num=i)
                if self.length <= delta:
                    (nr, ng, nb) = (tr, tg, tb)
                    self.length = 0  # signal finished of this animation
                    del (self.fade_goals[i])  # clear them
                else:
                    nr = int(fr + (tr - fr) * progress)
                    # print("i",i,"new r",nr,"from r",fr,"to r",tr,"progress",progress) # debug
                    ng = int(fg + (tg - fg) * progress)
                    nb = int(fb + (tb - fb) * progress)
                if (cr, cg, cb) != (nr, ng, nb):
                    self.device._set(nr, ng, nb, i)
                    changed = True
            if self.effect is not None:
                stepsize = self.length / abs(self.effect_steps)
                # print("step-debug-1", self.effect_step, stepsize, delta,self.length) # debug
                while True:
                    current = stepsize * (self.effect_step + 1)
                    # print("step-debug-2", current) # debug
                    if current > delta or current >= self.length + stepsize:
                        break;
                    self.effect_step += 1
                    # print("executing step", self.effect_step, current, delta, stepsize) # debug
                    self.effect()
                    changed = True
                if delta >= self.length:
                    # print("finishing", self.effect_step, current, delta, stepsize, changed) # debug
                    self.length = 0
                    self.effect = None
            if changed:
                self.device._write()
