#!/usr/bin/env python
import sys, subprocess, os
from utilities import *

baseDir = sys.path[0]
os.chdir(baseDir)  # change current working dir to script dir

ret = shellcall("grep --color='auto' -rInP \"[\\x80-\\xFF]\" ./*;")

sys.exit(1 if not ret else 0)
