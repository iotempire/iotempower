# humidity temperature
# author: ulno
# created: 2017-04-08
#

from uiot._htdht import _HTDHT


class DHT11(_HTDHT):
    def __init__(self, name, pin, on_change=None):
        import dht
        _HTDHT.__init__(self, name, pin, dht.DHT11(pin), 1000,
                        on_change=on_change)
