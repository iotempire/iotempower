# convenient help and overwrite of existing

import os

_oldhelp = help


def help_list_print(list):
    max_line = 79
    output = ""
    for e in list:
        newlen = len(output) + len(e) + 2
        if newlen >= max_line:
            print(output + ", ")
            output = e
        else:
            if len(output) > 0:
                output = output + ", " + e
            else:
                output = e
    print(output)


def _open_db():
    return open("/help.db","r")


def _process_help(name):
    db = _open_db()
    search_mode = True
    search_for = "!!!" + name
    for l in db:
        l = l.strip()
        if search_mode:
            if l == search_for:  # found help
                search_mode = False
        else:
            if l.startswith("!!!"):  # this is the next command
                break  # stop process
            if l.startswith("!!"):
                eval(l[2:])
            else:
                print(l)
    db.close()
    if search_mode:  # hasn't switched -> nothing found
        print("No help entry found for:",name)


def command_list(ignore_underscore=True):
    outlist = []
    for l in _open_db():
        if l.startswith("!!!"):
            l = l.strip()
            l = l[3:]
            if ignore_underscore:
                if l.startswith("_"):
                    continue
            outlist.append(l)
    outlist.sort()
    return outlist


class HELP:
    def __repr__(self):
        _process_help("_general")
        return ""

    def __call__(self, *args):
        if len(args) > 0:
            h = args[0]
            if h is not None and isinstance(h, str):
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

    def __call__(self, *args):
        if len(args) == 0:
            self.__repr__()
        else:
            help(*args)


man = Man()
