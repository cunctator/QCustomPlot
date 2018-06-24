#!/usr/bin/env python
import re, sys

if len(sys.argv) != 2:
    print("provide code file as first argument")
    exit()

f = open(sys.argv[1], "r")
line = f.readline()

foundProblem = False
hasdoc = False
linenumber = 0
while line:
    linenumber = linenumber + 1
    # warn for undocumented functions and ctors:
    if (re.search(r"^[<>\w]+ ", line) or re.search(r"^[<>\w]+::[^~]", line)) and not hasdoc:
        print("line "+str(linenumber)+": "+line[0:-1])
        foundProblem = True
    # warn for documented dtors:
    if (re.search(r"^\w+::~", line)) and hasdoc:
        print("dtor has doc, line "+str(linenumber)+": "+line[0:-1])
        foundProblem = True
    # determine for next loop iteration/line whether it has doc before it
    hasdoc = line.startswith("*/") or line.startswith("/*")
    line = f.readline()

f.close()

sys.exit(1 if foundProblem else 0)

