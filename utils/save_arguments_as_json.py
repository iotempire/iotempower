import argparse
import json

"""
This module is used to save output results from different scripts as json files to store the state.
You can use -h flag to see the help message
"""
parser = argparse.ArgumentParser(description="Saves arguments to json file")
parser.add_argument(
    "variables",
    metavar="args",
    type=str,
    nargs="+",
    help="arguments to pass to json",
)
parser.add_argument(
    "--path",
    "-p",
    dest="file_destination",
    default="arguments.json",
    help="Destination file to save the arguments",
)
parser.add_argument(
    "--splitter",
    "-s",
    dest="splitter",
    type=str,
    default="",
    help="Splitter to parse the argument key and values. Possible options are: ':' and '='. Ex: key=value",
)

args = parser.parse_args()
if args.splitter in [":", "="]:
    tuple_list = [tuple(arg.split(args.splitter)) for arg in args.variables]
    args_dict = dict(tuple_list)
else:
    args_dict = {arg: arg for arg in args.variables}

with open(args.file_destination, "w") as f:
    json.dump(args_dict, f, indent=2)
