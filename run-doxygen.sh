#!/bin/bash
cd "$( dirname "$0" )"
rm -r documentation/html
mkdir documentation/html

# let doxygen generate qhp index and html hierarchy from source files:
echo Running doxygen...
doxygen 1>/dev/null

# do post-processing on documentation images and html hierarchy:
echo Post-processing documentation images...
./documentation/postprocess-doc-images.py 1>/dev/null
echo Post-processing documentation html...
./documentation/postprocess-doc-html.py 1>/dev/null

# generate qhc file from qhp index and html for QtAssistant integration:
echo Building qhc...
qhelpgenerator documentation/html/index.qhp -o documentation/qthelp/qcustomplot.qch 1>/dev/null

# cleanup html hierarchy:
rm documentation/html/index.qhp
rm documentation/html/*.map
rm documentation/html/*.md5

