# lcd display
# author: ulno
# created: 2017-10-16
#
# drive 44780/1602 i2c displays
#
# uses library from here:
#

# TODO: enable scrolling (get from display, predict wrapping and cursor)

from machine import I2C
from esp8266_i2c_lcd import I2cLcd
from uiot.device import Device


class Display44780(Device):

    # Handle display
    def __init__(self, name, sda=None, scl=None,
                 width=16, height=2,
                 addr=0x27):
        self.present = False
        self.last_text = ""
        self.char_width = width
        self.char_height = height
        self.display_buffer = bytearray(b" " * width * height)
        self.back_buffer = bytearray(b" " * width * height)
        self.x = 0
        self.y = 0

        if type(sda) is I2C:
            i2c = sda
        else:
            i2c = I2C(sda=sda, scl=scl)
        # test if lcd is responding
        try:
            self.dp = I2cLcd(i2c, addr, height, width)
            self.dp.clear()
            self.clear()
            self.hide_cursor()
            self.println("iot.ulno.net")

        except OSError:
            print("lcd not found")
        else:
            self.present = True
            Device.__init__(self, name, i2c, setters={"set": self.evaluate},
                            report_change=False)
            self.getters[""] = self.value

    def hide_cursor(self):
        self.dp.hide_cursor()

    def show_cursor(self):
        self.dp.show_cursor()

    def on(self):
        self.dp.backlight_on()

    def off(self):
        self.dp.backlight_off()

    def text(self, t, x, y, show=True):
        self.set_cursor(x, y)
        tx = x
        for c in t:
            self.back_buffer[y * self.char_width + tx] = ord(c)
            tx += 1
        if show: self.show()

    def set_cursor(self, x, y):
        self.x = x
        self.y = y

    # clear display immediately
    def clear(self, show=True):
        for line in range(self.char_height):
            for column in range(self.char_width):
                self.back_buffer[line * self.char_width + column] = 32
        self.set_cursor(0, 0)
        if show: self.show()

    def show(self):
        for line in range(self.char_height):
            for column in range(self.char_width):
                if self.display_buffer[line * self.char_width + column] \
                        != self.back_buffer[line * self.char_width + column]:
                    self.dp.move_to(column, line)
                    self.dp.putchar(chr(self.back_buffer[line * self.char_width + column]))
                    self.display_buffer[line * self.char_width + column] \
                        = self.back_buffer[line * self.char_width + column]
        self.dp.move_to(self.x, self.y)

    # scroll one line (add empty line at bottom)
    def scroll(self, show=True):
        for line in range(self.char_height - 1):
            self.back_buffer[line * self.char_width:(line + 1) * self.char_width] = \
                self.back_buffer[(line + 1) * self.char_width:(line + 2) * self.char_width]
        for x in range(self.char_width):
            self.back_buffer[(self.char_height - 1) * self.char_width] = 32
        if show: self.show()

    # move cursor down and scroll the text area by one line if at screen end
    def line_feed(self, show=True):
        if self.y < self.char_height - 1:
            self.y += 1
        else:
            self.scroll()
            self.dp.move_to(0, 0)

            # TODO: check if line really needs to be cleared (seems to happen sometimes that it does not clear)
            self.clear_line()
            if show: self.dp.show()
        self.x = 0

    # move just to start of line and clear the whole line
    def clear_line(self, show=True):
        self.x = 0
        # clear line
        for x in range(self.char_width):
            self.back_buffer[self.y * self.char_width + x] = 32
        if show: self.show()

    # print some text in the text area and linebreak and wrap if necessary
    def print(self, text="", newline=False, show=True):
        text = str(text)
        linefeed_last = text.endswith("\n")
        if linefeed_last:
            text = text[:-1]
        l_first = True
        for l in text.split("\n"):
            if not l_first:  # scroll if it's not the first line
                self.line_feed(show=False)
            l_first = False
            while len(l) > 0:
                sub = l[0:self.char_width - self.x]
                self.text(sub, self.x, self.y)
                self.x += len(sub)
                if self.x >= self.char_width:
                    self.line_feed(show=False)
                l = l[len(sub):]
        if linefeed_last:
            self.line_feed(show=False)
        if newline:
            self.line_feed(show=False)
        if show: self.show()

    def println(self, text="", show=True):
        self.print(text, newline=True, show=show)

    def evaluate(self, msg):
        print("Received text in callback:", msg)
        if msg.startswith("&&clear"):
            self.clear()
            if len(msg) > 8:
                self.evaluate(msg[8:])
        elif msg.startswith("&&linefeed"):
            self.line_feed()
            if len(msg) > 11:
                self.evaluate(msg[11:])
        elif msg.startswith("&&newline"):
            self.line_feed()
            if len(msg) > 11:
                self.evaluate(msg[11:])
        elif msg.startswith("&&lf"):
            self.line_feed()
            if len(msg) > 5:
                self.evaluate(msg[5:])
        elif msg.startswith("&&nl"):
            self.line_feed()
            if len(msg) > 5:
                self.evaluate(msg[5:])
        elif msg.startswith("&&cursor"):
            csplit = msg[9:].strip().split()
            self.set_cursor(int(csplit[0]), int(csplit[1]))
            inlen = 11 + len(csplit[0]) + len(csplit[1])
            if len(msg) > inlen:
                self.evaluate(msg[inlen:])
        else:
            self.print(msg)
            self.last_text = msg

    def value(self):
        return self.last_text
