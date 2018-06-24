#!/usr/bin/env python
from utilities import *

baseDir = sys.path[0]
os.chdir(baseDir)  # change current working dir to script dir

foundnonascii = subprocess.call("grep --color='auto' -rInP \"[\\x80-\\xFF]\" ./*;", shell=True) == 0

sys.exit(1 if foundnonascii else 0)
