# convenient help and overwrite of existing

import os

_oldhelp = help
help_path = "/ulnoiot/help"


def _help_file_name(name):
    return name + ".txt"


def command_list():
    outlist=[]
    for f in os.listdir(help_path):
        if f.endswith(".txt"):
            if not f.startswith("_"):
                outlist.append(f[0:-4])
    return outlist


def _process_help(name):
    p=os.getcwd()
    if p == "":
        p="/"
    os.chdir(help_path)
    for l in open(_help_file_name(name)):
        if l.startswith("!!"):
            eval(l[2:])
        else:
            print(l.strip())
    os.chdir(p)


class HELP:
    def __repr__(self):
        _process_help("_general")
        return ""

    def __call__(self,*args):
        if len(args)>0:
            h=args[0]
            if h is not None and isinstance(h,str):
                try:
                    _process_help(h)
                except:
                    _oldhelp(*args)
            else:
                _oldhelp(*args)
        else:
            self.__repr__()
            print()

help = HELP()

class Man():

    def __repr__(self):
        _process_help("man")
        return ""
    def __call__(self,*args):
        if len(args) == 0:
            self.__repr__()
        else:
            help(*args)

man = Man()
