#!/bin/env python3
# tail a file into a plot
#
# take parameter of filename
#
# Resources:
# - http://stackoverflow.com/questions/11874767/real-time-plotting-in-while-loop-with-matplotlib
# - from: https://lethain.com/tailing-in-python/
#
# Author: ulno
# Create date: 2017-04-30
#

import time
from optparse import OptionParser
import numpy as np
import matplotlib.pyplot as plt
import numbers

SLEEP_INTERVAL = 0.02

minx = None
maxx = None
miny = None
maxy = None
interval_start = None
average_sum = 0
last_average = None
average_count = 0
point_counter = 0


def add_point(x, y, c="blue"):
    global minx, maxx, miny, maxy, interval_start, average_sum, average_count
    if minx is None:
        minx = x
        maxx = x + 1
        # TODO: better init value for max
        miny = y
        maxy = y + 1
        interval_start = x
    else:
        minx = min(minx, x)
        maxx = max(maxx, x)
        miny = min(miny, y)
        maxy = max(maxy, y)
    plt.axis([minx, maxx, miny, maxy])
    plt.scatter(x, y, c=c)


def draw(x, y, interval, diff=False, c="blue"):
    global point_counter, last_average, average_sum, average_count, interval_start

    if not isinstance(x, numbers.Number) or not isinstance(y, numbers.Number):
        return  # don't draw if one is not a number

    if interval is not None:
        if interval_start == None and x is not None:
            interval_start = x
        if x > interval_start + interval:
            current_average = last_average
            if average_count > 0:
                current_average = average_sum / average_count
                point_counter += 1
                # print("x", x,"avg", average_sum, "count", average_count, current_average)
            if current_average is not None:
                if diff and last_average is not None:
                    add_point(interval_start + interval / 2.0,
                              current_average - last_average, c=c)
                else:
                    add_point(interval_start + interval / 2.0, current_average, c=c)
            last_average = current_average
            average_sum = 0
            average_count = 0
            interval_start += interval
        average_sum += y
        average_count += 1
    else:
        add_point(x, y, c=c)
        point_counter += 1


def parse_lineas_tuple(l):
    s = l.strip().split()
    if len(s) >= 2:
        retval = []
        for i in s:
            try:
                conv = float(i)
            except:
                conv = None
            retval.append(conv)
        return retval
    else:
        return None


def init(fin, column, interval, diff=False, c="blue"):
    global point_counter
    counter = 0
    for l in fin:
        t = parse_lineas_tuple(l)
        if t is not None and len(t) > column:
            draw(t[0], t[column], interval, diff=diff, c=c)
        counter += 1
        if counter % 1000 == 0:
            print("Read", counter, "lines,", point_counter, "valid points.")
    plt.ion()
    plt.show()


def tail(fin):
    "Listen for new lines added to file."
    while True:
        where = fin.tell()
        line = fin.readline()
        if not line:
            plt.pause(SLEEP_INTERVAL)
            fin.seek(where)
        else:
            yield line


def main():
    p = OptionParser("usage: liveplot.py file [color [[column] [interval for averaging [differential:diff ]]]]")
    (options, args) = p.parse_args()
    if len(args) < 1:
        p.error("must at least specify a file to watch")
    with open(args[0], 'r') as fin:
        column = 1
        color = "blue"
        interval = None
        if len(args) > 1:
            color = args[1]
        if len(args) > 2:
            column = int(args[2])
        if len(args) > 3:
            interval = float(args[3])
        if len(args) > 4:
            diff = args[4].lower().startswith("diff")
        else:
            diff = False
        init(fin, column, interval, diff=diff, c=color)
        print("Read", point_counter, "valid points.")
        print("Reached file end. If valid points are 0,\nno graphics is shown until there are valid points.")
        for line in tail(fin):
            p = parse_lineas_tuple(line)
            if p is not None:
                draw(p[0], p[column], interval, diff=diff, c=color)
                plt.pause(SLEEP_INTERVAL)


if __name__ == '__main__':
    main()
