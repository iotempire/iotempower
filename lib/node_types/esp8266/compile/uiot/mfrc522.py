# MFRC522 Device
# Reading of rfid and nfc tags
#
# author: ulno
#
# Based on:
#  - Stefan Wendlers (wendlers): https://github.com/wendlers/micropython-mfrc522
#  - Cefn Hoile (cefn): https://github.com/cefn/micropython-mfrc522
#  - TODO: add esp32 support from here: https://github.com/Lennyz1988/micropython-mfrc522
#
# created: 2018-03-20
#
# Driver to use the MFRC522 NFC/RFID-tag reader/writer

from machine import Pin, SPI
from uiot.device import Device

from os import uname
import ubinascii
import time

from micropython import const

emptyRecv = b""

_GAIN_REG = const(0x26)
_MAX_GAIN = const(0x07)

_OK = const(0)
_NOTAGERR = const(1)
_ERR = const(2)

_REQIDL = const(0x26)
#_REQALL = const(0x52)
_AUTHENT1A = const(0x60)
#_AUTHENT1B = const(0x61)


class MFRC522(Device):

    def __init__(self, name, rst, cs=None, sck=None, mosi=None, miso=None,
                 key=b'\xff\xff\xff\xff\xff\xff', datasize=128,
                 on_change=None, report_change=True, filter=None):

        rst.init(Pin.OUT)
        self.rst = rst
        if self.rst is not None:
            self.rst.value(0)
        self.datasize = datasize
        self.key = key
        board = uname()[0]
        if sck is None: # select defaults (hw based spi)
            if board == 'WiPy' or board == 'LoPy' or board == 'FiPy':
                spi = SPI(0)
                spi.init(SPI.MASTER, baudrate=1000000,
                         pins=(14, 13, 12))
            elif board == 'esp8266':
                spi = SPI(1, baudrate=2500000, polarity=0, phase=0)
                spi.init()
                # hw uses the following pins
                # sck = Pin(14,Pin.OUT)  # d5 (ck)
                # mosi = Pin(13,Pin.OUT) # d7 (mo)
                # miso = Pin(12,Pin.IN)  # d6 (mi)
            else:
                raise RuntimeError("Unsupported platform")
            # TODO: does it make sense to set cs here to default?
            if cs is None:
                self.cs = Pin(15, Pin.OUT) # d8 (cs)
            else:
                self.cs = cs
                cs.init(Pin.OUT)
            self.spi = spi
        else:
            self.cs = cs
            if cs is not None:
                cs.init(Pin.OUT)
            if type(sck) is SPI:
                self.spi = sck
            else:
                if board == 'WiPy' or board == 'LoPy' or board == 'FiPy':
                    spi = SPI(0)
                    spi.init(SPI.MASTER, baudrate=1000000,
                             pins=(sck, mosi, miso))
                elif board == 'esp8266':
                    sck.init(Pin.OUT)
                    mosi.init(Pin.OUT)
                    miso.init(Pin.IN)
                    spi = SPI(baudrate=100000, polarity=0, phase=0,
                              sck=sck, mosi=mosi, miso=miso)
                    spi.init()
                else:
                    raise RuntimeError("Unsupported platform")
                self.spi = spi

        if self.rst is not None:
            self.rst.value(1)
        if self.cs is not None:
            self.cs.value(0)

        # get some memory
        self.reg_buf = bytearray(4)
        self.block_write_buf = bytearray(18)
        self.auth_buf = bytearray(12)
        self.wreg_buf = bytearray(2)
        self.rreg_buf = bytearray(1)
        self.recv_buf = bytearray(16)
        self.recv_mv = memoryview(self.recv_buf)
        self.maxdata = datasize
        self.card_data_buf = bytearray(16)

        self.phys_init()
        self.last_access = time.ticks_ms()
        self.access_interval = 500  # only allow reads every 500ms

        Device.__init__(self, name, spi,
                        on_change=on_change,
                        report_change=report_change, filter=filter)

    def measure(self):
        now = time.ticks_ms()
        if time.ticks_diff(now, self.last_access) > self.access_interval:
            retval = b'0'  # Let's assume nothing is there
            try:
                (stat, tag_type) = self.request(_REQIDL)
                if stat == _OK:  # Something is there, so ignore this if not successfully read
                    retval = None

                    (stat, raw_uid) = self.anticoll()

                    if stat == _OK:
                        retval = str(tag_type).encode() + b' ' + ubinascii.hexlify(bytes(raw_uid))

                        if self.select_tag(raw_uid) == _OK:

                            data_read = False
                            for sector in range(0, self.datasize//12): # /16/3*4
                                if (sector+1) % 4 != 0:  # every 4th sector has only management info
                                    if self.auth(_AUTHENT1A, sector+4, self.key, raw_uid) == _OK:
                                        if not data_read:
                                            retval += b' '
                                        self._read(sector + 4, into=self.card_data_buf)
                                        retval += ubinascii.hexlify(self.card_data_buf)  # TODO: remove hexlify and do binary
                                        data_read = True
                                    else:
                                        print("MFRC522: authentication error")
                                        return None  # Not successfull read
                            self.stop_crypto1()
                            # TODO: think how to avoid this busy-waiting
                            print("MFRC522: read time " + str(time.ticks_diff(time.ticks_ms(), now)))
                        else:
                            print("MFRC522: Select failed")
                        _ = self.anticoll()  # prevent reading of empty
                        self.last_access = time.ticks_ms()  # TODO: check why this is necessary
            except Exception as e:
                print('Trouble in MFRC522. Got exception:', e)
                print('Retval so far:', retval)
            return retval
        return None  # didn't read

    def write(self, data):
        if type(data) is str:
            data=data.encode()
        data_mv = memoryview(data)

        print()
        print("Place card before reader to write the following data: ", data)
        print()

        try:
            while True:

                (stat, tag_type) = self.request(_REQIDL)

                if stat == _OK:

                    (stat, raw_uid) = self.anticoll()

                    if stat == _OK:
                        print("New card detected")
                        print("  - tag type: 0x%02x" % tag_type)
                        print("  - uid	 : 0x%02x%02x%02x%02x" % (raw_uid[0], raw_uid[1], raw_uid[2], raw_uid[3]))
                        print()

                        if self.select_tag(raw_uid) == _OK:
                            data_ptr = 0
                            for sector in range(0, (len(data)+15)//12): # /16/3*4
                                if (sector+1) % 4 != 0:  # every 4th sector has only management info
                                    if self.auth(_AUTHENT1A, sector+4, self.key, raw_uid) == _OK:
                                        print("data_ptr", data_ptr)
                                        stat = self._write(sector + 4, data_mv[data_ptr:data_ptr + 16])
                                        if stat == _OK:
                                            print("Data written to sector",sector+4)
                                        else:
                                            print("Failed to write data to sector",sector+4)
                                        data_ptr += 16
                                    else:
                                        print("MFRC522: authentication error")
                                        break
                            self.stop_crypto1()
                        else:
                            print("MFRC522: Failed to select tag")
                        break  # finish loop

        except KeyboardInterrupt:
            print("Bye")

    def _wreg(self, reg, val):
        if self.cs is not None:
            self.cs.value(0)
        buf = self.wreg_buf
        buf[0]=0xff & ((reg << 1) & 0x7e)
        buf[1]=0xff & val
        self.spi.write(buf)
        if self.cs is not None:
            self.cs.value(1)

    def _rreg(self, reg):
        if self.cs is not None:
            self.cs.value(0)
        buf = self.rreg_buf
        buf[0]=0xff & (((reg << 1) & 0x7e) | 0x80)
        self.spi.write(buf)
        val = self.spi.read(1)
        if self.cs is not None:
            self.cs.value(1)

        return val[0]

    def _sflags(self, reg, mask):
        self._wreg(reg, self._rreg(reg) | mask)

    def _cflags(self, reg, mask):
        self._wreg(reg, self._rreg(reg) & (~mask))

    def _tocard(self, cmd, send, into=None):

        recv = emptyRecv
        bits = irq_en = wait_irq = n = 0
        stat = _ERR

        if cmd == 0x0E:
            irq_en = 0x12
            wait_irq = 0x10
        elif cmd == 0x0C:
            irq_en = 0x77
            wait_irq = 0x30

        self._wreg(0x02, irq_en | 0x80)
        self._cflags(0x04, 0x80)
        self._sflags(0x0A, 0x80)
        self._wreg(0x01, 0x00)

        for c in send:
            self._wreg(0x09, c)
        self._wreg(0x01, cmd)

        if cmd == 0x0C:
            self._sflags(0x0D, 0x80)

#        i = 2000
        i = 100 # ulno: 2000 seems very high - is this busy waiting here necessary?
        while True:
            n = self._rreg(0x04)
            i -= 1
            if ~((i != 0) and ~(n & 0x01) and ~(n & wait_irq)):
                break

        self._cflags(0x0D, 0x80)

        if i:
            if (self._rreg(0x06) & 0x1B) == 0x00:
                stat = _OK

                if n & irq_en & 0x01:
                    stat = _NOTAGERR
                elif cmd == 0x0C:
                    n = self._rreg(0x0A)
                    lbits = self._rreg(0x0C) & 0x07
                    if lbits != 0:
                        bits = (n - 1) * 8 + lbits
                    else:
                        bits = n * 8

                    if n == 0:
                        n = 1
                    elif n > 16:
                        n = 16

                    if into is None:
                        recv = self.recv_buf
                    else:
                        recv = into
                    pos = 0
                    while pos < n:
                        recv[pos] = self._rreg(0x09)
                        pos += 1
                    if into is None:
                        recv = self.recv_mv[:n]
                    else:
                        recv = into

            else:
                stat = _ERR

        return stat, recv, bits

    def _assign_crc(self, data, count):

        self._cflags(0x05, 0x04)
        self._sflags(0x0A, 0x80)

        dataPos = 0
        while dataPos < count:
            self._wreg(0x09, data[dataPos])
            dataPos += 1

        self._wreg(0x01, 0x03)

        i = 0xFF
        while True:
            n = self._rreg(0x05)
            i -= 1
            if not ((i != 0) and not (n & 0x04)):
                break

        data[count] = self._rreg(0x22)
        data[count + 1] = self._rreg(0x21)

    def phys_init(self):
        self.reset()
        self._wreg(0x2A, 0x8D)
        self._wreg(0x2B, 0x3E)
        self._wreg(0x2D, 30)
        self._wreg(0x2C, 0)
        self._wreg(0x15, 0x40)
        self._wreg(0x11, 0x3D)
        self.set_gain(_MAX_GAIN)
        self.antenna_on()


    def reset(self):
        self._wreg(0x01, 0x0F)

    def antenna_on(self, on=True):

        if on and ~(self._rreg(0x14) & 0x03):
            self._sflags(0x14, 0x03)
        else:
            self._cflags(0x14, 0x03)

    def request(self, mode):

        self._wreg(0x0D, 0x07)
        (stat, recv, bits) = self._tocard(0x0C, [mode])

        if (stat != _OK) | (bits != 0x10):
            stat = _ERR

        return stat, bits

    def anticoll(self):

        ser_chk = 0
        ser = [0x93, 0x20]

        self._wreg(0x0D, 0x00)
        (stat, recv, bits) = self._tocard(0x0C, ser)

        if stat == _OK:
            if len(recv) == 5:
                for i in range(4):
                    ser_chk = ser_chk ^ recv[i]
                if ser_chk != recv[4]:
                    stat = _ERR
            else:
                stat = _ERR

        # CH Note bytearray allocation here - TODO fix
        return stat, bytearray(recv)

    def select_tag(self, ser):
        # TODO CH normalise all list manipulation to bytearray, avoid below allocation
        buf = bytearray(9)
        buf[0] = 0x93
        buf[1] = 0x70
        buf[2:7] = ser
        self._assign_crc(buf, 7)
        (stat, recv, bits) = self._tocard(0x0C, buf)
        return _OK if (stat == _OK) and (bits == 0x18) else _ERR

    def auth(self, mode, addr, sect, ser):
        # TODO CH void ser[:4] implicit list allocation
        buf = self.auth_buf
        buf[0] = mode # A or B
        buf[1] = addr # block
        buf[2:8] = sect # key bytes
        buf[8:12] = ser[:4] # 4 bytes of id
        return self._tocard(0x0E, buf)[0]

    # TODO this may well need to be implemented for vault to properly back out from a card session
    # TODO how, why, when is 'HaltA' needed? see https://github.com/cefn/micropython-mfrc522/issues/1
    def halt_a(self):
        pass

    def stop_crypto1(self):
        self._cflags(0x08, 0x08)

    def set_gain(self, gain):
        assert gain <= _MAX_GAIN
        # clear bits
        self._cflags(_GAIN_REG, 0x07 << 4)
        # set bits according to gain
        self._sflags(_GAIN_REG, gain << 4)

    def _read(self, addr, into = None):
        buf = self.reg_buf
        buf[0]=0x30
        buf[1]=addr
        self._assign_crc(buf, 2)
        (stat, recv, _) = self._tocard(0x0C, buf, into=into)
        # TODO this logic probably wrong (should be 'into is None'?)
        if into is None: # superstitiously avoid returning read buffer memoryview
            # CH Note bytearray allocation here
            recv = bytearray(recv)
        return recv if stat == _OK else None

    def _write(self, addr, data):
        buf = self.reg_buf
        buf[0] = 0xA0
        buf[1] = addr
        self._assign_crc(buf, 2)
        (stat, recv, bits) = self._tocard(0x0C, buf)

        if not (stat == _OK) or not (bits == 4) or not ((recv[0] & 0x0F) == 0x0A):
            stat = _ERR
        else:
            buf = self.block_write_buf

            i = 0
            dl = len(data)
            while i < 16:
                if i<dl:
                    buf[i] = data[i]  # TODO: eliminate this, accelerate it?
                else:
                    buf[i] = 0
                i += 1

            self._assign_crc(buf, 16)
            (stat, recv, bits) = self._tocard(0x0C, buf)
            if not (stat == _OK) or not (bits == 4) or not ((recv[0] & 0x0F) == 0x0A):
                stat = _ERR

        return stat
