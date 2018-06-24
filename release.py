#!/usr/bin/env python
import os, sys, shutil, distutils.dir_util
from utilities import *

baseDir = sys.path[0]
os.chdir(baseDir)  # change current working dir to script dir

tarcommand = "GZIP=\"-9\" tar -caf"  # -a means determine compressor by archive suffix
tarsuffix = ".tar.gz"

if raw_input("\033[1;31m"+"This will completely reset the current working directory.\n(Call 'git clean -dxn -e \".idea/\"' to see what will be deleted.)\nContinue? (y/n): "+"\033[1;m").lower() != 'y':
    printinfo("Aborted.")
    sys.exit(1)

# clean working dir:
printinfo("Cleaning working directory...")
shellcall("git clean -dxf -e \".idea/\"", error="Failed to clean working directory with git.", terminate=True)
# amalgamate sources:
printinfo("Amalgamating sources...")
shellcall("./run-amalgamate.sh")

# look for undocumented methods via script:
printinfo("Checking for undocumented methods...")
shellcall("./find-undocumented.py qcustomplot.cpp", error="Undocumented methods found in amalgamated sources!")
# look for non-ascii characters in code files via script:
printinfo("Checking for non-ascii characters...")
shellcall("./find-nonascii.py", error="Non-ASCII characters found in codebase!")

# generate documentation images:
printinfo("Generating documentation images...")
os.chdir("./documentation/doc-image-generator")
run_qmake_make("qmake474")
shellcall("./doc-image-generator", error="Failed to generate documentation images.", terminate=True)
os.chdir("../..")
# generate documentation:
printinfo("Compiling documentation...")
shellcall("./run-doxygen.sh")

# build release packages in temp directory:
print("")
tempDir = baseDir+"/temp"
os.mkdir(tempDir)

# full:
os.mkdir(tempDir+"/qcustomplot")
os.chdir(tempDir+"/qcustomplot")
printinfo("Building full QCustomPlot package")
distutils.dir_util.copy_tree(baseDir+"/documentation/html", "./documentation/html")
shutil.copy2(baseDir+"/documentation/qthelp/qcustomplot.qch", "./documentation/")
for f in [baseDir+"/qcustomplot.h", baseDir+"/qcustomplot.cpp", baseDir+"/GPL.txt", baseDir+"/changelog.txt"]:
    shutil.copy2(f, "./")
distutils.dir_util.copy_tree(baseDir+"/examples", "./examples")
os.chdir("./examples/plots")
shutil.rmtree("./screenshots")
os.chdir("../../")
shellcall("find . -name .gitignore -exec rm -f \"{}\" \;")
os.chdir(tempDir)
shellcall(tarcommand+" QCustomPlot"+tarsuffix+" *")
shutil.move("QCustomPlot"+tarsuffix, baseDir+"/")
shutil.rmtree("./qcustomplot")

# source only:
os.mkdir(tempDir+"/qcustomplot-source")
os.chdir(tempDir+"/qcustomplot-source")
printinfo("Building QCustomPlot-source package")
for f in [baseDir+"/qcustomplot.h", baseDir+"/qcustomplot.cpp", baseDir+"/GPL.txt", baseDir+"/changelog.txt"]:
    shutil.copy2(f, "./")
shellcall("find . -name .gitignore -exec rm -f \"{}\" \;")
os.chdir(tempDir)
shellcall(tarcommand+" QCustomPlot-source"+tarsuffix+" *")
shutil.move("QCustomPlot-source"+tarsuffix, baseDir+"/")
shutil.rmtree("./qcustomplot-source")

# shared lib:
os.mkdir(tempDir+"/qcustomplot-sharedlib")
os.chdir(tempDir+"/qcustomplot-sharedlib")
printinfo("Building QCustomPlot-sharedlib package")
distutils.dir_util.copy_tree(baseDir+"/sharedlib/", "./")
shellcall("find . -name .gitignore -exec rm -f \"{}\" \;")
os.chdir(tempDir)
shellcall(tarcommand+" QCustomPlot-sharedlib"+tarsuffix+" *")
shutil.move("QCustomPlot-sharedlib"+tarsuffix, baseDir+"/")
shutil.rmtree("./qcustomplot-sharedlib")

# clean up:
os.chdir(baseDir)
os.rmdir("./temp")
printinfo("done")


