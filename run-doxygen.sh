#!/bin/bash
cd "$( dirname "$0" )"

rm -rf documentation/html
mkdir documentation/html

rm -rf documentation/xml
mkdir documentation/xml

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

# generate SQL database statements for documentation links from xml:
echo Generating SQL doclinks...
./documentation/extract-doclinks.py 1>/dev/null

# cleanup html hierarchy:
rm -f documentation/html/index.qhp
rm -f documentation/html/*.map
rm -f documentation/html/*.md5
