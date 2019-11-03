#!/usr/bin/env python
#
# Communicate with the IoTempower esp dongle
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
import psutil
import subprocess


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
            self.ser = serial.Serial(self.port, 115200, timeout=1);
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
        self.ser.close()
        try:
            os.kill(os.getpid(), 2) # send external ctrl-c to interrupt input
        except KeyboardInterrupt:
            pass

    def send_info(self):
        sys.stderr.write("Answering info request.\n")
        # send in format: $info ssid uptime mem_free load
        self.ser.write("$info ".encode())
        env="IOTEMPOWER_AP_NAME_FULL"
        if env in os.environ:
            self.ser_write("\"%s\" " %os.environ[env])
        else:
            self.ser_write("N/A ")

        # env="IOTEMPOWER_AP_IP"
        # if env in os.environ:
        #     self.ser.write("%s"%os.environ[env])

        uptime = time.time() - psutil.boot_time()
        if uptime < 3600:
            uptime = "%dmin" % ((uptime+30)//60)
        elif uptime < 3600 * 24 * 2:
            uptime = "%.1fh" % (uptime/3600)
        elif uptime < 3600 * 24 * 200:
            uptime = "%.1fd" % (uptime/3600/24)
        else:
            uptime = "%.1fy" % (uptime/3600/24/365.25)
        self.ser_write("%s "%uptime)

        memory = psutil.virtual_memory().available//1024//1024
        if memory > 1024:
            memory = "%.1fG" % (memory/1024)
        else:
            memory = "%dM" % memory
        self.ser_write("%s "%memory)

        cpu_util = round(psutil.cpu_percent())
        self.ser_write("%d%%\n"%cpu_util)
        self.ser.flush()

    def ser_write(self, text):
        self.ser.write(text.encode())

    def listen(self):
        while self.running:
            if self.connected:
                try:
                    if self.ser.in_waiting>0:
                        answer = self.ser.readline()
                        if not answer:  # timeout
                            sys.stderr.write("Trouble communicating with dongle.")
                            #self.quit()
                            return
                        answer = answer.strip()
                        if answer == b"!daemon_check":
                            self.send_info()
                        elif answer == b"!shutdown":
                            subprocess.run(["sudo","poweroff"],timeout=60)
                            self.quit()
                        else:
                            logging.debug(answer)
                except:
                   logging.error("Lost serial connection.")
                   sys.stderr.write("Lost serial connection exiting...\n")
                   self.quit()

                if self.request_release:
                    self.request_release = False
                    self.ser.close()
                    self.ser = None
                    self.connected = False
            time.sleep(0.1)  # prevent busy waiting
        self.release()
        sys.stderr.write("Exiting.\n")

    def run(self):
        self.running = True
        self.listen()



def stdin_listener(ser_listener, input_file):
    while True:
        try:
            if input_file == "-":
                l = input("c)onnect, r)elease, q)uit:\n")
            else:
                l = open(input_file).readline()
        except KeyboardInterrupt:
            print()
            sys.stderr.write("Input interrupted.\n")
            break
        l = l.lower()
        if l.startswith("c"):
            ser_listener.connect()
        elif l.startswith("r"):
            # TODO: start timer to automatically reconnect if host application died
             
            ser_listener.release()
        elif l.startswith("q"):
            ser_listener.quit()
            break
        time.sleep(0.1)  # prevent busy waiting


def parser(unparsed_args):
    parser = optparse.OptionParser(
        usage = "%prog [options]",
        description = "Listen to a IoTempower esp dongle and" + \
                    "react to commands from the dongle or on stdin."
    )

    # Configuration
    group = optparse.OptionGroup(parser, "Configuration")
    group.add_option("-p", "--port",
        dest = "esp_port",
        type = "str",
        help = "IoTEmpower Esp Dongle serial port (where the dongle is connected). Default /dev/ttyUSB0",
        default = "/dev/ttyUSB0"
    )
    group.add_option("-i", "--input",
        dest = "input",
        type = "str",
        help = "Redirect input from given file. Default: stdin",
        default = "-"
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

    if ser_listener.connect():
        ser_listener.start()

        stdin_listener(ser_listener, options.input) # this might be interrupted by ctrl-c
        try:
            ser_listener.quit()
        except KeyboardInterrupt:
            pass
    else:
        sys.stderr.write("Cannot connect to dongle.\n")
        return 1

    return 0
# end main


if __name__ == '__main__':
    sys.exit(main(sys.argv))

