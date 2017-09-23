#!/usr/bin/env python3
#
# Program to copy a folder to a destination folder on the netrepl device
#
# author: ulno
# create date: 2017-09-23
#

import time
from netrepl import Netrepl_Parser
import sys, os, select

_debug="netrepl copy:"

def main():
    parser = Netrepl_Parser('Copy recursively a local file or directory to a '
                            'netrepl enabled device.',
                            debug=_debug)
    parser.parser.add_argument('-t','--timeout', nargs=1,
                               type=int, required=False, default = 600,
                               help='seconds to wait for process '
                                    'to finish (default 600s)')
    parser.parser.add_argument('-s','--src', '--source', nargs="?",
                               type=str, required=False, default="./",
                               help='source directory. '
                                    'Default: current directory (./). '
                                    'If source should be a directory it has to '
                                    'be postfixed by a slash (/).')
    parser.parser.add_argument('-d','--dest', '--destination',
                               type=str, required=False, nargs="?", default="/",
                               help='destination directory on netrepl device). '
                                    'Default: netrepl root (/) directory. '
                                    'If this is postfixed by a slash (/) and '
                                    'source is a directory copy only content '
                                    'of source into destination. If there is '
                                    'no slash, create teh source directory in '
                                    'destination.')
    parser.parser.add_argument('--del','--delete',
                               required=False, action="store_true",
                               help='if given, delete locally absent files '
                                    'also on destination.')
    parser.parser.add_argument('-n','--dry', '--dryrun',
                               required=False, action="store_true",
                               help='if given, only show what would be done, '
                                    'do not copy or delete actually anything.')
    parser.parser.add_argument('-x','--exclude',
                               required=False, type=str, nargs="+",
                               help='files or directories to exclude. '
                                    'A slash in the end is ignored. '
                                    'Multiple files and directories can be '
                                    'specified here.')
    parser.parser.add_argument('-o','--only-create','--create-only',
                               required=False, type=str, nargs="+",
                               help='files or directories to only create, '
                                    'when they are not yet present in '
                                    'destination. '
                                    'A slash in the end is ignored. '
                                    'Multiple files and directories can be '
                                    'specified here.')
    # TODO: what about symlinks?
    # TODO: should hashing be optional?

    con = parser.connect()
    print(parser.args) # debug, TODO: remove
    source=parser.args.src
    src_dir=os.path.dirname(source)
    if not src_dir.endswith("/"):
        src_dir += "/"
    src_file=os.path.basename(source)
    dest=parser.args.dest
    dest_base_dir = os.path.dirname(dest)
    dest_create_dir = os.path.basename(dest)
    # create source list
    if src_file: # not empty
        src_list=[src_file]
    else:
        # just got a directory, so let's generate it
        src_list=[]
        for root, dirs, files in os.walk(src_dir):
            if not root.endswith("/"):
                root += "/"
            root=root[len(src_dir):]
            for d in dirs:
                src_list.append(root+d+"/")
            for filename in files:
                path = root + filename  # skip ./
                src_list.append(path)
    # create destination list

    print(src_list)

    # get destination file-list with hashes
    data=con.repl_command("from ulnoiot.util import file_hashs; "
                          "file_hashs(root=\"{}\")".format(dest_base_dir),
                          timeoutms=parser.args.timeout*1000)
    dest_hashes={}
    for line in data.split(b"\n"):
        line=line[:-1]
        parts=line.split(b" ")
        dest_hashes[b" ".join(parts[1:])]=parts[0]
    print(dest_hashes)


    # if data is None:
    #     if _debug: print(_debug, 'Timeout occurred, data discarded.')
    # else:
    #     if _debug: print(_debug, 'Data successfully received.')

    if _debug: print("{} Closing connection.".format(_debug))
    con.repl_normal()  # normal repl
    con.close(report=True)
    if _debug: print("{} Connection closed.".format(_debug))

    # if data is None:
    #     sys.exit(1)  # not successful
    # else:
    #     if _debug: print(_debug,"Output follows starting from next line.")
    #     try:
    #         sys.stdout.write(data.decode())
    #     except:
    #         if _debug:
    #             print("{} Got some weird data of len "
    #                   "{}: >>{}<<".format(_debug, len(data), data))
    #     sys.stdout.flush()


# main function
if __name__ == "__main__":
    main()
