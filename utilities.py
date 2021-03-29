from __future__ import print_function
import os, sys, subprocess, re


# print function to error out:
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

# colored print/eprint functions:
def printinfo(message):
    print("\033[1;36m" + message + "\033[1;m")

def printerror(message):
    eprint("\033[1;31m" + message + "\033[1;m")


# build helpers:
def shellcall(command, error="", terminate=False):
    if subprocess.call(command, shell=True) != 0: # .call is like .Popen only that it waits till the process finishes
        if error:
            printerror(error)
        if terminate:
            sys.exit(1)
        return False
    return True

def shellcallListen(command, error="", terminate=False):
    p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    while(True):
        retcode = p.poll() # returns None while subprocess is running
        line = p.stdout.readline().decode('utf-8')
        yield line
        if retcode is not None:
            if retcode != 0:
                if error:
                    printerror(error)
                if terminate:
                    sys.exit(1)
            break

def run_qmake_make(qmakecommand, silent=True, threads=5): # silent prevents make from echoing commands it issues. It does not silent warnings or errors of the called compiler/linker
    shellcall(qmakecommand+" > /dev/null", error="qmake failed", terminate=True)
    shellcall("make {} -j{}".format("-s" if silent else "", threads), error="make failed", terminate=True)


def list_qmakes():
    paths = os.environ["PATH"].split(os.path.pathsep)
    result = []
    qmakeregex = re.compile(r"^qmake\d+$")
    for path in filter(os.path.isdir, paths):
        for f in filter(qmakeregex.match, os.listdir(path)):
            if os.access(os.path.join(path, f), os.X_OK):
                result.append(f)
    return sorted(result, key=lambda s: s if len(s)>8 else s[:-2]+'0'+s[-2:])  # lambda makes sure qmake5101 comes after qmake591

