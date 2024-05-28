#!/usr/bin/env python
# Do a rough tokenization of the given input source file and check which
# of the given commands are used in this code.
# Prints out the found commands as a list of defines to be included.
# The commands-input file must give a command in each line with all its dependencies (including the command itself).

import io
import re
import sys
import os
import tokenize
import configparser # for ini-file


def parse_devices_ini(ini_file):
    config = configparser.ConfigParser(comment_prefixes=(';'))  # only use comments in lines starting with ; to allow lines startign with # to be code
    config.read(ini_file)
    return config

def soft_camelizer(s):
    return '_'.join(word.capitalize() for word in s.split('_'))


class TokenChecker:
    def __init__(self, filepath):
        with open(filepath, 'r') as f:
            code = f.read()
        code = re.sub(re.compile(r"/\*.*?\*/", re.DOTALL), "", code) # TODO: verify that r=raw works here
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
            commands = set(commands)
        found_commands = set()
        for command in commands:
            c=command.split(" ")[0] # only check first word - TODO: still necessary?
            if c in self.tokens:
                found_commands.add(command)
        return found_commands


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: command_checker.py </path/to/devices.ini> </path/to/setup.h> <output_dir>")
        sys.exit(1)

    devices_ini = sys.argv[1]
    filepath = sys.argv[2]
    output_dir = sys.argv[3]

    devices = parse_devices_ini(devices_ini)  # device configuration from ini file

    aliases_main = {} # reverse map for all devices to main device in ini file

    for d in devices:
        data = devices[d]
        aliases_main[d] = d
        aliases = data.get('aliases', '').split()  # other names for same device
        for a in aliases:
            aliases_main[a] = d
        

    checker = TokenChecker(filepath)
    found_commands = checker.check_for(aliases_main.keys())
    # have a default - TODO: might have to be removed for other MCUs
    found_commands.add("output")
    found_commands.add("input")
    found_commands.add("display")  # for dongle adoption mode

    # trim found_commands to only the main commands (remove aliases)
    trimmed_commands = set()
    for command in found_commands:
        trimmed_commands.add(aliases_main[command])
        
        def check_dependency(dev_name):
            depends = devices[dev_name].get('depends', '').split()  # other devices, this depends on
            for dep in depends:
                if not dep in trimmed_commands:
                    trimmed_commands.add(dep)
                    check_dependency(dep)

        check_dependency(aliases_main[command])


    # now generate files
    libs = set()
    libs_esp32 = set()
    filenames = set()
    with open(os.path.join(output_dir, "src", "devices_generated.h"), 'w') as devices_generated:
        for command in devices:
            if command in trimmed_commands:  # was found in setup.h
                print(f"// Begin: {command}", file=devices_generated)
                print(f"#define IOTEMPOWER_COMMAND_{command.upper()}", file=devices_generated)
                
                data = devices[command]
                aliases = data.get('aliases', '').split()
                
                filename = data.get('filename', None)
                if filename is None:
                    filename = command
                print(f"#include <dev_{filename}.h>", file=devices_generated)
                filenames.add(filename)
                
                lib = data.get('lib', None)
                if lib:
                    lib = lib.split(',')
                    libs.update(lib)
                
                lib_esp32 = data.get('lib_esp32', None)
                if lib_esp32:
                    lib_esp32 = lib_esp32.split(',')
                    libs_esp32.update(lib_esp32)
                
                extra_code = data.get('extra_code', None)
                if extra_code:
                    extra_code = extra_code.strip('"""').strip().replace("\\\n","\\\n    ")
                    print(extra_code, file=devices_generated)
                
                code = data.get('code', None)
                if code:
                    code = code.strip('"""').strip().replace("\\\n","\\\n    ")
                    print(code, file=devices_generated)
                else:  # if no default code is given, generate macros
                    class_name = data.get('class', None)
                    if class_name is None:
                        class_name = soft_camelizer(command)
                    print(f"#define {command}_(internal_name, ...) \\", file=devices_generated)
                    print(f"    IOTEMPOWER_DEVICE_({class_name}, internal_name, ##__VA_ARGS__)", file=devices_generated)
                    print(f"#define {command}(name, ...) IOTEMPOWER_DEVICE(name, {command}_, ##__VA_ARGS__)", file=devices_generated)
                    # TODO: think if aliases always should be generated
                    for a in aliases:
                        print(f"#define {a}_(gcc_va_args...) {command}_(gcc_va_args)", file=devices_generated)
                        print(f"#define {a}(gcc_va_args...) {command}(gcc_va_args)", file=devices_generated)
                
                print(f"// End: {command}", file=devices_generated)
                print(file=devices_generated)
    
    with open(os.path.join(output_dir, "platformio-libs.ini"), 'w') as libs_include:
        print("[common]", file=libs_include)
        if len(libs) > 0:
            print(f"  extra_lib_deps = ", file=libs_include)
            for l in libs:
                print(f"    {l}", file=libs_include)
        if len(libs_esp32) > 0:
            print(f"  extra_lib_deps_esp32 = ", file=libs_include)
            for l in libs_esp32:
                print(f"    {l}", file=libs_include)

print(" ".join(sorted(filenames)))
