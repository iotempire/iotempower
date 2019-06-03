#!/usr/bin/env python
#
# Check if an IoTempower dongle is on the given port

import sys
import os
import optparse
import logging

import serial
import time


def do_probe(port):
    # Create a serial connection
    answer = b""
    try:
        ser = serial.Serial(port, 115200, timeout=1);
        logging.info('Starting on %s.', port)
        ser.read_all()  # discard all
        ser.write(b"\n")
        ser.flush()
        answer = ser.read_until(b"UED>")
    except Exception as e:
        answer = b""

    if not answer.endswith(b"UED>"):
        sys.stderr.write("No dongle detected on %s.\n"%port)
        ser.close()
        return 1

    sys.stderr.write("Dongle detected on %s.\n"%port)

    ser.close()
    return 0


def parser(unparsed_args):
    parser = optparse.OptionParser(
        usage = "%prog [options]",
        description = "probe a given port (or /dev/ttyUSB0) for a IoTempower esp dongle."
    )

    # Configuration
    group = optparse.OptionGroup(parser, "Configuration")
    group.add_option("-p", "--port",
        dest = "esp_port",
        type = "str",
        help = "IoTempower esp dongle serial port (where the dongle is connected). Default /dev/ttyUSB0",
        default = "/dev/ttyUSB0"
    )
    parser.add_option_group(group)


    # output group
    group = optparse.OptionGroup(parser, "Output")
    group.add_option("-d", "--debug",
        dest = "debug",
        help = "Show debug output. And override loglevel with debug.",
        action = "store_true",
        default = False
    )
    parser.add_option_group(group)

    (options, args) = parser.parse_args(unparsed_args)

    return options
    # end parser


def main(args):
    # get options
    options = parser(args)

    # adapt log level
    loglevel = logging.WARNING
    if (options.debug):
        loglevel = logging.DEBUG
    # end if

    # logging
    logging.basicConfig(level = loglevel, format = '%(asctime)-8s [%(levelname)s]: %(message)s', datefmt = '%H:%M:%S')

    logging.debug("Options: %s", str(options))

    return do_probe(options.esp_port)
# end main


if __name__ == '__main__':
      sys.exit(main(sys.argv))
