# Some small utilities for ulnoiot
#

import os
from uhashlib import sha1
import ubinascii
import gc
import machine


def file_hashs(root=""):
    speed=machine.freq()
    machine.freq(160000000) # we want to be fast for this
    l = os.listdir("/"+root)
    for f in l:
        gc.collect()
        st = os.stat("%s/%s" % (root, f))
        if st[0] & 0x4000:  # stat.S_IFDIR
            file_hashs(root+"/"+f)
        else:
            file=root+"/"+f
            h=sha1()
            for l in open(file):
                h.update(l)
            print(ubinascii.hexlify(h.digest()).decode(),file[1:])
    machine.freq(speed) # restore speed
