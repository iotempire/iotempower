#!/usr/bin/env python
#
# Communicate with the ulnoiot esp dongle
# and scan for uiot-node networks

import sys
import os
import optparse
import logging

import serial
import time


def do_scan(port, cutoff):
    # Create a serial connection
    ser = serial.Serial(port, 460800, timeout=15);
    logging.info('Starting on %s.', port)
    ser.read_all()  # discard all
    ser.write(b"\n")
    ser.flush()
    answer = ser.read_until(b"UED>")
    if not answer.endswith(b"UED>"):
        sys.stderr.write("Trouble communicating with dongle.")
        return 1

    # show options
    sys.stderr.write("Scanning for nodes that can be adopted... ")
    ser.write(b"scan\n")
    ser.flush()
    if not ser.read_until(b"!network_scan start\r\n"):
        sys.stderr.write("Network scan failed.\n")
        return 1

    node_list = []
    while(True):
        l = ser.readline().strip()
        if (not b" " in l) or (l == b"!network_scan end"):
            break
        try:
            strength, name = l.split(b" ", 1)
            strength = int(strength)
            if strength >= cutoff:
                if name.startswith(b"uiot-node-"):
                    node_list.append((name.decode(), strength))
        except ValueError:
            pass

    sys.stderr.write("done.\n")

    if len(node_list) == 0:
        sys.stderr.write("No nodes that can be adopted found.\n")
        return 1

    # sort by strength
    node_list.sort(key=lambda x: (x[1],x[0]))

    sys.stderr.write("Following nodes found (ranked by strength):\n")
    for name,s in node_list:
        print("%s (%d)"%(name,s))

    # while True:
    #     n = input("Which one should be adopted? (enter number, default=1) ")
    #     if not n:
    #         n = 1
    #     else:
    #         try:
    #             n = int(n)
    #         except ValueError:
    #             n = -1
    #     n -= 1

    #     if n>=0 and n<len(node_list):
    #         break

    ser.close()


def parser(unparsed_args):
    parser = optparse.OptionParser(
        usage = "%prog [options]",
        description = "Use a ulnoiot esp dongle to scan for adoptable nodes and" + \
                    "list them in oder from strongest to weakest."
    )

    # Configuration
    group = optparse.OptionGroup(parser, "Configuration")
    group.add_option("-p", "--port",
        dest = "esp_port",
        type = "str",
        help = "ulnoiot esp dongle serial port (where the dongle is connected). Default /dev/ttyUSB0",
        default = "/dev/ttyUSB0"
    )
    group.add_option("-c", "--cutoff",
        dest = "cutoff",
        type = "int",
        help = "Decimal-value to use as detection of too weak networks. " + \
            "Anything weaker than this will be ignored. Default: -85",
        default = "-85"
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

    return do_scan(options.esp_port, options.cutoff)
# end main


if __name__ == '__main__':
      sys.exit(main(sys.argv))
