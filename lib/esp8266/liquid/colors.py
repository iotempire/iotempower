# Some rgb-colors
#
# author: ulno
# created: 2017-05-23
#

black = (0, 0, 0)
white = (255, 255, 255)

clist = {
    "red": (255, 0, 0),
    "green": (0, 255, 0),
    "blue": (0, 0, 255),
    "yellow": (255, 255, 0),
    "orange": (255, 128, 0),
    "lightgreen": (128, 255, 0),
    "purple": (255, 0, 255),
    "black": black,
    "white": white
}


def get(s):
    if s in clist:
        return clist[s]
    # it might be given in hex
    if len(s) == 6:
        return (int("0x" + s[0:2]), int("0x" + s[2:4]), int("0x" + s[4:6]))
    return None
