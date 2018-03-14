# humidity temperature
# author: ulno
# created: 2017-04-08
#

from uiot._htdht import _HTDHT

class DHT22(_HTDHT):
    def __init__(self, name, pin, on_change=None, filter=None):
        # TODO: also handle an I2C object here
        import dht
        _HTDHT.__init__(self, name, pin, dht.DHT22(pin), 2000,
                        on_change=on_change, filter=filter)
