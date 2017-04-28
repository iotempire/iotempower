# Some small utilities for ulnoiot
#

import os
from uhashlib import sha1
import ubinascii
import gc
import machine


def file_hashs():
    speed=machine.freq()
    machine.freq(160000000) # we want to be fast for this
    p=os.getcwd()
    if p == "":
        p = "/"
    os.chdir("/")
    _file_hashs("")
    os.chdir(p)
    machine.freq(speed) # restore speed


def _file_hashs(root):
    l = os.listdir(root)
    for f in l:
        gc.collect()
        st = os.stat("%s/%s" % (root, f))
        if st[0] & 0x4000:  # stat.S_IFDIR
            _file_hashs(root+"/"+f)
        else:
            file=root+"/"+f
            h=sha1()
            for l in open(file):
                h.update(l)
            print(ubinascii.hexlify(h.digest()).decode(),file[1:])
