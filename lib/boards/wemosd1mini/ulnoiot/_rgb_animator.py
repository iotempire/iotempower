# Animation class for performing a smimple animation
# of rgb lights
#
# author: ulno
# created: 2017-05-23
#
import time
from ulnoiot import colors as _c

class Animation():
    def __init__(self,device,command_list):
        self.device=device
        self.playing = True
        self.program = self.split()
        self.step = 0
        self.fade_goals = {}
        self.length = 0
        self.starttime = 0
        self.delta = 0
        self.lasttime = time.ticks_ms()

    def stop(self):
        self.playing=False

    def start(self):
        if self.program is not None and len(self.program)>0:
            if self.step >= len(self.program):
                self.step=0
            self.playing = True

    def next(self):
        current_time = time.ticks_ms()
        while self.length <= 0:
            # find next command
            commands="sfpr"
            while self.step<len(self.program) \
                    and self.program[self.step] not in commands:
                self.step += 1
            if self.step>=len(self.program):
                self.stop()
                return
            cmd=self.program[self.step]
            # read arguments
            args=[]
            self.step += 1
            while self.step<len(self.program) \
                    and self.program[self.step] not in commands:
                args.append(self.program[self.step])
                self.step += 1
            if cmd=="s": # set
                self.device._set_rgb_list(args)
            elif cmd=="f": # fade goal
                led_num=int(args[0])
                self.fade_goals[int(args[0])] = \
                    (self.device.get_rgb(led_num=led_num), \
                     _c.get(args[1]))
            elif cmd=="p":
                self.length =  int(args[0])
                self.starttime = current_time
            elif cmd=="r":
                self.step = 0
                break # even if this loses a bit of time, we want to prevent endless loops
        if self.length > 0: # play animation
            delta = time.ticks_diff(current_time, self.starttime)
            progress = delta / self.length
            changed=False
            for i in self.fade_goals:
                ((fr,fg,fb),(tr,tg,tb)) = self.fade_goals[i]
                (cr,cg,cb) =  self.device.get_rgb(led_num=i)
                if self.length <= delta :
                    (nr, ng, nb) = (tr,tg,tb)
                else:
                    nr = tr-(fr-tr)*progress
                    ng = tg-(fg-tg)*progress
                    nb = tb-(fb-tb)*progress
                if (cr,cg,cb) != (nr,ng,nb):
                    self.device._set_rgb(nr,ng,nb,no_write=True)
                    changed=True
            if changed:
                self.device._write_rgb()
