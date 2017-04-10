# convenient help and overwrite of existing

from ulnoiot import _helplist

_oldhelp = help

def _helppath( name ):
    return  "/ulnoiot/help/" + name + ".txt"

def _helpcontent(name):
    return open(_helppath(name)).read()

class HELP:
    def __repr__(self):
        l=_helplist.values()
        l.sort()
        return _helpcontent("_general") \
               + "\n\nFor more help, try to inquire with help(command_name)\nabout the following commands:\n" \
               + ", ".join(l)

    def __call__(self,*args):
        if len(args)>0:
            a=args[0]
            try:
                h=_helplist.get(a)
            except:
                h=None
            if h is not None:
                try:
                    print(_helpcontent(h))
                except:
                    _oldhelp(*args)
            else:
                _oldhelp(*args)
        else:
            print(self.__repr__())

help = HELP()

class Man():

    def __repr__(self):
        return _helpcontent("man")
    def __call__(self,*args):
        if len(args) == 0:
            print( self.__repr__() )
        else:
            help(*args)

man = Man()
