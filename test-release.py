#!/usr/bin/env python
import os, sys, subprocess, shutil, distutils.dir_util, argparse
from utilities import *

# Define command line interface:
argparser = argparse.ArgumentParser()
argparser.add_argument("-qt", type=int, default=0,
                       help="Specify a specific qt version (e.g. 474 or 502) to test.")
argparser.add_argument("-p", "--pause", action="store_true",
                       help="Pause after every qt version.")
argparser.add_argument("-s", "--short", action="store_true",
                       help="Only test the plots example.")
argparser.add_argument("-i", "--interactive", action="store_true",
                       help="Keep tests open for user interaction and continue test only after user quits them.")
argparser.add_argument("-nr", "--noqcpobjectreuse", action="store_true",
                       help="Normally the qcustomplot.o file is compiled only once per Qt version tested, and reused for all test projects. With this flag set, the object is compied for each test project separately.")
config = argparser.parse_args()

execTestSuffix = "& sleep 1; kill $!"  # appended to test execution command line. Starts test executable in background and quits it after one second (if executable fails, kill will fail and thus propagate !=0 return value)
if config.interactive:
    execTestSuffix = ""
qcpObjectDir = ""  # points to the path of the compiled qcustomplot.o, so subsequent examples can pull them in (except if -nr argument passed)


def run_example(examplePath, executableName):
    global qcpObjectDir
    workingdir = os.getcwd()
    printinfo(examplePath)
    os.chdir(examplePath)
    if not config.noqcpobjectreuse:
        if qcpObjectDir:
            shutil.copy2(os.path.join(qcpObjectDir, "qcustomplot.o"), "./")
            shutil.copy2(os.path.join(qcpObjectDir, "moc_qcustomplot.o"), "./")
        else:
            qcpObjectDir = os.getcwd()
    run_qmake_make(qmakecommand)
    shellcall("./"+executableName+execTestSuffix, error="Execution unsuccessful")
    os.chdir(workingdir)


# main test loop:
qmakeVersions = list_qmakes()
printinfo("QCustomPlot Release Package Test")
printinfo("Found Qt versions: "+" ".join(list(map(lambda s: s[5:], qmakeVersions))))
if config.qt > 0:
    qmakeVersions = ["qmake"+str(config.qt)]
for qmakecommand in qmakeVersions:
    qcpObjectDir = ""  # make sure to compile qcustomplot objects new for every qt version, if -r flag set
    try:
        qmakeproc = subprocess.Popen([qmakecommand, "-v"], stdout=subprocess.PIPE)
        printinfo(qmakeproc.stdout.readlines()[1].rstrip())  # print used Qt version in this run
    except:
        printinfo("Qt version of '"+qmakecommand+"' not found, skipping.")
        continue
    baseDir = sys.path[0]
    tempDir = baseDir+"/temp"
    os.mkdir(tempDir)

    # ================== QCustomPlot full ==================
    printinfo("QCustomPlot full:")
    os.chdir(tempDir)
    shutil.copy2(baseDir+"/QCustomPlot.tar.gz", "./")
    shellcall("tar -xf QCustomPlot.tar.gz", error="Failed to untar QCustomPlot.tar.gz", terminate=True)
    os.chdir(tempDir+"/qcustomplot")
    # test examples:
    run_example("examples/plots", "plot-examples")
    if not config.short:
        run_example("examples/axis-tags", "axis-tags-example")
        run_example("examples/interactions", "interaction-example")
        run_example("examples/text-document-integration", "text-document-integration")
        run_example("examples/scrollbar-axis-range-control", "scrollbar-axis-range-control")
    os.chdir(tempDir)

    # ================== QCustomPlot-sharedlib and -source package ==================
    if not config.short:
        printinfo("QCustomPlot sharedlib and source:")
        os.chdir(tempDir)
        shutil.copy2(baseDir+"/QCustomPlot-sharedlib.tar.gz", "./")
        shellcall("tar -xf QCustomPlot-sharedlib.tar.gz", error="Failed to untar QCustomPlot-sharedlib.tar.gz", terminate=True)
        shutil.copy2(baseDir+"/QCustomPlot-source.tar.gz", "./")
        shellcall("tar -xf QCustomPlot-source.tar.gz", error="Failed to untar QCustomPlot-source.tar.gz", terminate=True)
        shutil.copy2(tempDir+"/qcustomplot-source/qcustomplot.h", tempDir+"/")  # copy qcp source to dir above sharedlib dir (sharedlib code expects it two dirs above source files)
        shutil.copy2(tempDir+"/qcustomplot-source/qcustomplot.cpp", tempDir+"/")  # copy qcp source to dir above sharedlib dir (sharedlib code expects it two dirs above source files)
        # sharedlib compile:
        printinfo("sharedlib-compilation")
        os.chdir(tempDir+"/qcustomplot-sharedlib/sharedlib-compilation")
        run_qmake_make(qmakecommand)
        shellcall("cp libqcustomplot* ../sharedlib-usage")
        # sharedlib use:
        printinfo("sharedlib-usage")
        os.chdir(tempDir+"/qcustomplot-sharedlib/sharedlib-usage")
        run_qmake_make(qmakecommand)
        shellcall("export LD_LIBRARY_PATH=.; ./sharedlib-usage"+execTestSuffix, error="Execution unsuccessful")
        os.chdir(tempDir)

    # ================== Cleanup ==================
    answer = ""
    if config.pause:
        if qmakecommand == qmakeVersions[len(qmakeVersions)-1]:
            printinfo("Press any key to finish release test.")
        else:
            printinfo("Enter 'q' and hit return to stop release test, enter nothing to continue with next qt version...")
        answer = raw_input()
    os.chdir(baseDir)
    shutil.rmtree("./temp")
    printinfo("cleaned up")
    if answer == "q":
        break
  

