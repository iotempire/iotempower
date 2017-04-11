#!/usr/bin/env python
# load a list of hash-filename entries and compare with local files
# to generate an upload list
#
# needs to be executed in files' root directory

import sys
import os
from hashlib import sha1

def main(argv):
    if len(argv)<3:
        print("Need port, hash source file, and update file as parameters.")
        return
    hashs={}
    for l in open(argv[1]):
        l=l.strip()
        hashs[l[41:]] = l[0:40]
    output=open(argv[2],"wb")
    output.write(b"open %b\n"%argv[0].encode())
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
            h=sha1()
            for l in f:
                h.update(l)
            if h.hexdigest() == hashs[filename]:
                print("#", filename, "matches")
            else:
                print("put %s %s"%(filename,filename))
                output.write(b"put %b %b\n"%(filename.encode(),filename.encode()))
            f.close()
    print("# Missing files")
    # now check which files are missing
    for root, dirs, files in os.walk("."):
        for filename in files:
            path=(root+"/"+filename)[2:] # skip ./
            if path not in hashs:
                print("put %s %s" % (path, path))
                output.write(b"put %b %b\n"%(path.encode(),path.encode()))
    output.write(b"\nexec import machine\nexec machine.reset()\n")
    output.close()


if __name__ == "__main__":
   main(sys.argv[1:])