from ulnoiot import *
import urandom

mqtt("192.168.12.1", "owl01")
eyes = rgb_multi("eyes", d2, 2)

wink_colors = ["white", "blue", "red", "white"]


def random_wink(id=None):
    if not eyes.animation_is_playing():
        eye_nr = urandom.getrandbits(1) + 1
        col_sel = wink_colors[urandom.getrandbits(2)]
        eyes.animation("s 1 black s 2 black f {} {} p 1000 f 1 black f 2 black p 1000".format(eye_nr, col_sel))
    schedule_next_wink()


def schedule_next_wink():
    #    time_delta = 100+urandom.getrandbits(7)
    time_delta = 45 + urandom.getrandbits(6)
    do_later(time_delta, random_wink)


schedule_next_wink()

run()
