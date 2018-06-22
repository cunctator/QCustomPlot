#!/usr/bin/env python
import os, sys, subprocess, shutil, distutils.dir_util, argparse
sys.path.insert(1, os.path.join(sys.path[0], '../..'))
from utilities import *

# Define command line interface:
argparser = argparse.ArgumentParser()
argparser.add_argument("-qt", type=int, default=0,
                    help="Specify a specific qt version (e.g. 474 or 502) to test.")
argparser.add_argument("-p", "--pause", action="store_true",
                    help="Pause after every qt version.")
config = argparser.parse_args()

baseDir = sys.path[0];
os.chdir(baseDir) # change current working dir to script dir

def runTest():
  if subprocess.call("./autotest", shell=True) != 0:
    printerror("Execution unsuccessful")
  if subprocess.call("make clean -s", shell=True) != 0:
    printerror("make clean unsuccessful")
  os.remove("./Makefile")
    
# main test loop:
qmakeVersions = list_qmakes()
if (config.qt > 0):
  qmakeVersions = ["qmake"+str(config.qt)]


for qmakecommand in qmakeVersions:
  try:
    qmakeproc = subprocess.Popen([qmakecommand, "-v"], stdout=subprocess.PIPE)
    printinfo(qmakeproc.stdout.readlines()[1].rstrip()); # print used Qt version in this run
  except:
    printinfo("Qt version of '"+qmakecommand+"' not found, skipping.")
    continue
  printinfo("compiling...")
  run_qmake_make(qmakecommand)
  printinfo("testing...")
  runTest()
  answer = ""
  if config.pause:
    if qmakecommand == qmakeVersions[len(qmakeVersions)-1]:
      printinfo("Press any key to finish release test.")
    else:
      printinfo("Enter 'q' and hit return to stop release test, enter nothing to continue with next qt version...")
    answer = raw_input()
  if (answer == "q"):
    break

