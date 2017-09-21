#!/usr/bin/env python3
#
# Terminal program to connect to encrypted netrepl
#
# Parameters: address [port] [key]
# key needs to be 256bit in either bytes or hex notation,
# if key is not given it is read as newline terminated 64 byte hex notation from stdin
#
# author: ulno
# create date: 2017-09-16
#
# based on telnet example at http://www.binarytides.com/code-telnet-client-sockets-python/

MAGIC = b"UlnoIOT-NetREPL:"

import socket, select, sys, os, time, threading
from crypt_socket import Crypt_Socket

input_buffer = ""
input_buffer_lock=threading.Lock()
quit_flag = False

# from: https://stackoverflow.com/questions/510357/python-read-a-single-character-from-the-user
# and https://github.com/magmax/python-readchar
# TODO: use whole readchar to support windows?

import tty, sys, termios  # raises ImportError if unsupported
def readchar():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
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

    if len(sys.argv) < 2:
        print('Usage : python terminal.py hostname [port] [key]')
        sys.exit(1)

    host = sys.argv[1]
    port = 23
    key = None
    if len(sys.argv) >= 2:
        if len(sys.argv[2])<10: # could be port
            port = int(sys.argv[2])
            if len(sys.argv) >= 3:
                key = sys.argv[3].encode()
        else:
            key = sys.argv[2].encode()

    if key is None:
        print('Enter key (32bytes as hex->64bytes):')
        key=sys.stdin.readline().strip().encode()
        if len(key)!=64:
            print("Key has to specified as 64 byte hex. Exiting.")
            sys.exit(1)
    if len(key)==64:
        key=bytes.fromhex(key)
    if len(key)!=32:
        print("Key doesn't resolve to 256bit (32 bytes). Exiting.")
        sys.exit(1)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(2) # block for 2s

    # connect to remote host
    try:
        s.connect((host, port))
    except:
        print('\nterminal: Unable to connect. Exiting.')
        sys.exit()

    print('\nterminal: Connected to remote host.')
    print('terminal: Sending plain text initialization and initialization vector.\n')
    iv_out = os.urandom(8)
    s.send(MAGIC+iv_out)
    print('terminal: Generating session key.\n')
    s.settimeout(0) # now, we can be non blocking
    cs = Crypt_Socket(s)
    cs.init_out(key,iv_out)
    key_in = os.urandom(32)
    iv_in = os.urandom(8)
    print('terminal: Sending initialization and session key.\n')

    cs.send(MAGIC+key_in+iv_in)  # send the key

    cs.init_in(key_in, iv_in)

    print('terminal: Waiting for connection.\n')
    print('terminal: press ctrl-] to quit.\n')

    #print('terminal: Requesting startscreen.')
    #time.sleep(2)
    #cs.send(b"help\r\n")

    input_thread=threading.Thread(target=char_reader)
    input_thread.start()

    while not quit_flag:
        # Check if we received anything via network
        # Get the list sockets which are readable
        read_sockets, write_sockets, error_sockets = \
            select.select([s], [], [], 0.01)

        for sock in read_sockets:
            # incoming message from remote server
            if sock == s:
                (data,l) = cs.receive()

                # TODO: figure out how to detect connection close
                # #print("recvd:",data)
                # if not data:
                #     print('\nterminal: Connection closed.')
                #     sys.exit()
                # else:
                if l>0:
                    # print data
                    sys.stdout.write(bytes(data[0:l]).decode())
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
            cs.send(send_buffer)
            #cs.send(send_buffer+b'\r\n')
            #cs.send(send_buffer+b'!')

    input_thread.join()
    print("Closing connection.")
    cs.close()

# main function
if __name__ == "__main__":
    main()
