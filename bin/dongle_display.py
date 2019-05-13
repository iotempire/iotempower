#!/usr/bin/env python
#
# Communicate with the IoTempower esp dongle
# and display some text on the locally connected display

import sys
import os
import optparse
import logging

import serial
import time


def display_text(port, text):
    # Create a serial connection
    ser = serial.Serial(port, 115200, timeout=15);
    logging.info('Starting on %s.', port)
    ser.read_all()  # discard all
    ser.write(b"\n")
    ser.flush()
    answer = ser.read_until(b"UED>")
    if not answer.endswith(b"UED>"):
        sys.stderr.write("Trouble communicating with dongle.\n")
        return 1

    # show options
    sys.stderr.write("Sending: %s\n"%text)
    ser.write(b"display \""+ text.encode() + b"\"\n")
    ser.flush()

    ser.close()
    return 0


def parser(unparsed_args):
    parser = optparse.OptionParser(
        usage = "%prog [options]",
        description = "Use a IoTempower esp dongle's display to display some text."
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

    return options, args[1:]
    # end parser


def main(args):
    # get options
    options, args = parser(args)

    # adapt log level
    loglevel = logging.WARNING
    if (options.debug):
        loglevel = logging.DEBUG
    # end if

    # logging
    logging.basicConfig(level = loglevel, format = '%(asctime)-8s [%(levelname)s]: %(message)s', datefmt = '%H:%M:%S')

    logging.debug("Options: %s", str(options))

    return display_text(options.esp_port, " ".join(args))
# end main


if __name__ == '__main__':
      sys.exit(main(sys.argv))
