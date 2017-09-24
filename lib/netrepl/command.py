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
    parser.parser.add_argument('-t','--timeout',
                               type=int, required=False, default = 60,
                               help='Seconds to wait for command '
                                    'to finish (default 60)')
    parser.parser.add_argument('-c','--command',
                               type=str, nargs="+", required=True,
                               help='A command to execute remotely.')

    con = parser.connect()
    data=con.repl_command(" ".join(parser.args.command),
                          timeoutms=parser.args.timeout*1000,
                          interrupt=True)
    if data is None:
        if _debug: print(_debug, 'Timeout occurred, data discarded.')
    else:
        if _debug: print(_debug, 'Data successfully received.')
    if _debug: print("{} Closing connection.".format(_debug))
    con.repl_normal()  # normal repl
    con.close(report=True)
    if _debug: print("{} Connection closed.".format(_debug))
    if data is None:
        sys.exit(1)  # not successful
    else:
        if _debug: print(_debug,"Output follows starting from next line.")
        try:
            sys.stdout.write(data.decode())
        except:
            if _debug:
                print("{} Got some weird data of len "
                      "{}: >>{}<<".format(_debug, len(data), data))
        sys.stdout.flush()


# main function
if __name__ == "__main__":
    main()
