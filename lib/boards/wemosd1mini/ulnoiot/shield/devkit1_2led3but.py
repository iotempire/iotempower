# Shield from ulno's iot devkit 1 with red and yellow led and 3 buttons
#

from ulnoiot import *

_p = "pressed"
_dp="de"+_p
lower=button("lower",d0,_dp,_p,pullup=False)
left=button("left",d3,_dp,_p)
right=button("right",d6,_dp,_p)
red=out("red",d7)
yellow=out("yellow",d8)

