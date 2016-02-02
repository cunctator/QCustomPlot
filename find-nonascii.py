#!/usr/bin/env python
import sys, subprocess, os

baseDir = sys.path[0];
os.chdir(baseDir) # change current working dir to script dir

ret = subprocess.call("grep --color='auto' -rInP \"[\\x80-\\xFF]\" ./*;", shell=True);

sys.exit(1 if ret == 0 else 0);

