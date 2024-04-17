#!/bin/env python
"""
A simple shell replacement for doing the most important tasks in iotempower
in combination with cloudcmd.
Turns out anything using ncurses in python on raspberry pi is way to slow
and pypy3 is not supported in raspbian
TODO: retire this
"""
from __future__ import unicode_literals
from prompt_toolkit import HTML
from prompt_toolkit.application import Application
from prompt_toolkit.application.current import get_app
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.key_binding.bindings.focus import focus_next, focus_previous
from prompt_toolkit.layout import VSplit, HSplit, Layout, Window
from prompt_toolkit.styles import Style
from prompt_toolkit.widgets import Button, Box, TextArea, Label, Frame

import os

result = None

# Event handlers for all the buttons.
def button1_clicked():
    global result
    result = 'Button 1 clicked'
    get_app().exit()


def button2_clicked():
    global result
    result = 'Button 2 clicked'
    get_app().exit()


def button3_clicked():
    global result
    result = 'Button 3 clicked'
    get_app().exit()


def button4_clicked():
    global result
    result = 'Button 4 clicked'
    get_app().exit()


def exit_clicked():
    global result
    result = 'Exit clicked'
    get_app().exit()


# All the widgets for the UI.
button1 = Button('Deploy (D)', width=18, handler=button1_clicked)
button2 = Button('Initialize (I)',  width=18, handler=button2_clicked)
button3 = Button('Upgrade (U)',  width=18, handler=button3_clicked)
button4 = Button('Advanced (A)',  width=18, handler=button4_clicked)
button_cancel = Button('Exit (X)',  width=30, handler=exit_clicked)
#text_area = TextArea(focusable=True)

# determine execution path
if 'ACTIVE_DIR' in os.environ:  # check environment variable of cloudcmd
    selected_path = os.environ['ACTIVE_DIR']
else:
    selected_path = os.path.abspath(os.path.curdir)
    if 'HOME' in os.environ:  # eventually strip home
        home = os.environ['HOME'] + "/"
        if selected_path.startswith(home):
            selected_path = selected_path[len(home):]

root_container = Box(HSplit([
    Box(
        Label(text=HTML('<bold>ul<ansired>no</ansired>iot</bold> Simple Shell'),
            dont_extend_width=True),  
        padding_top=1, padding_bottom=1, style="bg:#ffffff fg:#000000"),
    Box(
        Label(text='Use mouse, arrow keys, enter, or shortcut key to select.',
            dont_extend_width=True),  
        padding_top=0, padding_bottom=1, style="bg:#ffffff fg:#000000"),
    Box(HSplit([
        Label(text='The selected action will be executed in this path:',
            dont_extend_width=True),
        Label(text=selected_path, dont_extend_width=True),
        ]), 
        padding_top=0, padding_bottom=0, style='class:box'),
    Box(HSplit([
            Box(VSplit([
                Box(Frame(body=button1, style='class:button-frame')), 
                Box(Frame(body=button2, style='class:button-frame')),
            ]), padding=0),
            Box(VSplit([
                Box(Frame(body=button3, style='class:button-frame')), 
                Box(Frame(body=button4, style='class:button-frame')),
            ]), padding=0),
            Box(Frame(body=button_cancel, style='class:button-frame'), 
                padding_top=1, padding_bottom=1, style="bg:#ffffff fg:#000000")
        ]),
        padding=0,
        style='class:box')
]), padding=0)

layout = Layout(
    container=root_container,
    focused_element=button_cancel)


# Key bindings.
kb = KeyBindings()
kb.add('tab')(focus_next)
kb.add('s-tab')(focus_previous)


# Styling.
style = Style([
    ('box', 'bg:#ffffeb fg:#404040'),
    ('button-frame', 'bg:#e0e0e0 fg:#202020'),
    ('button', '#000000'),
#    ('button.arrow', 'bg:#0000ff fg:#00ff00'),
    ('button.focused', 'bg:#00b000 fg:#00b000'),
    ('text-area focused', 'bg:#ff0000'),
])

# does not work either in before_render nor in after_render
#def hide_cursor(_):
#    global application
#    application.output.hide_cursor()

# Build a main application object.
application = Application(
    layout=layout,
    key_bindings=kb,
    style=style,
    mouse_support=True,
    full_screen=True,
    max_render_postpone_time=0.5)

# This does hide the cursor, tip from Jonathan Lenders:
# https://github.com/prompt-toolkit/python-prompt-toolkit/issues/827
application.output.show_cursor = lambda:None


def main():
    global result
    application.run()
    print("Result:", result)

if __name__ == '__main__':
    main()
