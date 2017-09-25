#!/usr/bin/env python3
#
# Terminal program to connect to encrypted netrepl
#
# author: ulno
# create date: 2017-09-16
#
# based on telnet example at http://www.binarytides.com/code-telnet-client-sockets-python/

_debug = "terminal:"

import select, threading
from netrepl import Netrepl_Parser

input_buffer = ""
input_buffer_lock=threading.Lock()
quit_flag = False

# from: https://stackoverflow.com/questions/510357/python-read-a-single-character-from-the-user
# and https://github.com/magmax/python-readchar
# TODO: use whole readchar to support windows?

import tty, sys, termios  # raises ImportError if unsupported
#def readchar():
#    fd = sys.stdin.fileno()
#    old_settings = termios.tcgetattr(fd)
#    try:
#        tty.setraw(sys.stdin.fileno())
#        ch = sys.stdin.read(1)
#    finally:
#        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
#    return ch

def readchar():
    ch = sys.stdin.read(1)
    return ch

def char_reader():
    # thread reading from stdin into inputbuffer
    global input_buffer, input_buffer_lock, quit_flag
    while not quit_flag:
        ch = readchar()
        if ch=="\x1d":
            quit_flag=True
            ch=""
        elif ch=="\r":
            ch="\r\n"
        input_buffer_lock.acquire()
        input_buffer += ch
        input_buffer_lock.release()


def getChar():
    answer = sys.stdin.read(1)
    return answer

def main():
    global quit_flag, input_buffer_lock, input_buffer

    parser = Netrepl_Parser('Connect to netrepl and open an'
                            'interactive terminal.',debug=_debug)


    con = parser.connect()
    if _debug is not None:
        print(_debug, 'Press ctrl-] to quit.\n')
    if _debug: print(_debug,'Requesting startscreen.')
    con.send(b"\r\nhelp\r\n")

    fd = sys.stdin.fileno()
    #old_settings = termios.tcgetattr(fd)
    tty.setraw(sys.stdin.fileno())

    input_thread=threading.Thread(target=char_reader)
    input_thread.start()

    while not quit_flag:
        # Check if we received anything via network
        # Get the list sockets which are readable
        read_sockets, write_sockets, error_sockets = \
            select.select([con.socket], [], [], 0.01)

        for sock in read_sockets:
            # incoming message from remote server
            if sock == con.socket:
                data = con.receive()
                l=len(data)

                # TODO: figure out how to detect connection close
                # #print("recvd:",data)
                # if not data:
                #     print('\nterminal: Connection closed.')
                #     sys.exit()
                # else:
                if l>0:
                    # print data
                    try:
                        sys.stdout.write(bytes(data[0:l]).decode())
                    except:
                        if _debug:
                            print("\r\n{} Got some weird data of len "
                                  "{}: >>{}<<\r\n".format(_debug,len(data),data))
                    #print("data:", str(data[0:l]))
                    sys.stdout.flush()

        if len(input_buffer)>0:
            # user entered a message
            input_buffer_lock.acquire()
            send_buffer=input_buffer.encode()
            input_buffer=""
            input_buffer_lock.release()
            #msg = sys.stdin.readline().strip()+'\r\n'
            #print("\r\nmsg {} <<\r\n".format(send_buffer))
            con.send(send_buffer)
            #cs.send(send_buffer+b'\r\n')
            #cs.send(send_buffer+b'!')

    input_thread.join()  # finsih input_thread
    if _debug: print("\r\n{} Closing connection.\r".format(_debug))
    con.repl_normal()  # normal repl
    con.close(report=True)
    if _debug: print("\r\n{} Connection closed.\n".format(_debug))
    tty.setcbreak(sys.stdin.fileno())
    #termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)


# main function
if __name__ == "__main__":
    main()

