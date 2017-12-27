# based and modified from
# https://github.com/micropython/micropython-lib/tree/master/upysh
# mods: ulno

import sys
import os


class LS:

    def __repr__(self):
        self.__call__()
        return ""

    def __call__(self, path="."):
        l = os.listdir(path)
        l.sort()
        for f in l:
            st = os.stat("%s/%s" % (path, f))
            if st[0] & 0x4000:  # stat.S_IFDIR
                print("   <dir> %s" % f)
            else:
                print("% 8d %s" % (st[6], f))


class PWD:

    def __repr__(self):
        return os.getcwd()

    def __call__(self):
        return self.__repr__()


class CLEAR:
    def __repr__(self):
        return "\x1b[2J\x1b[H"

    def __call__(self):
        return self.__repr__()


pwd = PWD()
ls = LS()
clear = CLEAR()

cd = os.chdir
mkdir = os.mkdir
mv = os.rename
rm = os.remove
rmdir = os.rmdir


def head(f, n=10):
    with open(f) as f:
        for i in range(n):
            l = f.readline()
            if not l: break
            sys.stdout.write(l)


def cat(f):
    head(f, 1 << 30)


def newfile(path):
    print("Type file contents line by line, finish with EOF (Ctrl+D).")
    with open(path, "w") as f:
        while 1:
            try:
                l = input()
            except EOFError:
                break
            f.write(l)
            f.write("\n")


# ulno mods
# remove thh man-page as also in ulnoiot

# copy helper
def cp(from_file, to_file):
    print("Copying %s to %s." % (from_file, to_file))
    ff = open(from_file, "rb")
    tf = open(to_file, "wb")
    tf.write(ff.read())
    tf.close()
    ff.close()


copy = cp
