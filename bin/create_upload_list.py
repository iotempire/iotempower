#!/usr/bin/env python3
# load a list of hash-filename entries and compare with local files
# to generate an upload list
#
# needs to be executed in files' root directory

import sys
import os
from hashlib import sha1

onlycreate=["webrepl_cfg.py","user.py","my_devices.py"]

def main(argv):
    if len(argv)<3:
        print("Need port, hash source file, and update file as parameters.")
        return
    hashs={}
    for l in open(argv[1]):
        l=l.strip()
        parts=l.split(" ")
        if parts[0] != "<dir>":
            hashs[(" ".join(parts[1:]))[1:]]=parts[0]
    output=open(argv[2],"wb")
    output.write(("open %s\n"%argv[0]).encode())
    output.write(b"cd /\n")
    output.write(b"md /ulnoiot\n")
    output.write(b"md /ulnoiot/help\n")
    output.write(b"md /ulnoiot/shield\n")
    for filename in hashs:
        try:
            f=open(filename,"rb")
        except FileNotFoundError:
            f=None
        if f is None:
            pass
            #print("rm %f"%f) delete nothing local
            print("#", filename,"is not existing locally.")
        else:
            if filename not in onlycreate:
                h=sha1()
                for l in f:
                    h.update(l)
                if h.hexdigest() == hashs[filename]:
                    print("#", filename, "matches")
                else:
                    print("put %s /%s"%(filename,filename))
                    output.write(("put %s /%s\n"%(filename,filename)).encode())
            f.close()
    print("# Missing files")
    # now check which files are missing
    for root, dirs, files in os.walk("."):
        for filename in files:
            path=(root+"/"+filename)[2:] # skip ./
            if path not in hashs:
                print("put %s %s" % (path, path))
                output.write(("put %s /%s\n"%(path,path)).encode())
    output.write(b"\nexec import machine\nexec machine.reset()\nclose")
    output.close()


if __name__ == "__main__":
   main(sys.argv[1:])