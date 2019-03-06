#!/usr/bin/env python
#
# ulnoiot esp dongle adopt/ota driver
# based on espota.py, modified by ulno
#
# Original espota.py by Ivan Grokhotkov:
# https://gist.github.com/igrr/d35ab8446922179dc58c
#
# Modified since 2015-09-18 from Pascal Gollor (https://github.com/pgollor)
# Modified since 2015-11-09 from Hristo Gochkov (https://github.com/me-no-dev)
# Modified since 2016-01-03 from Matthew O'Gorman (https://githumb.com/mogorman)
# Modified to support ulnoiot dongle adopt by ulno starting 2019-02-35
#
# This script will push an initial OTA update through a ESP dongle in the
# ulnoiot environment
#
# Use it like this: python dongle_adopt.py -p <usbport> -f <firmware.bin>


import sys
import os
import optparse
import logging
import hashlib
import random

import serial
import time

# Commands
PROGRESS = False

# update_progress() : Displays or updates a console progress bar
## Accepts a float between 0 and 1. Any int will be converted to a float.
## A value under 0 represents a 'halt'.
## A value at 1 or bigger represents 100%
def update_progress(progress):
    if (PROGRESS):
        barLength = 40 # Modify this to change the length of the progress bar
        status = ""
        if isinstance(progress, int):
            progress = float(progress)
        if not isinstance(progress, float):
            progress = 0
            status = "error: progress var must be float\r\n"
        if progress < 0:
            progress = 0
            status = "Halt...\r\n"
        if progress >= 1:
            progress = 1
            status = "Done...\r\n"
        block = int(round(barLength*progress))
        text = "\rUploading: [{0}] {1}% {2}".format( "="*block + " "*(barLength-block), int(progress*100), status)
        sys.stderr.write(text)
        sys.stderr.flush()
    else:
        sys.stderr.write('.')
        sys.stderr.flush()

def serve(node_network, filename, port):
    # Create a serial connection
    ser = serial.Serial(port, 115200, timeout=15);
    logging.info('Starting on %s.', port)
    ser.read_all()  # discard all
    ser.write(b"\n")
    ser.flush()
    answer = ser.read_until(b"UED>")
    if not answer.endswith(b"UED>"):
        logging.debug(answer)
        sys.stderr.write("Trouble communicating with dongle.\n")
        ser.close()
        return 1

    content_size = os.path.getsize(filename)
    f = open(filename,'rb')
    file_md5 = hashlib.md5(f.read()).hexdigest()
    f.close()

    logging.info('Upload size: %d', content_size)

    ser.write(("adopt %s %d %s\n"%(node_network, content_size, file_md5)).encode())
    ser.flush()

    while(True):
        answer = ser.readline()
        logging.debug(answer)
        if not answer:
            sys.stderr.write("Trouble with dongle communication.\n")
            ser.close()
            f.close()
            return 1
        answer = answer.strip()
        if answer.startswith(b"!error"):
            sys.stderr.write("Error 10 from dongle: %s\n"%answer[7:].decode())
            ser.close()
            return 1
        if answer.startswith(b"!upload"):
            break

    received_ok = False

    f = open(filename, "rb")
    logging.info("Starting upload.")
    sys.stderr.write('\n')
    if (PROGRESS):
        update_progress(0)
    else:
        sys.stderr.write('\nUploading')
        sys.stderr.flush()
    offset = 0
    success = False
    while offset < content_size:
#        chunk = f.read(1460)
#        chunk = f.read(256) # bigger blocks crash the serial communication due to rxbuffersize
        chunk = f.read(1024) # make sure RXBuffer is at least this size
        if not chunk: break
        logging.debug("Sending chunk of size %d, offset %d, content_size %d.",
            len(chunk), offset, content_size)
        offset += len(chunk)
        update_progress(offset/float(content_size))
        ser.write(chunk)
        ser.flush()
        while True:
            answer = ser.readline()
            logging.debug(answer)
            if not answer:
                sys.stderr.write('Error uploading, no answer from dongle.\n')
                ser.close()
                f.close()
                return 1
            answer = answer.strip()
            if answer.startswith(b"!send more"):
                break;
            elif answer.startswith(b"!error"):
                sys.stderr.write("Error 20 from dongle: %s\n"%answer[7:].decode())
                ser.close()
                f.close()
                return 1
            elif answer.startswith(b"!success"):
                success = True
                break
            elif answer.startswith(b"Block"):
                pass # ignore
            else:  # unknown answer
                sys.stderr.write('Error uploading, wrong answer from dongle.\n')
                ser.close()
                f.close()
                return 1


    f.close()
    ser.close()
    sys.stderr.write('\n')
    if not success:
        sys.stderr.write('\nError finishing upload\n')
        return 1

    logging.info('Result: OK')
    return 0

    # end serve


def parser(unparsed_args):
    parser = optparse.OptionParser(
        usage = "%prog [options]",
        description = "Transmit initial image over the air over a dongle to the esp8266 module with OTA support."
    )

    # usb port
    group = optparse.OptionGroup(parser, "Destination")
    group.add_option("-p", "--port",
        dest = "esp_port",
        type = "str",
        help = "ulnoiot esp dongle serial port (where the dongle is connected). Default /dev/ttyUSB0",
        default = "/dev/ttyUSB0"
    )
    group.add_option("-n", "--node",
        dest = "node_network",
        type = "str",
        help = "Node network name to adopt (use scan to list). Must be supplied.",
        default = None
    )
    parser.add_option_group(group)

    # image
    group = optparse.OptionGroup(parser, "Image")
    group.add_option("-f", "--file",
        dest = "image",
        help = "Image file. Must be supplied.",
        metavar="FILE",
        default = None
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
    group.add_option("-r", "--progress",
        dest = "progress",
        help = "Show progress output. Does not work for ArduinoIDE",
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
#    loglevel = logging.DEBUG  # force debug

    # logging
    logging.basicConfig(level = loglevel, format = '%(asctime)-8s [%(levelname)s]: %(message)s', datefmt = '%H:%M:%S')

    logging.debug("Options: %s", str(options))

    # check options
    global PROGRESS
    PROGRESS = options.progress
    if not options.image or not options.node_network:
        logging.critical("Not enough arguments. Image and node have to be supplied.")

        return 1
    # end if

    return serve(options.node_network, options.image, options.esp_port)
# end main


if __name__ == '__main__':
      sys.exit(main(sys.argv))
