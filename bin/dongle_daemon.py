#!/usr/bin/env python
#
# Communicate with the ulnoiot esp dongle
# implement some watchdog functions
# listen on stdin (can be connected to fifo)
# for commands (release, connect)
#

import sys
import os
import optparse
import logging

import serial
import time
import threading


class UED_Listener(threading.Thread):
    """
    UED serial listener
    """
    def __init__(self, port):
        threading.Thread.__init__(self)
        self.ser = None
        self.port = port
        self.connected = False
        self.running = False
        self.request_release = False

    def connect(self):
        if not self.connected:
            # Create a serial connection
            self.ser = serial.Serial(self.port, 2000000, timeout=1);
            logging.info('Starting on %s.', self.port)
            self.ser.read_all()  # discard all
            self.ser.write(b"\n")
            self.ser.flush()
            answer = self.ser.read_until(b"UED>")
            if not answer.endswith(b"UED>"):
                sys.stderr.write("Trouble communicating with dongle.\n")
                self.ser.close()
                return False
            self.connected = True
            sys.stderr.write("Connected to serial port.\n")
            return True

    def release(self):
        if self.connected:
            self.request_release = True # signal disconnect
            while self.running and self.connected: # wait until Thread reacts
                time.sleep(0.1)
            sys.stderr.write("Disconnected from serial port.\n")

    def quit(self):
        self.running = False

    def send_info(self):
        sys.stderr.write("Sending info.\n")
        self.ser.write("!info start\n")
        env="ULNOIOT_AP_NAME"
        if env in os.environ:
            self.ser.write("AP: %s"%os.environ(env))
        env="ULNOIOT_AP_IP"
        if env in os.environ:
            self.ser.write("IP: %s"%os.environ(env))
        self.ser.write("!info end\n")

    def listen(self):
        while self.running:
            if self.connected:
                if self.ser.in_waiting>0:
                    answer = self.ser.readline()
                    if not answer:  # timeout
                        sys.stderr.write("Trouble communicating with dongle.")
                        self.quit()
                        return
                    answer = answer.strip()
                    if answer == "!daemon_check":
                        self.send_info()
                    elif answer == "!shutdown":
                        pass # TODO: call shutdown
                if self.request_release:
                    self.request_release = False
                    self.ser.close()
                    self.ser = None
                    self.connected = False
        self.release()
        sys.stderr.write("Exiting...\n")

    def run(self):
        self.running = True
        self.listen()


def stdin_listener(ser_listener):
    while True:
        l = input("c)onnect, r)elease, q)uit:\n")
        l = l.lower()
        if l.startswith("c"):
            ser_listener.connect()
        elif l.startswith("r"):
            ser_listener.release()
        elif l.startswith("q"):
            ser_listener.quit()
            break
    return True


def parser(unparsed_args):
    parser = optparse.OptionParser(
        usage = "%prog [options]",
        description = "Listen to a ulnoiot esp dongle and" + \
                    "react to commands from the dongle or on stdin."
    )

    # Configuration
    group = optparse.OptionGroup(parser, "Configuration")
    group.add_option("-p", "--port",
        dest = "esp_port",
        type = "str",
        help = "ulnoiot esp dongle serial port (where the dongle is connected). Default /dev/ttyUSB0",
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

    ser_listener = UED_Listener(options.esp_port)

    ser_listener.connect()
    ser_listener.start()

    return stdin_listener(ser_listener)
# end main


if __name__ == '__main__':
    sys.exit(main(sys.argv))

