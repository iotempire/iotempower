#!/usr/bin/env python
# Do a rough tokenization of the given input source file and check which
# of the given commands are used in this code.
# Prints out the found commands as a list of defines to be included.
# The commands-input file must give a command in each line with all its dependencies (including the command itself).

import io
import re
import sys
import tokenize

class TokenChecker:
    def __init__(self, filepath):
        with open(filepath, 'r') as f:
            code = f.read()
        code = re.sub(re.compile("/\*.*?\*/", re.DOTALL), "", code)
        code = re.sub(r"(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*?$)", "", code, flags=re.MULTILINE)
        self.tokens=set()
        try:
            for token in tokenize.tokenize(io.BytesIO(code.encode('utf-8')).readline):
                # Append the token value to the tokens list
                tok = token[1]
                if len(tok)>0 and tok[0].isalpha(): # only add tokens that start with letter
                    self.tokens.add(tok)
        except tokenize.TokenError:
            print(f"Error tokenizing {filepath}")
        self.filepath = filepath


    def check_for(self, commands):
        if isinstance(commands, str):
            commands = [commands]
        found_commands = []
        for command in commands:
            c=command.split(" ")[0] # only check first word
            if c in self.tokens:
                found_commands.append(command)
        return found_commands


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: command_checker.py <file_to_check> <commands_file>")
        sys.exit(1)

    filepath = sys.argv[1]
    commands_file = sys.argv[2]

    commands = {}

    with open(commands_file, 'r') as f:
        for line in f:
            words = line.strip().split(" ")
            command = words[0]
            dependencies = words[1:]
            commands[command] = dependencies

    checker = TokenChecker(filepath)
    found_commands = checker.check_for(commands.keys())

    includes=set()
    for command in found_commands:
        for dependency in commands[command]:
            includes.add(dependency)

    for include in includes:
        print(f"#define IOTEMPOWER_COMMAND_{include.upper()}")
