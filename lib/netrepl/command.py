#!/usr/bin/env python3
#
# Program to run a command via netrepl on a destination host
#
# author: ulno
# create date: 2017-09-16
#

import time
from netrepl import Netrepl_Parser
import sys, select

_debug="netrepl command:"

def main():
    parser = Netrepl_Parser('Connect to netrepl and run'
                            'a command.',
                            debug=_debug)
    parser.parser.add_argument('-c','--command',
                               type=str, nargs="+", required=True,
                               help='A command to execute remotely.')

    con = parser.connect()
    if _debug: print(_debug,'Sending command.')
    con.repl_interrupt()
    con.repl_raw()
    con.read_until(b"raw REPL; CTRL-B to exit\r\n>", timeoutms=2000)
    con.send(" ".join(parser.args.command).encode())
    con.repl_execute()
    con.read_until(b"OK", timeoutms=2000)  # wait for output start
    data=con.read_until(b"\x04\x04>", timeoutms=60000)
    try:
        sys.stdout.write(data.decode())
    except:
        if _debug:
            print("\r\n{} Got some weird data of len "
                  "{}: >>{}<<\r\n".format(_debug, len(data), data))
    sys.stdout.flush()

    if _debug: print("\r\n{} Closing connection.\r".format(_debug))
    con.repl_normal()  # normal repl
    con.close(report=True)
    if _debug: print("\r\n{} Connection closed.\n".format(_debug))


# main function
if __name__ == "__main__":
    main()
