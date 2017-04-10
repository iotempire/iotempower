import os
from ulnoiot import *

def list():
    l = os.listdir("/ulnoiot/shield")
    lf = []
    for f in l:
        if f.endswith(".py") and not f.startswith("_"):
            lf.append(f[0:-3])
    lf.sort()
    print("Available shield drivers:")
    print(", ".join(lf))
    print("To use shieldname: from ulnoiot.shield import shieldname")
    print("Show this list again with import ulnoiot.shield; ulnoiot.shield.list()")
# TODO: think about ways to move this text to help

list()