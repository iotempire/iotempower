# convenient help and overwrite of existing

import os

_oldhelp = help
help_path = "/ulnoiot/help"


def _help_file_name(name):
    return name + ".txt"


def help_list_print(list):
    max_line=79
    output=""
    for e in list:
        newlen = len(output) + len(e) + 2
        if  newlen >= max_line:
            print(output + ", ")
            output=e
        else:
            if len(output) > 0:
                output = output + ", " + e
            else:
                output = e
    print(output)


def dir_content(path, extensions=["py","txt","rst"], ignore_underscore=True):
    outlist=[]
    for f in os.listdir(path):
        if ignore_underscore:
            if f.startswith("_"):
                continue
        for ext in extensions:
            if f.endswith("." + ext):
                outlist.append(f[0:-len(ext)-1])
    outlist.sort()
    return outlist


def command_list():
    return dir_content(help_path)

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
