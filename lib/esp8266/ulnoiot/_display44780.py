# lcd display
# author: ulno
# created: 2017-10-16
#
# drive 44780/1602 i2c displays

# TODO: enable scrolling (get from display, predict wrapping and cursor)

from machine import I2C
from esp8266_i2c_lcd import I2cLcd
from ulnoiot.device import Device

class Display44780(Device):

    # Handle display
    def __init__(self, name, sda=None, scl=None,
                 width=16, height=2,
                 addr=0x27, ignore_case=False):
        self.present = False
        self.last_text = ""
        self.char_width = width
        self.char_height = height

        # test if lcd is responding
        i2c = I2C(sda=sda, scl=scl)
        try:
            self.dp = I2cLcd(i2c, addr, height, width)
            self.clear()
            self.hide_cursor()
            self.println("iot.ulno.net\n")

        except OSError:
            print("lcd not found")
        else:
            self.present = True
            Device.__init__(self, name, i2c, setters={"set":self.evaluate},
                            ignore_case=ignore_case,report_change=False)
            self.getters[""]=self.value

    def hide_cursor(self):
        self.dp.hide_cursor()
    def show_cursor(self):
        self.dp.show_cursor()
    def on(self):
        self.dp.backlight_on()
    def off(self):
        self.dp.backlight_off()

    def text(self,t,x,y):
        self.dp.move_to(x,y)
        self.dp.putstr(t)

    def set_cursor(self,x, y):
        self.dp.move_to(x,y)

    # clear display immediately
    def clear(self):
        self.dp.clear()


    # print some text in the text area and linebreak and wrap if necessary
    def print(self,text="", newline=False, show=True):
        text=str(text)
        self.dp.putstr(text)
        if newline:
            self.dp.putstr("\n")

    def println(self,text="", show=True):
        self.print(text, newline=True, show=show)

    def evaluate(self, msg):
        print("Received text in callback:", msg)
        if msg == "&&clear":
            self.clear()
        else:
            self.println( msg )
            self.last_text = msg

    def value(self):
        return self.last_text