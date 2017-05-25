# Animation class for performing a smimple animation
# of rgb lights
#
# author: ulno
# created: 2017-05-23
#
import time
from ulnoiot import colors as _c

class Animation():
    def __init__(self, device, command_str):
        self.program=command_str.split()
        self.device=device
        self.playing = True
        self.program = command_str.split()
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
                self.device._command_list_rgb(args)
            elif cmd=="f": # fade goal
                led_num=int(args[0])-1
                if led_num<=0: led_num=0
                self.fade_goals[led_num] = \
                    (self.device.get(led_num=led_num), \
                     _c.get(args[1]))
                print("fade goal",led_num,self.fade_goals[led_num])
            elif cmd=="p":
                self.length =  int(args[0])
                self.starttime = current_time
            elif cmd=="r":
                self.step = 0
                break # even if this loses a bit of time, we want to prevent endless loops

        if self.length > 0: # play animation
            delta = time.ticks_diff(current_time, self.starttime)
            progress = min(1.0,delta/self.length)
            changed=False
            for i in self.fade_goals:
                ((fr,fg,fb),(tr,tg,tb)) = self.fade_goals[i]
                (cr,cg,cb) = self.device.get(led_num=i)
                if self.length <= delta:
                    (nr,ng,nb) = (tr,tg,tb)
                    self.length=0 # signal finished of this animation
                    del(self.fade_goals[i]) # clear them
                else:
                    nr = int(fr+(tr-fr)*progress)
                    #debug print("i",i,"new r",nr,"from r",fr,"to r",tr,"progress",progress)
                    ng = int(fg+(tg-fg)*progress)
                    nb = int(fb+(tb-fb)*progress)
                if (cr,cg,cb) != (nr,ng,nb):
                    self.device._set(nr,ng,nb,i)
                    changed=True
            if changed:
                self.device._write()