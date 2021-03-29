#!/usr/bin/env python
import re, os, sys, argparse
sys.path.insert(1, os.path.join(sys.path[0], '../..'))
from utilities import *

baseDir = sys.path[0]
os.chdir(baseDir)  # change current working dir to script dir

# Define command line interface:
argparser = argparse.ArgumentParser()
argparser.add_argument("-qt", type=int, default=0,
                       help="Specify a specific qt version (e.g. 474 or 502) to test.")
argparser.add_argument("-p", "--pause", action="store_true",
                       help="Pause after every qt version.")
config = argparser.parse_args()


def run_test():
    passed = 0
    failed = 0
    skipped = 0
    ignorePattern = re.compile(r"(Start testing of|Finished testing of|Config: Using|PASS)")
    totalsPattern = re.compile(r"Totals: (\d+) passed, (\d+) failed, (\d+) skipped")
    for line in shellcallListen("./autotest", error="Execution unsuccessful"):
        if not line.strip():
            continue
        if ignorePattern.search(line):
            continue
        m = totalsPattern.search(line)
        if m:
            passed += int(m.group(1))
            failed += int(m.group(2))
            skipped += int(m.group(3))
            continue
        printerror(line.strip()) # unexpected output -> error, skip, or other important output
    printinfo("{} passed, {} failed, {} skipped".format(passed, failed, skipped))
    shellcall("make clean -s", error="make clean unsuccessful")
    os.remove("./Makefile")


# main test loop:
qmakeVersions = list_qmakes()
if config.qt > 0:
    qmakeVersions = ["qmake"+str(config.qt)]


for qmakecommand in qmakeVersions:
    try:
        qmakeproc = subprocess.Popen([qmakecommand, "-v"], stdout=subprocess.PIPE)
        printinfo(qmakeproc.stdout.readlines()[1].rstrip())  # print used Qt version in this run
    except:
        printinfo("Qt version of '"+qmakecommand+"' not found, skipping.")
        continue
    printinfo("compiling...")
    run_qmake_make(qmakecommand)
    printinfo("testing...")
    run_test()
    answer = ""
    if config.pause:
        if qmakecommand == qmakeVersions[len(qmakeVersions)-1]:
            printinfo("Press any key to finish release test.")
        else:
            printinfo("Enter 'q' and hit return to stop release test, enter nothing to continue with next qt version...")
        answer = raw_input()
    if answer == "q":
        break

