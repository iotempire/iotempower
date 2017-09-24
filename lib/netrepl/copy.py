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
from hashlib import sha256
import binascii

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
    parser.parser.add_argument('--delete','--del',
                               required=False, action="store_true",
                               help='if given, delete locally absent files '
                                    'also on destination.')
    parser.parser.add_argument('-n','--dry', '--dryrun',
                               required=False, action="store_true",
                               help='if given, only show what would be done, '
                                    'do not copy or delete actually anything.')
    parser.parser.add_argument('-x','--exclude', '--ignore',
                               required=False, type=str, nargs="+", default="",
                               help='files or directories to exclude. '
                                    'Tries to match path or filename '
                                    '(like rsync).'
                                    'Multiple files and directories can be '
                                    'specified here.')
    parser.parser.add_argument('-o','--only-create','--create-only',
                               required=False, type=str, nargs="+",default="",
                               help='files or directories to only create, '
                                    'when they are not yet present in '
                                    'destination. '
                                    'A slash in the end is ignored. '
                                    'Multiple files and directories can be '
                                    'specified here.')
    # TODO: what about symlinks?
    # TODO: should hashing be optional?

    con = parser.connect()
    source=parser.args.src
    src_dir=os.path.dirname(source)
    if not src_dir.endswith("/"):
        src_dir += "/"
    src_file=os.path.basename(source)
    dest=parser.args.dest
    dest_base_dir = os.path.dirname(dest)
    if not dest_base_dir.endswith("/"):
        dest_base_dir += "/"
    dest_prefix_dir = os.path.basename(dest)
    if dest_prefix_dir: dest_prefix_dir+="/"

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

    # get destination file-list with hashes
    if _debug: print("{} Requesting destination hash list.".format(_debug))
    data=con.repl_command("from ulnoiot.util import file_hashs; "
                          "file_hashs(root=\"{}\")".format(dest_base_dir+dest_prefix_dir),
                          timeoutms=parser.args.timeout*1000)
    dest_hashes={}
    for line in data.split(b"\n"):
        line=line[:-1]
        parts=line.split(b" ")
        if len(parts) and parts[0]:
            dest_hashes[b" ".join(parts[1:]).decode()
                [len(dest_base_dir+dest_prefix_dir):]] \
                    = bytes(parts[0]).decode()

    #print(src_list) # debug
    #print(dest_hashes)  # debug

    # Compute copy, delete, exclude, create only lists
    exclude_list=parser.args.exclude
    only_create_list=parser.args.only_create
    # create copy_list
    copy_list=[]
    for f in src_list:
        # check if file needs to be excluded due to being on exclude list
        match=False
        for e in exclude_list:
            if e.startswith("/"): # only allow path matches
                e=e[1:]
                if e.endswith("/"):
                    if f.startswith(e):
                        match=True
                        break
                else:
                    if f.startswith(e+"/") or f==e:
                        match = True
                        break
            else: # allow also file match
                if e.endswith("/"):
                    if f.startswith(e):
                        match=True
                        break
                else:
                    if f.startswith(e+"/"):
                        match = True
                        break
                    if os.path.basename(f) == e or f==e:
                        match = True
                        break
        # check if file needs to be excluded due to being in only-create
        match=False
        for o in only_create_list:
            if o.startswith("/"):
                if o[1:] == f:
                    match = True
                    break
            if f==o or os.path.basename(f)==o:
                match=True
                break
        if match: # is in create_only list, so check if it's already at destin.
            if f in dest_hashes:
                continue  # then we don't need to add it
        # nothing matches, so add it
        copy_list.append(f)

    # create delete list
    delete_list=[]
    if parser.args.delete:
        for f in dest_hashes:
            if f not in src_list:
                delete_list.append(f)

    not_dryrun=not parser.args.dry
    # process delete list
    for f in delete_list:
        print("Deleting {}.".format(f))
        if not_dryrun:
            pass # TODO: delete
    # process copy list
    for f in copy_list:
        h = sha256()
        hf = open(src_dir + f, "rb")
        while True:
            b = hf.read(1024)
            if len(b) == 0: break
            h.update(b)
        digest=binascii.hexlify(h.digest()).decode()
        if digest == dest_hashes[f]:
            print("Skipping {} due to same hash.".format(f))
        else:
            print("Copying {}.".format(f))
            if not_dryrun:
                pass # TODO: copy

    if _debug: print("{} Closing connection.".format(_debug))
    con.repl_normal()  # normal repl
    con.close(report=True)
    if _debug: print("{} Connection closed.".format(_debug))

# main function
if __name__ == "__main__":
    main()
