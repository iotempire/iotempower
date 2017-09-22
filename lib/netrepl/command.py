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
    con.send(" ".join(parser.args.command).encode())
    con.repl_execute()

    quit_flag=False
    while not quit_flag:
        # Check if we received anything via network
        # Get the list sockets which are readable
        read_sockets, write_sockets, error_sockets = \
            select.select([con.socket], [], [], 0.2)

        for sock in read_sockets:
            # incoming message from remote server
            if sock == con.socket:
                while True:
                    data = con.receive()
                    if data is None: break
                    # print data
                    try:
                        sys.stdout.write(data.decode())
                    except:
                        if _debug:
                            print("\r\n{} Got some weird data of len "
                                  "{}: >>{}<<\r\n".format(_debug,len(data),data))
                    #print("data:", str(data[0:l]))
                    sys.stdout.flush()

    if _debug: print("\r\n{} Closing connection.\r".format(_debug))
    con.repl_normal()  # normal repl
    con.close()
    if _debug: print("\r\n{} Connection closed.\n".format(_debug))


# main function
if __name__ == "__main__":
    main()
